#include "shared.h"

#include <common/strlib.h>

#include <media/rms/manager.h>

#include <sg/listener.h>

#include <render/scene_render.h>

#include <input/cursor.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME             = "script.binds.Engine";
const char* BINDER_NAME                = "Engine";
const char* ATTACHMENT_REGISTRY_PREFIX = "Engine";
const char* SUBSYSTEM_MANAGER_LIBRARY  = "Engine.SubsystemManager";

/*
    Регистрация библиотек
*/

template <class T> void register_attachment (const char* name, T& value)
{
  AttachmentRegistry::Register (name, value);
}

template <> void register_attachment<scene_graph::Listener> (const char* name, scene_graph::Listener& listener)
{
  scene_graph::Listener::Pointer listener_pointer (&listener);

  AttachmentRegistry::Register (name, listener_pointer);
}

template <class T> void unregister_attachment (const char* name)
{
  AttachmentRegistry::Unregister<T> (name);
}

template <class T> bool is_attachment_present (const char* name)
{
  return AttachmentRegistry::Find<T> (name) != 0;
}

template <class T> struct selector_result_type
{
  typedef T type;

  static T get (T& value) { return value; }
};

template <> struct selector_result_type<scene_graph::Listener>
{
  typedef scene_graph::Listener::Pointer type;

  static type get (scene_graph::Listener& value) { return &value; }
};

template <class T> typename selector_result_type<T>::type get_attachment (const char* name)
{
  return selector_result_type<T>::get (AttachmentRegistry::Get<T> (name));
}

template <class T> size_t find_attachment (IStack& stack)
{
  const char* name  = stack.GetString (1);
  T*          value = AttachmentRegistry::Find<T> (name);

  if (value)
  {
    script::detail::push_argument (stack, xtl::make_const_ref (selector_result_type<T>::get (*value)));
  }
  else
  {
    stack.Push ((void*)0);
  }

  return 1;
}

template <class T> void bind_attachment_methods (Environment& environment, const char* suffix)
{
  InvokerRegistry& lib = environment.Library (common::format ("%s.%s", ATTACHMENT_REGISTRY_PREFIX, suffix).c_str ());

  lib.Register ("Register",   make_invoker (&register_attachment<T>));
  lib.Register ("Unregister", make_invoker (&unregister_attachment<T>));
  lib.Register ("IsPresent",  make_invoker (&is_attachment_present<T>));
  lib.Register ("Get",        make_invoker (&get_attachment<T>));
  lib.Register ("Find",       &find_attachment<T>);
}

void bind_attachment_registry_library (Environment& environment)
{
  bind_attachment_methods<render::Screen>        (environment, "Screens");
  bind_attachment_methods<scene_graph::Listener> (environment, "Listeners");

  typedef xtl::function<void (const char*)> InputHandler;

  bind_attachment_methods<InputHandler>  (environment, "InputEventHandlers");
  bind_attachment_methods<input::Cursor> (environment, "Cursors");
}

void bind_subsystem_manager_library (Environment& environment)
{
  InvokerRegistry& lib = environment.Library (SUBSYSTEM_MANAGER_LIBRARY);

  lib.Register ("Start",   make_invoker (xtl::implicit_cast<void (SubsystemManager::*) (const char*, const char*)> (&SubsystemManager::Start)));
  lib.Register ("Restart", make_invoker (xtl::implicit_cast<void (SubsystemManager::*) (const char*, const char*)> (&SubsystemManager::Restart)));
  lib.Register ("Remove",  make_invoker (xtl::implicit_cast<void (SubsystemManager::*) (const char*)>              (&SubsystemManager::Remove)));

  environment.RegisterType<SubsystemManager> (SUBSYSTEM_MANAGER_LIBRARY);
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
      bind_attachment_registry_library (environment);
      bind_subsystem_manager_library   (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> EngineScriptBind (COMPONENT_NAME);

}

}
