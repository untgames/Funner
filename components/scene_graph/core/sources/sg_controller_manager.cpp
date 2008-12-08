#include "shared.h"

using namespace scene_graph;

namespace
{

/*
    Константы
*/

const char* DEFAULT_COMPONENTS_MASK = "scene_graph.controllers.*";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер контроллеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class ControllerManagerImpl
{
  public:
    typedef ControllerManager::ControllerCreator ControllerCreator;
  
///Регистрация функтора создания контроллера
    void Register (const char* name, const ControllerCreator& creator)
    {
      static const char* METHOD_NAME = "scene_graph::ControllerManagerImpl::Register";
      
        //проверка корректности аргументов

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        //проверка повторной регистрации
        
      CreatorMap::iterator iter = creators.find (name);
      
      if (iter != creators.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Controller already registered");

        //регистрация нового контроллера

      creators.insert_pair (name, creator);
    }
    
///Отмена регистрации контроллера
    void Unregister (const char* name)
    {
      if (!name)
        return;
        
      creators.erase (name);
    }
    
///Отмена регистрации всех контроллеров
    void UnregisterAll ()
    {
      creators.clear ();
    }

///Зарегистрирован ли контроллер
    bool IsPresent (const char* name)
    {
      if (!name)
        return false;
        
      return creators.find (name) != creators.end ();
    }
    
///Создание контроллера
    IController* CreateController (Node& node, const char* name, const xtl::any_reference& param)
    {
      try
      {                
          //проверка корректности аргументов

        if (!name)
          throw xtl::make_null_argument_exception ("", "name");
          
          //загрузка компонентов по умолчанию
          
        static common::ComponentLoader loader (DEFAULT_COMPONENTS_MASK);

          //поиск функтора создания контроллера

        CreatorMap::iterator iter = creators.find (name);

        if (iter == creators.end ())
          throw xtl::make_argument_exception ("", "name", name, "Controller has not registered");

           //создание контроллера

        return iter->second (node, param);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::ControllerManagerImpl::CreateController");
        throw;
      }
    }    

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ControllerCreator> CreatorMap;

  private:
    CreatorMap creators; //карта дескрипторов создания контроллеров
};

typedef common::Singleton<ControllerManagerImpl> ControllerManagerSingleton;

}

/*
    Обёртки над вызовами к ControllerManagerImpl
*/

void ControllerManager::Register (const char* name, const ControllerCreator& creator)
{
  ControllerManagerSingleton::Instance ().Register (name, creator);
}

void ControllerManager::Unregister (const char* name)
{
  ControllerManagerSingleton::Instance ().Unregister (name);
}

void ControllerManager::UnregisterAll ()
{
  ControllerManagerSingleton::Instance ().UnregisterAll ();
}

bool ControllerManager::IsPresent (const char* name)
{
  return ControllerManagerSingleton::Instance ().IsPresent (name);
}

namespace scene_graph
{

IController* create_controller (Node& node, const char* name, const xtl::any_reference& param)
{
  return ControllerManagerSingleton::Instance ().CreateController (node, name, param);
}

}
