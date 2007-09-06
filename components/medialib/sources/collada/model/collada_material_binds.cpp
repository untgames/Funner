#include <media/collada/scene.h>
#include <stl/hash_map>
#include <stl/string>

using namespace medialib::collada;
using namespace common;

/*
    Описание реализации присоединенного материала
*/

typedef stl::hash_map<size_t, stl::string>                   ChannelMap;
typedef stl::hash_map<stl::hash_key<const char*>, Material*> MaterialMap;

struct MaterialBinds::Impl
{
  Entity&     owner;     //владелец
  ChannelMap  channels;  //карта каналов    
  MaterialMap materials; //карта материалов
  
  Impl (Entity& in_owner) : owner (in_owner) {}
};

/*
    Конструктор / деструктор
*/

MaterialBinds::MaterialBinds (Entity& owner)
  : impl (new Impl (owner))
  {}

MaterialBinds::~MaterialBinds ()
{
  delete impl;
}

/*
    Связывание материалов
*/

const Material* MaterialBinds::FindMaterial (const char* symbol) const //nothrow
{
  if (!symbol)
    return 0;
    
  MaterialMap::const_iterator iter = impl->materials.find (symbol);
  
  return iter != impl->materials.end () ? iter->second : 0;
}

Material* MaterialBinds::FindMaterial (const char* symbol)
{
  return const_cast<collada::Material*> (const_cast<const MaterialBinds&> (*this).FindMaterial (symbol));
}

const Material* MaterialBinds::FindMaterial (const Surface& surface) const //nothrow
{
    //здесь должна быть проверка совместимости с поверхностью, но пока что это невозможно из-за отсутствия метода Surface::Owner

  return FindMaterial (surface.MaterialName ());
}

Material* MaterialBinds::FindMaterial (const Surface& surface)
{
  return const_cast<Material*> (const_cast<const MaterialBinds&> (*this).FindMaterial (surface));
}

void MaterialBinds::SetMaterial (const char* symbol, Material& material)
{
  if (!symbol)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetMaterial", "symbol");
    
  if (material.Owner () != impl->owner.Owner ())
    raise_incompatible ("medialib::collada::MaterialBinds::SetMaterial", material, impl->owner);
    
  impl->materials [symbol] = &material;
}

void MaterialBinds::RemoveMaterial (const char* symbol)
{
  if (!symbol)
    RaiseNullArgument ("medialib::collada::MaterialBinds::RemoveMaterial", "symbol");
    
  impl->materials.erase (symbol);
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
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetTexcoordChannelName", "material");

  if (!texture_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetTexcoordChannelName", "texture_channel");

  if (!surface_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::SetTexcoordChannelName", "surface_channel");
    
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
    RaiseNullArgument ("medialib::collada::MaterialBinds::RemoveTexcoordChannelName", "material");

  if (!texture_channel)
    RaiseNullArgument ("medialib::collada::MaterialBinds::RemoveSurfaceChannelName", "texture_channel");
    
  size_t hash = get_hash (material, texture_channel);
  
  impl->channels.erase (hash);
}

int MaterialBinds::FindTexcoordChannel (const Surface& surface, const Texture& texture) const //nothrow
{
    //здесь должна быть проверка совместимости с поверхностью, но пока что это невозможно из-за отсутствия метода Surface::Owner

  const char* surface_channel_name = TexcoordChannelName (surface.MaterialName (), texture.TexcoordChannel ());

  if (!*surface_channel_name)
    return -1;
    
  return surface.TexVertexChannels ().Find (surface_channel_name);
}

/*
    Очистка
*/

void MaterialBinds::Clear ()
{
  impl->channels.clear ();
  impl->materials.clear ();
}
