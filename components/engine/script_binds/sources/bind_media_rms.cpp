#include "shared.h"

#include <media/rms/manager.h>
#include <media/rms/binding.h>

using namespace media::rms;
using namespace script;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* RMS_RESOURCE_MANAGER_LIBRARY = "Media.ResourceManager";
const char* RMS_BINDING_LIBRARY          = "Media.ResourceBinding";
const char* RMS_GROUP_LIBRARY            = "Media.ResourceGroup";
const char* BINDER_NAME                  = "Media";
const char* COMPONENT_NAME               = "script.binds.Media";

///Создание ресурсного менеджера
ResourceManager create_resource_manager ()
{
  return ResourceManager ();
}

//создание биндинга
Binding create_file_group_binding (ResourceManager& resource_manager, const char* resources)
{
  return resource_manager.CreateBinding (create_file_group (resources));
}

///Регистрация библиотеки работы с ресурсными менеджерами
void bind_resource_manager_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RMS_RESOURCE_MANAGER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_resource_manager));

    //регистрация операций

  lib.Register ("CreateFileGroupBinding", make_invoker (&create_file_group_binding));
  lib.Register ("CreateBinding",          make_invoker (&ResourceManager::CreateBinding));
  lib.Register ("FlushUnusedResources",   make_invoker (&ResourceManager::FlushUnusedResources));

  environment.RegisterType<ResourceManager> (RMS_RESOURCE_MANAGER_LIBRARY);
}

///Регистрация библиотеки работы со связываниями
void bind_binding_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RMS_BINDING_LIBRARY);

    //регистрация операций

  lib.Register ("Load",     make_invoker (&Binding::Load));
  lib.Register ("Unload",   make_invoker (&Binding::Unload));
  lib.Register ("Prefetch", make_invoker (&Binding::Prefetch));

  environment.RegisterType<Binding> (RMS_BINDING_LIBRARY);
}

///Создание группы ресурсов
Group create_group ()
{
  return Group ();
}

//Добавление группы ресурсов
void add_file_group_wrapper (Group& group, const char* file_group)
{
  add_file_group (file_group, group);
}

///Регистрация библиотеки работы с группами ресурсов
void bind_group_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RMS_GROUP_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (implicit_cast<Group (*) (const char*)> (&create_file_group)),
                                        make_invoker (implicit_cast<Group (*) ()> (&create_group))));

    //регистрация операций

  lib.Register ("get_Empty",    make_invoker (&Group::IsEmpty));
  lib.Register ("get_Size",     make_invoker (&Group::Size));
  lib.Register ("Item",         make_invoker (&Group::Item));
  lib.Register ("Clear",        make_invoker (&Group::Clear));
  lib.Register ("Add",          make_invoker (make_invoker (implicit_cast<void (Group::*) (const char*)> (&Group::Add)),
                                              make_invoker (implicit_cast<void (Group::*) (const Group&)> (&Group::Add))));
  lib.Register ("AddFileGroup", make_invoker (&add_file_group_wrapper));
  lib.Register ("Remove",       make_invoker (make_invoker (implicit_cast<void (Group::*) (const char*)> (&Group::Remove)),
                                              make_invoker (implicit_cast<void (Group::*) (const Group&)> (&Group::Remove))));

  environment.RegisterType<Group> (RMS_GROUP_LIBRARY);
}

/*
    Регистрация библиотеки менеджера ресурсов
*/

void bind_rms_library (Environment& environment)
{
    //регистрация библиотек

  bind_resource_manager_library (environment);
  bind_binding_library          (environment);
  bind_group_library            (environment);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_rms_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> RmsScriptBind (COMPONENT_NAME);

//?????? tests

}

}
