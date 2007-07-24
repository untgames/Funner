#include <media/collada/geometry.h>

#include <stl/string>
#include <stl/vector>

using namespace medialib::collada;
using namespace common;

/*
    –еализаци€ surface
*/

namespace
{

//канал текстурировани€ вершин
template <class T> struct Channel
{
  T*          verts; //вершины
  stl::string name;  //им€ канала
  
  Channel  (size_t verts_count, const char* in_name) :
    verts ((T*)::operator new (sizeof (T) * verts_count)), name (in_name) {}

  ~Channel () { ::operator delete (verts); }
};

typedef Channel<TexVertex>             TexVertexChannel;
typedef Channel<math::vec3f>           ColorChannel;
typedef stl::vector<TexVertexChannel*> TexVertexChannelArray;
typedef stl::vector<ColorChannel*>     ColorChannelArray;

}

struct Surface::Impl
{
  collada::Material&      material;           //материал
  collada::PrimitiveType  primitive_type;     //тип примитивов
  size_t                  vertices_count;     //количество вершин
  size_t                  indices_count;      //количество индексов
  Vertex*                 vertices;           //вершины
  size_t*                 indices;            //индексы
  ColorChannelArray       color_channels;     //каналы вершинных цветов
  TexVertexChannelArray   texvertex_channels; //каналы текстурировани€
  
  enum {
    DEFAULT_TEXVERTEX_CHANNELS_RESERVE = 8, //резервируемое количество каналов текстурировани€
    DEFAULT_COLOR_CHANNELS_RESERVE     = 4  //резервируемое количество каналов вершинных цветов
  };
  
  void RemoveAllTextureChannels ()
  {
    for (TexVertexChannelArray::iterator i=texvertex_channels.begin (); i!=texvertex_channels.end (); ++i)
      delete *i;    
      
    texvertex_channels.clear ();
  }
  
  void RemoveAllColorChannels ()
  {
    for (ColorChannelArray::iterator i=color_channels.begin (); i!=color_channels.end (); ++i)
      delete *i;    
      
    color_channels.clear ();
  }  
  
  Impl (collada::Material& in_material, collada::PrimitiveType in_primitive_type, size_t in_vertices_count, size_t in_indices_count) : 
    material (in_material), vertices_count (in_vertices_count), indices_count (in_indices_count),
    primitive_type (in_primitive_type)
  {
    texvertex_channels.reserve (DEFAULT_TEXVERTEX_CHANNELS_RESERVE);
    color_channels.reserve (DEFAULT_COLOR_CHANNELS_RESERVE);
    
    switch (primitive_type)
    {
      case PrimitiveType_LineList:
      case PrimitiveType_LineStrip:
      case PrimitiveType_TriangleList:
      case PrimitiveType_TriangleStrip:
      case PrimitiveType_TriangleFan:
        break;
      default:
        RaiseInvalidArgument ("medialib::collada::Surface::Surface", "primitive_type", in_primitive_type);
        break;
    }
    
    vertices = (Vertex*)::operator new (vertices_count * sizeof (Vertex));
    
    try
    {
      indices  = (size_t*)::operator new (indices_count * sizeof (size_t));
    }
    catch (...)
    {
      ::operator delete (vertices);
      throw;
    }
  }
  
  ~Impl ()
  {
    RemoveAllTextureChannels ();
    RemoveAllColorChannels ();

    ::operator delete (vertices);
    ::operator delete (indices);
  }
};

/*
     онструктор / деструктор
*/

Surface::Surface (medialib::collada::Material& material, medialib::collada::PrimitiveType type, size_t verts_count, size_t indices_count)
  : impl (new Impl (material, type, verts_count, indices_count))
  {}

Surface::~Surface ()
{
  delete impl;
}

/*
    ћатериал поверхности
*/

Material& Surface::Material ()
{
  return impl->material;
}

const Material& Surface::Material () const
{
  return impl->material;
}

/*
    “ип примитивов
*/

PrimitiveType Surface::PrimitiveType () const
{
  return impl->primitive_type;
}

/*
     оличество вершин и индексов
*/

size_t Surface::VerticesCount () const
{
  return impl->vertices_count;
}

size_t Surface::IndicesCount () const
{
  return impl->indices_count;
}

