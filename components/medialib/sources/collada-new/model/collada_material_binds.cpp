#include <media/collada/scene.h>
#include <stl/hash_map>
#include <stl/string>

using namespace medialib::collada;
using namespace common;

/*
    Описание реализации присоединенного материала
*/

typedef stl::hash_map<size_t, stl::string> ChannelMap;

struct MaterialBinds::Impl
{
  ChannelMap channels; //карта каналов  
};

/*
    Конструктор / деструктор
*/

MaterialBinds::MaterialBinds ()
  : impl (new Impl)
  {}

MaterialBinds::~MaterialBinds ()
{
  delete impl;
}

/*
    Имя канала текстурированных вершин поверхности
*/

namespace
{

size_t get_hash (const char* material, const char* channel)
{
  return strhash (channel, strhash ("#", strhash (material)));
}

}

const char* MaterialBinds::SurfaceChannelName (const char* material, const char* texture_channel) const
{
  if (!material)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SurfaceChannelName", "material");

  if (!texture_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SurfaceChannelName", "texture_channel");

  ChannelMap::const_iterator iter = impl->channels.find (get_hash (material, texture_channel));

  return iter != impl->channels.end () ? iter->second.c_str () : "";
}

void MaterialBinds::SetSurfaceChannelName (const char* material, const char* texture_channel, const char* surface_channel)
{
  if (!material)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetSurfaceChannelName", "material");

  if (!texture_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetSurfaceChannelName", "texture_channel");

  if (!surface_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetSurfaceChannelName", "surface_channel");
    
  size_t hash = get_hash (material, texture_channel);
    
  if (!*surface_channel)
  {
    impl->channels.erase (hash);
  }
  else
  {
    impl->channels [hash] = surface_channel;
  }
}

/*
    Очистка
*/

void MaterialBinds::Clear ()
{
  impl->channels.clear ();
}
