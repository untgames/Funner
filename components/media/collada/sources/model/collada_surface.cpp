#include "shared.h"

using namespace media::collada;
using namespace common;

namespace
{

template <class T, class ChannelKey>
struct Channel
{
  typedef stl::vector<T> Array;

  Array      data;
  ChannelKey name;

  Channel (unsigned int verts_count, ChannelKey in_name) : data (verts_count), name (in_name) {}

  ChannelKey GetName ()
  {
    return name;
  }
};

template <class T>
struct Channel<T, const char*>
{
  typedef stl::vector<T> Array;

  Array       data;
  stl::string name;

  Channel (unsigned int verts_count, const char* in_name) : data (verts_count), name (in_name) {}

  const char* GetName ()
  {
    return name.c_str ();
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список каналов поверхности
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Key> class ChannelListImpl: public Surface::IChannelList<T, Key>
{
  enum { DEFAULT_CHANNELS_RESERVE = 8 };
  public:
    ChannelListImpl (unsigned int in_vertices_count) : vertices_count (in_vertices_count)
    {
      channels.reserve (DEFAULT_CHANNELS_RESERVE);
    }
    
    ChannelListImpl (const ChannelListImpl& impl) : vertices_count (impl.vertices_count)
    {
      channels.reserve (impl.channels.size ());
      
      for (typename ChannelArray::const_iterator i=impl.channels.begin (), end=impl.channels.end (); i!=end; ++i)
      {
        Channel<T, Key>* channel = new Channel<T, Key> (**i);
        
        try
        {
          channels.push_back (channel);
        }
        catch (...)
        {
          delete channel;
          throw;
        }
      }
    }
    
    ~ChannelListImpl ()
    {
      Clear ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество каналов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////      
    unsigned int Size    () const { return (unsigned int)channels.size (); }
    bool         IsEmpty () const { return channels.empty (); }
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Create (Key name)
    {
      static const char* METHOD_NAME = "media::collada::Surface::IChannelList::Create";

      try
      {
        CheckKeyName (name);

        if (channels.size () >= (unsigned int)-1)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't create new channel, channels count limit exceeded");

        Channel<T, Key>* channel = new Channel<T, Key> (vertices_count, name);

        try
        {
          channels.push_back (channel);

          return (unsigned int)channels.size () - 1;
        }
        catch (...)
        {
          delete channel;
          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch (METHOD_NAME);
        throw;
      }
    }
    
    void Remove (unsigned int channel)
    {
      if (channel >= channels.size ())
        throw xtl::make_range_exception ("media::collada::Surface::IChannelList::Remove", "channel", channel, channels.size ());
        
      delete channels [channel];
      
      channels.erase (channels.begin () + channel);
    }
    
    void Clear ()
    {
      for (typename ChannelArray::iterator i=channels.begin (); i!=channels.end (); ++i)
        delete *i;
        
      channels.clear ();      
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    Key Name (unsigned int channel) const
    {
      if (channel >= channels.size ())
        throw xtl::make_range_exception ("media::collada::Surface::IChannelList::Name", "channel", channel, channels.size ());
        
      return channels [channel]->GetName ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск канала по имени. Возвращает номер канала, или -1 в случае неудачи
///////////////////////////////////////////////////////////////////////////////////////////////////
    int Find (Key name) const
    {
      try
      {
        CheckKeyName (name);

        for (typename ChannelArray::const_iterator i=channels.begin (), end=channels.end (); i!=end; ++i)
          if ((*i)->name == name)
            return (int)(i - channels.begin ());

        return -1;
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::Surface::IChannelList::Find");
        throw;
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    const T* Data (unsigned int channel) const
    {
      if (channel >= channels.size ())
        throw xtl::make_range_exception ("media::collada::Surface::IChannelList::Data", "channel", channel, channels.size ());
        
      return &channels [channel]->data [0];
    }

    T* Data (unsigned int channel)
    {
      return const_cast<T*> (const_cast<const ChannelListImpl&> (*this).Data (channel));
    }

  private:
    template <class KeyType>
    static void CheckKeyName (KeyType name)
    {
    }

    template <typename KeyType>
    static void CheckKeyName (KeyType* name)
    {
      if (!name)
        throw xtl::make_null_argument_exception ("media::collada::Surface::IChannelList::CheckKeyName", "name");
    }
  
  private:
    typedef stl::vector<Channel<T, Key>*> ChannelArray;
    
   private:     
     ChannelArray channels;
     unsigned int vertices_count;
};

typedef ChannelListImpl<TexVertex, unsigned int>      TexVertexChannelListImpl;
typedef ChannelListImpl<math::vec3f, unsigned int>    ColorChannelListImpl;
typedef ChannelListImpl<VertexInfluence, const char*> InfluenceChannelListImpl;

}

/*
    Реализация surface
*/

typedef stl::vector<Vertex> VertexArray;
typedef stl::vector<unsigned int> IndexArray;

struct Surface::Impl: public xtl::reference_counter
{
  stl::string               material_name;      //имя материала
  collada::PrimitiveType    primitive_type;     //тип примитивов
  VertexArray               vertices;           //вершины
  IndexArray                indices;            //индексы
  ColorChannelListImpl      color_channels;     //каналы вершинных цветов
  TexVertexChannelListImpl  texvertex_channels; //каналы текстурирования
  InfluenceChannelListImpl  influence_channels; //каналы вершинных весов
  
  Impl (collada::PrimitiveType in_primitive_type, unsigned int in_vertices_count, unsigned int in_indices_count) :
    primitive_type (in_primitive_type), vertices (in_vertices_count), indices (in_indices_count),
    color_channels (in_vertices_count), texvertex_channels (in_vertices_count), influence_channels (in_vertices_count)
  {
    switch (primitive_type)
    {
      case PrimitiveType_LineList:
      case PrimitiveType_LineStrip:
      case PrimitiveType_TriangleList:
      case PrimitiveType_TriangleStrip:
      case PrimitiveType_TriangleFan:
        break;
      default:
        throw xtl::make_argument_exception ("media::collada::Surface::Surface", "primitive_type", in_primitive_type);
        break;
    }    
  }
};

/*
    Конструктор / деструктор
*/

Surface::Surface (media::collada::PrimitiveType type, unsigned int verts_count, unsigned int indices_count)
  : impl (new Impl (type, verts_count, indices_count), false)
  {}
  
Surface::Surface (const Surface& srf)
  : impl (srf.impl)
  {}
  
Surface::Surface (Impl* in_impl)
  : impl (in_impl, false)
  {}

Surface::~Surface ()
{
}

Surface& Surface::operator = (const Surface& srf)
{
  impl = srf.impl;

  return *this;
}

/*
    Создание копии
*/

Surface Surface::Clone () const
{
  return Surface (new Impl (*impl));
}

/*
    Имя материала поверхности
*/

const char* Surface::Material () const
{
  return impl->material_name.c_str ();
}

void Surface::SetMaterial (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::collada::Surface::SetMaterial", "name");
    
  impl->material_name = name;
}

/*
    Тип примитивов
*/

PrimitiveType Surface::PrimitiveType () const
{
  return impl->primitive_type;
}

/*
    Количество вершин и индексов
*/

unsigned int Surface::VerticesCount () const
{
  return (unsigned int)impl->vertices.size ();
}

unsigned int Surface::IndicesCount () const
{
  return (unsigned int)impl->indices.size ();
}

/*
    Работа с вершинами
*/

Vertex* Surface::Vertices ()
{
  return &impl->vertices [0];
}

const Vertex* Surface::Vertices () const
{
  return &impl->vertices [0];
}

/*
    Индексы примитивов
*/

unsigned int* Surface::Indices ()
{
  return &impl->indices [0];
}

const unsigned int* Surface::Indices () const
{
  return &impl->indices [0];
}

/*
    Работа с каналами вершинных цветов
*/

Surface::ColorChannelList& Surface::ColorChannels ()
{
  return impl->color_channels;
}

const Surface::ColorChannelList& Surface::ColorChannels () const
{
  return impl->color_channels;
}

/*
    Работа с каналами текстурированных вершин
*/

Surface::TexVertexChannelList& Surface::TexVertexChannels ()
{
  return impl->texvertex_channels;
}

const Surface::TexVertexChannelList& Surface::TexVertexChannels () const
{
  return impl->texvertex_channels;
}

/*
    Работа с каналами вершинных весов
*/

Surface::InfluenceChannelList& Surface::InfluenceChannels ()
{
  return impl->influence_channels;
}

const Surface::InfluenceChannelList& Surface::InfluenceChannels () const
{
  return impl->influence_channels;
}
