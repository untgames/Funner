#include <stl/hash_set>
#include <stl/string>
#include <stl/vector>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/window.h>

#include <client/window_manager.h>

using namespace common;
using namespace client;

namespace
{

const char* WINDOWS_SUBSYSTEM_NAME   = "WindowsSubsytem";
const char* LOG_NAME                 = "client.WindowManager";
const char* REGISTRY_COMPONENTS_MASK = "client.subsystems.*";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсистема движка               
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsSubsytem : public IEngineSubsystem, public xtl::reference_counter
{
  private:
    typedef xtl::shared_ptr<syslib::Window> WindowPtr;

  public:
    const char* Name ()
    {
      return WINDOWS_SUBSYSTEM_NAME;
    }

    void AddWindow (WindowPtr new_window)
    {
      windows.push_back (new_window);
    }

    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:  
    typedef stl::vector<WindowPtr> WindowsArray;

  private:
    WindowsArray windows;
};

stl::string get_string (VarRegistry& registry, const char* name)
{
  return registry.GetValue (name).cast<stl::string> ();
}

size_t get_size_t (VarRegistry& registry, const char* name)
{
  return atoi (get_string (registry, name).c_str ());
}

syslib::WindowStyle get_window_style (VarRegistry& registry, const char* name)
{
  stl::string window_style (get_string (registry, name));

  if (!xtl::xstricmp ("PopUp", window_style.c_str ()))
    return syslib::WindowStyle_PopUp;
  if (!xtl::xstricmp ("Overlapped", window_style.c_str ()))
    return syslib::WindowStyle_Overlapped;

  throw xtl::format_operation_exception ("", "Unknown '%s' value", name);
}

stl::string get (VarRegistry& registry, const char* name, const char* default_value)
{
  if (registry.HasVariable (name))
    return get_string (registry, name);

  return stl::string (default_value);
}

struct WindowManagerImpl
{
  private:
    typedef WindowManager::StartupHandler StartupHandler;

  public:
    WindowManagerImpl () : need_sort (false), log (LOG_NAME) {}

