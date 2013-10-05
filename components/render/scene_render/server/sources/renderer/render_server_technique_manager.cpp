#include "shared.h"

using namespace render;
using namespace render::scene::server;

namespace
{

/*
    Менеджер техник
*/

class TechniqueManagerImpl
{
  public:
    typedef TechniqueManager::TechniqueCreator TechniqueCreator;

///Регистрация техник
    void RegisterTechnique (const char* technique, const TechniqueCreator& creator)
    {
      static const char* METHOD_NAME = "render::scene::server::TechniqueManagerImpl::RegisterTechnique";

      if (!technique)
        throw xtl::make_null_argument_exception (METHOD_NAME, "technique");

      TechniqueMap::iterator iter = techniques.find (technique);

      if (iter != techniques.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Technique '%s' has been already registered", technique);

      techniques.insert_pair (technique, creator);
    }

    void UnregisterTechnique (const char* technique)
    {
      if (!technique)
        return;

      techniques.erase (technique);
    }

    void UnregisterAllTechniques ()
    {
      techniques.clear ();
    }

///Создание техники
    TechniquePtr CreateTechnique (const char* technique, RenderManager& manager, const common::ParseNode& configuration)
    {
      try
      {
        if (!technique)
          throw xtl::make_null_argument_exception ("", "technique");

        TechniqueMap::iterator iter = techniques.find (technique);

        if (iter == techniques.end ())
          throw xtl::format_operation_exception ("", "Technique '%s' has not been registered", technique);

        return TechniquePtr (iter->second (manager, configuration), false);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::TechniqueManagerImpl::CreateTechnique(technique='%s')", technique ? technique : "<nil>");
        throw;
      }
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, TechniqueCreator> TechniqueMap;

  private:
    TechniqueMap techniques; //техники
};

typedef common::Singleton<TechniqueManagerImpl> TechniqueManagerSingleton;

}

/*
    Делегирование вызовов
*/

void TechniqueManager::RegisterTechnique (const char* technique, const TechniqueCreator& creator)
{
  TechniqueManagerSingleton::Instance ()->RegisterTechnique (technique, creator);
}

void TechniqueManager::UnregisterTechnique (const char* technique)
{
  TechniqueManagerSingleton::Instance ()->UnregisterTechnique (technique);
}

void TechniqueManager::UnregisterAllTechniques ()
{
  TechniqueManagerSingleton::Instance ()->UnregisterAllTechniques ();
}

TechniquePtr TechniqueManager::CreateTechnique (const char* technique, RenderManager& manager, const common::ParseNode& configuration)
{
  return TechniqueManagerSingleton::Instance ()->CreateTechnique (technique, manager, configuration);
}