/*
    –абота с вершинами
*/

Vertex* Surface::Vertices ()
{
  return impl->vertices;
}

const Vertex* Surface::Vertices () const
{
  return impl->vertices;
}

/*
    –абота с вершинными цветами
*/

size_t Surface::ColorChannelsCount () const
{
  return impl->color_channels.size ();
}

size_t Surface::CreateColorChannel (const char* name)
{
  if (!name)
    name = "";
    
  ColorChannel* channel = new ColorChannel (impl->vertices_count, name);
  
  try
  {    
    impl->color_channels.push_back (channel);
  
    return impl->color_channels.size () - 1;
  }
  catch (...)
  {
    delete channel;
    throw;
  }
}

void Surface::RemoveColorChannel (size_t channel)
{
  if (channel >= impl->color_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::RemoveColorChannel", "channel", channel, impl->color_channels.size ());
    
  delete impl->color_channels [channel];
  
  impl->color_channels.erase (impl->color_channels.begin () + channel);
}

void Surface::RemoveAllColorChannels ()
{
  impl->RemoveAllColorChannels ();
}

const char* Surface::ColorChannelName (size_t channel) const
{
  if (channel >= impl->color_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::ColorChannelName", "channel", channel, impl->color_channels.size ());
    
  return impl->color_channels [channel]->name.c_str ();
}

bool Surface::HasColorChannel (size_t channel) const
{
  return channel < impl->color_channels.size ();
}

int Surface::FindColorChannel (const char* name) const
{
  if (!name)
    RaiseNullArgument ("media::collada::Surface::FindColorChannel", "name");
    
  for (ColorChannelArray::const_iterator i=impl->color_channels.begin (), end=impl->color_channels.end (); i!=end; ++i)
    if ((*i)->name == name)
      return i - impl->color_channels.begin ();
      
  return -1;
}

const math::vec3f* Surface::Colors (size_t channel) const
{
  if (channel >= impl->color_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::Colors", "channel", channel, impl->color_channels.size ());
    
  return impl->color_channels [channel]->verts;
}

math::vec3f* Surface::Colors (size_t channel)
{
  if (channel >= impl->color_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::Colors", "channel", channel, impl->color_channels.size ());
    
  return impl->color_channels [channel]->verts;
}

/*
    –абота с текстурированными вершинами
*/

size_t Surface::TextureChannelsCount () const
{
  return impl->texvertex_channels.size ();
}

size_t Surface::CreateTextureChannel (const char* name)
{
  if (!name)
    name = "";
    
  TexVertexChannel* channel = new TexVertexChannel (impl->vertices_count, name);
  
  try
  {    
    impl->texvertex_channels.push_back (channel);
  
    return impl->texvertex_channels.size () - 1;
  }
  catch (...)
  {
    delete channel;
    throw;
  }
}

void Surface::RemoveTextureChannel (size_t channel)
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::RemoveTextureChannel", "channel", channel, impl->texvertex_channels.size ());
    
  delete impl->texvertex_channels [channel];
  
  impl->texvertex_channels.erase (impl->texvertex_channels.begin () + channel);
}

void Surface::RemoveAllTextureChannels ()
{
  impl->RemoveAllTextureChannels ();
}

const char* Surface::TextureChannelName (size_t channel) const
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureChannelName", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->name.c_str ();
}

bool Surface::HasTextureChannel (size_t channel) const
{
  return channel < impl->texvertex_channels.size ();
}

int Surface::FindTextureChannel (const char* name) const
{
  if (!name)
    RaiseNullArgument ("media::collada::Surface::FindTextureChannel", "name");
    
  for (TexVertexChannelArray::const_iterator i=impl->texvertex_channels.begin (), end=impl->texvertex_channels.end (); i!=end; ++i)
    if ((*i)->name == name)
      return i - impl->texvertex_channels.begin ();
      
  return -1;
}

const TexVertex* Surface::TextureVertices (size_t channel) const
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureVertices", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->verts;
}

TexVertex* Surface::TextureVertices (size_t channel)
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureVertices", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->verts;
}

/*
    »ндексы примитивов
*/

size_t* Surface::Indices ()
{
  return impl->indices;
}

const size_t* Surface::Indices () const
{
  return impl->indices;
}