    void RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler, int order)
    {
      static const char* METHOD_NAME = "client::WindowManager::RegisterStartupHandler";

      if (!node_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "node_name");

      StartupHandlers::iterator iter = FindStartupHandler (node_name);

      if (iter != startup_handlers.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't register startup handler with node name '%s', already registered", node_name);

      startup_handlers.push_back (StartupHandlerEntryPtr (new StartupHandlerEntry (node_name, startup_handler, order)));

      need_sort = true;
    }
    
    void UnregisterStartupHandler (const char* node_name)
    {
      if (!node_name)
        throw xtl::make_null_argument_exception ("client::WindowManager::UnregisterStartupHandler", "node_name");

      StartupHandlers::iterator iter = FindStartupHandler (node_name);

      if (iter != startup_handlers.end ())
        startup_handlers.erase (iter);
    }

    void UnregisterAllStartupHandlers ()
    {
      startup_handlers.clear ();
    }

    void InitWindow (syslib::Window& window, VarRegistry& var_registry, Engine& engine)
    {
      static common::ComponentLoader loader (REGISTRY_COMPONENTS_MASK);

      if (need_sort)
      {
        stl::sort (startup_handlers.begin (), startup_handlers.end (), xtl::bind (&WindowManagerImpl::StartupHandlerEntryLessPredicate, this, _1, _2));
        need_sort = false;
      }

      stl::hash_set<stl::hash_key<const char*> > registry_branches;

      for (VarRegistry::Iterator configuration_branch_iter = var_registry.CreateIterator (); configuration_branch_iter; ++configuration_branch_iter)
        registry_branches.insert (configuration_branch_iter->Name ());

      VarRegistry configuration_registry;

      for (StartupHandlers::iterator iter = startup_handlers.begin (), end = startup_handlers.end (); iter != end; ++iter)
      {
        if (!registry_branches.count ((*iter)->node_name.c_str ()))
        {
          log.Printf ("Startup handler with node name '%s' was not called, no such branch in registry '%s'", (*iter)->node_name.c_str (), var_registry.BranchName ());
          continue;
        }

        if (*var_registry.BranchName ())
          configuration_registry.Open (common::format ("%s.%s", var_registry.BranchName (), (*iter)->node_name.c_str ()).c_str ());
        else
          configuration_registry.Open ((*iter)->node_name.c_str ());

        (*iter)->handler (window, configuration_registry, engine);
      }
    }

    void InitWindows (VarRegistry& var_registry, Engine& engine)
    {
      static common::ComponentLoader loader (REGISTRY_COMPONENTS_MASK);

      xtl::com_ptr<WindowsSubsytem> windows_subsystem (new WindowsSubsytem (), false); 

      VarRegistry window_registry;

      for (VarRegistry::Iterator window_iter=var_registry.CreateIterator (); window_iter; ++window_iter)
      {
        if (*var_registry.BranchName ())
          window_registry.Open (common::format ("%s.%s", var_registry.BranchName (), window_iter->Name ()).c_str ());
        else
          window_registry.Open (window_iter->Name ());

        if (!TestRegistryVariable (window_registry, "Style")) continue;
        if (!TestRegistryVariable (window_registry, "Width")) continue;
        if (!TestRegistryVariable (window_registry, "Height")) continue;

        try
        {
          xtl::shared_ptr<syslib::Window> new_window (new syslib::Window (get_window_style (window_registry, "Style"), 
                                                                          get_size_t       (window_registry, "Width"),
                                                                          get_size_t       (window_registry, "Height")));

          new_window->SetTitle (get (window_registry, "Title", "").c_str ());
          
          syslib::Point window_position = new_window->Position ();

          if (window_registry.HasVariable ("XPosition"))
            window_position.x = atoi (get_string (window_registry, "XPosition").c_str ());
          if (window_registry.HasVariable ("YPosition"))
            window_position.y = atoi (get_string (window_registry, "YPosition").c_str ());
          
          new_window->SetPosition (window_position);
        
          if (window_registry.HasVariable ("Show"))
            if (get_size_t (window_registry, "Show"))
              new_window->Show ();

          InitWindow (*new_window, window_registry, engine);

          windows_subsystem->AddWindow (new_window);
        }
        catch (xtl::exception& e)
        {
          log.Printf ("Can't add window from registry '%s', exception '%s'", window_registry.BranchName (), e.what ());
        }
        catch (...)
        {
          log.Printf ("Can't add window from registry '%s', unknown exception", window_registry.BranchName ());
        }
      }

      engine.AddSubsystem (windows_subsystem);
    }

  private:
    struct StartupHandlerEntry
    {
      stl::string    node_name;
      StartupHandler handler;
      int            order;

      StartupHandlerEntry (const char* in_node_name, const StartupHandler& in_handler, int in_order)
        : node_name (in_node_name), handler (in_handler), order (in_order)
        {}
    };

    typedef xtl::shared_ptr<StartupHandlerEntry> StartupHandlerEntryPtr;
    typedef stl::vector<StartupHandlerEntryPtr>  StartupHandlers;

  private:
    bool StartupHandlerEntryLessPredicate (const StartupHandlerEntryPtr& left, const StartupHandlerEntryPtr& right)
    {
      return left->order < right->order;
    }

    StartupHandlers::iterator FindStartupHandler (const char* node_name)
    {
      for (StartupHandlers::iterator iter = startup_handlers.begin (), end = startup_handlers.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->node_name.c_str (), node_name))
          return iter;

      return startup_handlers.end ();
    }

    bool TestRegistryVariable (VarRegistry& registry, const char* var_name)
    {
      if (registry.HasVariable (var_name))
        return true;

      log.Printf ("Can't create window from registry branch '%s', no '%s' variable", registry.BranchName (), var_name);

      return false;
    }

  private:
    StartupHandlers startup_handlers;
    bool            need_sort;
    common::Log     log;
};

/*
   Синглтон менеджера запуска
*/

typedef common::Singleton<WindowManagerImpl> WindowManagerSingleton;

}

/*
   Менеджер окон
*/

/*
   Работа с обработчиками
*/

void WindowManager::RegisterStartupHandler (const char* name, const StartupHandler& startup_handler, int order)
{
  WindowManagerSingleton::Instance ().RegisterStartupHandler (name, startup_handler, order);
}

void WindowManager::UnregisterStartupHandler (const char* name)
{
  WindowManagerSingleton::Instance ().UnregisterStartupHandler (name);
}

void WindowManager::UnregisterAllStartupHandlers ()
{
  WindowManagerSingleton::Instance ().UnregisterAllStartupHandlers ();
}

/*
   Инициализация окон
*/

void WindowManager::InitWindow (syslib::Window& window, VarRegistry& var_registry, Engine& engine)
{
  WindowManagerSingleton::Instance ().InitWindow (window, var_registry, engine);
}

void WindowManager::InitWindows (VarRegistry& var_registry, Engine& engine)
{
  WindowManagerSingleton::Instance ().InitWindows (var_registry, engine);
}
