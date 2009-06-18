#include "shared.h"

using namespace media;

namespace
{

/*
    Реализация менеджера построителей атласа изображений
*/

class AtlasBuilderManagerImpl
{
  public:
    typedef AtlasBuilderManager::PackHandler PackHandler;

    void RegisterPacker (const char* name, const PackHandler& handler)
    {
      static const char* METHOD_NAME = "media::AtlasBuilderManager::Register";
      
      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
        
      PackersMap::iterator iter = packers_map.find (name);

      if (iter != packers_map.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Packer with this name already registered");

      packers_map[name] = handler;
    }

    void UnregisterPacker (const char* name)
    {
      if (!name)
        return;
        
      packers_map.erase (name);
    }

    void UnregisterAllPackers ()
    {
      packers_map.clear ();
    }
    
    const PackHandler& GetPacker (const char* name)
    {
      static const char* METHOD_NAME = "media::AtlasBuilderManager::GetPacker";
      
      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
      static common::ComponentLoader loader ("media.image.atlas_builder.packers.*");

      PackersMap::iterator iter = packers_map.find (name);
      
      if (iter == packers_map.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "No packer with this name found");
      
      return iter->second;
    }
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, PackHandler> PackersMap;

  private:
    PackersMap packers_map;
};

typedef common::Singleton<AtlasBuilderManagerImpl> AtlasBuilderManagerSingleton;

}

/*
   Менеджер построителей атласа изображений
*/

/*
   Получение упаковщика атласа
*/

const AtlasBuilderManager::PackHandler& AtlasBuilderManager::GetPacker (const char* name)
{
  return AtlasBuilderManagerSingleton::Instance ()->GetPacker (name);
}

/*
   Регистрация упаковщиков атласа
*/

void AtlasBuilderManager::RegisterPacker (const char* name, const PackHandler& handler)
{
  AtlasBuilderManagerSingleton::Instance ()->RegisterPacker (name, handler);
}

void AtlasBuilderManager::UnregisterPacker (const char* name)
{
  AtlasBuilderManagerSingleton::Instance ()->UnregisterPacker (name);
}

void AtlasBuilderManager::UnregisterAllPackers ()
{
  AtlasBuilderManagerSingleton::Instance ()->UnregisterAllPackers ();
}
