#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    ќписание реализации присоединенного материала
*/

typedef stl::hash_map<size_t, stl::string>                     ChannelMap;
typedef stl::hash_map<stl::hash_key<const char*>, stl::string> MaterialMap;

struct MaterialBinds::Impl: public xtl::reference_counter
{
  ChannelMap  channels;  //карта каналов    
  MaterialMap materials; //карта материалов  
};

/*
     онструкторы / деструктор / присваивание
*/

MaterialBinds::MaterialBinds ()
  : impl (new Impl, false)
  {}
  
MaterialBinds::MaterialBinds (Impl* in_impl)
  : impl (in_impl, false)
  {}

MaterialBinds::MaterialBinds (const MaterialBinds& binds)
  : impl (binds.impl)
  {}

MaterialBinds::~MaterialBinds ()
{
}

MaterialBinds& MaterialBinds::operator = (const MaterialBinds& binds)
{
  impl = binds.impl;
  
  return *this;
}

/*
    —оздание копии
*/

MaterialBinds MaterialBinds::Clone () const
{
  return MaterialBinds (new Impl (*impl));
}

/*
    —в€зывание материалов
*/

const char* MaterialBinds::FindMaterial (const char* symbol) const //nothrow
{
  if (!symbol)
    return 0;
    
  MaterialMap::const_iterator iter = impl->materials.find (symbol);
  
  return iter != impl->materials.end () ? iter->second.c_str () : 0;
}

const char* MaterialBinds::FindMaterial (const Surface& surface) const //nothrow
{
    //здесь должна быть проверка совместимости с поверхностью, но пока что это невозможно из-за отсутстви€ метода Surface::Owner

  return FindMaterial (surface.Material ());
}

void MaterialBinds::SetMaterial (const char* symbol, const char* material)
{
  if (!symbol)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::SetMaterial", "symbol");    

  if (!material)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::SetMaterial", "material");

  impl->materials [symbol] = material;
}

void MaterialBinds::RemoveMaterial (const char* symbol)
{
  if (!symbol)
    return;
    
  impl->materials.erase (symbol);
}

/*
    »м€ канала текстурированных вершин поверхности
*/

namespace
{

size_t get_hash (const char* material, const char* channel)
{
  return strhash (channel, strhash ("#", strhash (material)));
}

}

const char* MaterialBinds::TexcoordChannelName (const char* material, const char* texture_channel) const //nothrow
{
  if (!material || !texture_channel)
    return "";

  ChannelMap::const_iterator iter = impl->channels.find (get_hash (material, texture_channel));

  return iter != impl->channels.end () ? iter->second.c_str () : "";
}

void MaterialBinds::SetTexcoordChannelName (const char* material, const char* texture_channel, const char* surface_channel)
{
  if (!material)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::SetTexcoordChannelName", "material");

  if (!texture_channel)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::SetTexcoordChannelName", "texture_channel");

  if (!surface_channel)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::SetTexcoordChannelName", "surface_channel");
    
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

void MaterialBinds::RemoveTexcoordChannelName (const char* material, const char* texture_channel)
{
  if (!material)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::RemoveTexcoordChannelName", "material");

  if (!texture_channel)
    throw xtl::make_null_argument_exception ("media::collada::MaterialBinds::RemoveSurfaceChannelName", "texture_channel");
    
  size_t hash = get_hash (material, texture_channel);
  
  impl->channels.erase (hash);
}

int MaterialBinds::FindTexcoordChannel (const Surface& surface, const Texture& texture) const //nothrow
{
  const char* surface_channel_name = TexcoordChannelName (surface.Material (), texture.TexcoordChannel ());

  if (!*surface_channel_name)
    return -1;
    
  return surface.TexVertexChannels ().Find (surface_channel_name);
}

/*
    ќчистка
*/

void MaterialBinds::Clear ()
{
  impl->channels.clear ();
  impl->materials.clear ();
}
