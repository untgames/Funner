#include "shared.h"

using namespace media::collada;
using namespace common;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список каналов поверхности
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ChannelListImpl: public Surface::IChannelList<T>
{
  enum { DEFAULT_CHANNELS_RESERVE = 8 };
  public:
    ChannelListImpl (size_t in_vertices_count) : vertices_count (in_vertices_count)
    {
      channels.reserve (DEFAULT_CHANNELS_RESERVE);
    }
    
    ChannelListImpl (const ChannelListImpl& impl) : vertices_count (impl.vertices_count)
    {
      channels.reserve (impl.channels.size ());
      
      for (ChannelArray::const_iterator i=impl.channels.begin (), end=impl.channels.end (); i!=end; ++i)
      {
        Channel* channel = new Channel (**i);
        
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
    size_t Size    () const { return channels.size (); }
    bool   IsEmpty () const { return channels.empty (); }
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Create (const char* name)
    {
      if (!name)
        RaiseNullArgument ("media::collada::Surface::IChannelList::Create", "name");
        
      Channel* channel = new Channel (vertices_count, name);
      
      try
      {    
        channels.push_back (channel);
      
        return channels.size () - 1;
      }
      catch (...)
      {
        delete channel;
        throw;
      }
    }
    
    void Remove (size_t channel)
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("media::collada::Surface::IChannelList::Remove", "channel", channel, channels.size ());
        
      delete channels [channel];
      
      channels.erase (channels.begin () + channel);
    }
    
    void Clear ()
    {
      for (ChannelArray::iterator i=channels.begin (); i!=channels.end (); ++i)
        delete *i;
        
      channels.clear ();      
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name (size_t channel) const
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("media::collada::Surface::IChannelList::Name", "channel", channel, channels.size ());
        
      return channels [channel]->name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск канала по имени. Возвращает номер канала, или -1 в случае неудачи
///////////////////////////////////////////////////////////////////////////////////////////////////
    int Find (const char* name) const
    {
      if (!name)
        RaiseNullArgument ("media::collada::Surface::IChannelList::Find", "name");
        
      for (ChannelArray::const_iterator i=channels.begin (), end=channels.end (); i!=end; ++i)
        if ((*i)->name == name)
          return i - channels.begin ();

      return -1;
    }
                
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    const T* Data (size_t channel) const
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("media::collada::Surface::IChannelList::Data", "channel", channel, channels.size ());
        
      return &channels [channel]->data [0];
    }

    T* Data (size_t channel)
    {
      return const_cast<T*> (const_cast<const ChannelListImpl&> (*this).Data (channel));
    }
  
  private:
    struct Channel
    {
      typedef stl::vector<T> Array;
      
      Array       data;
      stl::string name;

      Channel  (size_t verts_count, const char* in_name) : data (verts_count), name (in_name) {}
    };
    
    typedef stl::vector<Channel*> ChannelArray;
    
   private:     
     ChannelArray channels;
     size_t       vertices_count;
};

typedef ChannelListImpl<TexVertex>       TexVertexChannelListImpl;
typedef ChannelListImpl<math::vec3f>     ColorChannelListImpl;
typedef ChannelListImpl<VertexInfluence> InfluenceChannelListImpl;

}

/*
    Реализация surface
*/

typedef stl::vector<Vertex> VertexArray;
typedef stl::vector<size_t> IndexArray;

struct Surface::Impl: public xtl::reference_counter
{
  stl::string               material_name;      //имя материала
  collada::PrimitiveType    primitive_type;     //тип примитивов
  VertexArray               vertices;           //вершины
  IndexArray                indices;            //индексы
  ColorChannelListImpl      color_channels;     //каналы вершинных цветов
  TexVertexChannelListImpl  texvertex_channels; //каналы текстурирования
  InfluenceChannelListImpl  influence_channels; //каналы вершинных весов
  
  Impl (collada::PrimitiveType in_primitive_type, size_t in_vertices_count, size_t in_indices_count) :
    vertices (in_vertices_count), indices (in_indices_count), primitive_type (in_primitive_type),
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
        RaiseInvalidArgument ("media::collada::Surface::Surface", "primitive_type", in_primitive_type);
        break;
    }    
  }
};

/*
    Конструктор / деструктор
*/

Surface::Surface (media::collada::PrimitiveType type, size_t verts_count, size_t indices_count)
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
    RaiseNullArgument ("media::collada::Surface::SetMaterial", "name");
    
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

size_t Surface::VerticesCount () const
{
  return impl->vertices.size ();
}

size_t Surface::IndicesCount () const
{
  return impl->indices.size ();
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

size_t* Surface::Indices ()
{
  return &impl->indices [0];
}

const size_t* Surface::Indices () const
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
