#include "shared.h"

#using <funner.tools.ui.windows_forms.PluginApi.dll>

using namespace System::Reflection;
using namespace System::Collections;
using namespace tools::ui::windows_forms;

namespace
{

const char* LOG_NAME        = "tools.ui.windows_forms.PluginManager";
const char* REGISTRY_BRANCH = "ApplicationServer";

typedef msclr::gcroot<Assembly^> AssemblyPtr;
typedef msclr::gcroot<IPlugin^>  IPluginPtr;

struct PluginEntry
{
  IPluginPtr  plugin;
  stl::string name;

  PluginEntry (IPlugin^ in_plugin, const char* in_name) : plugin (in_plugin), name (in_name) {}
};

class VarRegistryEventHandler
{
  public:
    VarRegistryEventHandler (System::String^ wc_mask, IPropertyListener^ in_property_listener, common::VarRegistry* var_registry)
      : property_listener (in_property_listener)
    {
      AutoString wc_mask_string (wc_mask);

      create_var_connection = var_registry->RegisterEventHandler (wc_mask_string.Data (), common::VarRegistryEvent_OnCreateVar, 
                                                                  xtl::bind (&VarRegistryEventHandler::OnCreateVar, this, _1));
      change_var_connection = var_registry->RegisterEventHandler (wc_mask_string.Data (), common::VarRegistryEvent_OnChangeVar, 
                                                                  xtl::bind (&VarRegistryEventHandler::OnChangeVar, this, _1));
      delete_var_connection = var_registry->RegisterEventHandler (wc_mask_string.Data (), common::VarRegistryEvent_OnDeleteVar, 
                                                                  xtl::bind (&VarRegistryEventHandler::OnDeleteVar, this, _1));
    }

  private:
    void OnCreateVar (const char* var_name)
    {
      property_listener->OnAddProperty (gcnew System::String (var_name));
    }

    void OnChangeVar (const char* var_name)
    {
      property_listener->OnChangeProperty (gcnew System::String (var_name));
    }

    void OnDeleteVar (const char* var_name)
    {
      property_listener->OnRemoveProperty (gcnew System::String (var_name));
    }

  private:
    msclr::gcroot<IPropertyListener^> property_listener;
    xtl::auto_connection              create_var_connection;
    xtl::auto_connection              change_var_connection;
    xtl::auto_connection              delete_var_connection;
};

}

/*
    Описание реализации интерфейса приложения
*/

namespace tools
{

namespace ui
{

namespace windows_forms
{

public ref class ApplicationServerImpl: public tools::ui::windows_forms::IApplicationServer
{
  public:
///Конструктор/деструктор
    ApplicationServerImpl (WindowSystem* in_window_system)
    {
      window_system = in_window_system;
      plugins = new PluginMap;

      try
      {
        var_registry_container = new StringRegistryContainer;

        try
        {
          var_registry_container->Mount (REGISTRY_BRANCH);

          var_registry = new common::VarRegistry (REGISTRY_BRANCH);
        }
        catch (...)
        {
          delete var_registry_container;
          throw;
        }
      }
      catch (...)
      {
        delete plugins;
        throw;
      }
    }

    ~ApplicationServerImpl ()
    {
      delete var_registry;
      delete var_registry_container;
      delete plugins;
    }

///Добавление плагина
    virtual void AddPlugin (System::String^ const plugin_name, IPlugin^ plugin)
    {
      AutoString plugin_name_string (plugin_name);

      PluginMap::iterator iter = plugins->find (plugin_name_string.Data ());

      if (iter != plugins->end ())
        throw xtl::format_operation_exception ("tools::ui::windows_forms::ApplicationServerImpl::AddPlugin", 
                                               "Can't add plugin with name %s, plugin with this name already added", plugin_name_string.Data ());

      plugins->insert_pair (plugin_name_string.Data (), PluginEntry (plugin, plugin_name_string.Data ()));
    }

///Удаление плагинов
    void RemovePlugins (const char* wc_mask)
    {
      for (PluginMap::iterator iter = plugins->begin (), end = plugins->end (); iter != end;)
      {
        if (common::wcmatch (iter->second.name.c_str (), wc_mask))
        {
          PluginMap::iterator next = iter;

          ++next;

          plugins->erase (iter);

          iter = next;
        }
        else ++iter;
      }
    }

///Выполнение команды на стороне приложения
    virtual void ExecuteCommand (System::String^ const command)
    {
      AutoString command_string (command);

      window_system->Execute (command_string.Data ());
    }

///Получение / установка значения свойства
    virtual void SetProperty (System::String^ const name, System::String^ const value)
    {
      AutoString var_name (name), var_value (value);

      var_registry->SetValue (var_name.Data (), xtl::any (stl::string (var_value.Data ()), true));
    }

    virtual void GetProperty (System::String^ const name, System::String^% value)
    {
      AutoString var_name (name);

      value = gcnew String (to_string (var_registry->GetValue (var_name.Data ())).c_str ());
    }

///Проверка наличия свойства / проверка режима "только чтения" свойства
    virtual System::Boolean IsPropertyPresent (System::String^ const name)
    {
      AutoString var_name (name);

      return var_registry->HasVariable (var_name.Data ());
    }

///Регистрация слушателей событий свойств
    virtual void RegisterPropertyListener (System::String^ const name_wc_mask, IPropertyListener^ listener)
    {
      for (int i = 0; i < property_listeners.Count; i++)
      {
        PropertyListenerEntry^ current_item = safe_cast<PropertyListenerEntry^>(property_listeners[i]);

        if ((current_item->name_wc_mask == name_wc_mask) && (current_item->listener == listener))
        {
          AutoString name_wc_mask_string (name_wc_mask);

          throw xtl::format_operation_exception ("tools::ui::windows_forms::ApplicationServerImpl::RegisterPropertyListener",
                                                 "Can't register listener, this listener for wc_mask %s already registered", name_wc_mask_string.Data ());
        }
      }

      PropertyListenerEntry^ new_listener = gcnew PropertyListenerEntry (name_wc_mask, listener, var_registry);

      property_listeners.Add (new_listener);
    }

    virtual void UnregisterPropertyListener (System::String^ const name_wc_mask, IPropertyListener^ listener)
    {
      for (int i = 0; i < property_listeners.Count; i++)
      {
        PropertyListenerEntry^ current_item = safe_cast<PropertyListenerEntry^>(property_listeners[i]);

        if ((current_item->name_wc_mask == name_wc_mask) && (current_item->listener == listener))
        {
          property_listeners.RemoveAt (i);
          return;
        }
      }
    }

    virtual void UnregisterAllPropertyListeners (System::String^ const name_wc_mask)
    {
      for (int i = 0; i < property_listeners.Count;)
      {
        if (safe_cast<PropertyListenerEntry^>(property_listeners[i])->name_wc_mask == name_wc_mask)
          property_listeners.RemoveAt (i);
        else
          i++;
      }
    }

    virtual void UnregisterAllPropertyListeners ()
    {
      property_listeners.Clear ();
    }

///Регистрация слушателей сообщений вывода
    virtual void RegisterOutputListener (IOutputListener^ listener)
    {
      if (output_listeners.IndexOf (listener) != -1)
        throw xtl::format_operation_exception ("tools::ui::windows_forms::ApplicationServerImpl::RegisterOutputListener",
                                               "Can't register listener, this listener already registered");

      output_listeners.Add (listener);
    }

    virtual void UnregisterOutputListener (IOutputListener^ listener)
    {
      output_listeners.Remove (listener);
    }

    virtual void UnregisterAllOutputListeners ()
    {
      output_listeners.Clear ();
    }

///Обработка сообщений консоли
    void ProcessConsoleMessage (const char* message)
    {
      System::String ^message_string = gcnew System::String (message);

      for (int i = 0; i < output_listeners.Count; i++)
        safe_cast<IOutputListener^>(output_listeners[i])->OnMessage (message_string);
    }

///Создание формы
    ChildForm::Pointer CreateForm (WindowSystem* window_system, const char* plugin, const char* init_string, FormDockState dock_state)
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ApplicationServerImpl::CreateForm";

      PluginMap::iterator plugin_iter = plugins->find (plugin);

      if (plugin_iter == plugins->end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't create form with plugin %s, plugin not registered", plugin);

      System::String^ init_string_wrap = gcnew System::String (init_string);

      System::Windows::Forms::Control^ new_form = plugin_iter->second.plugin->CreateControl (init_string_wrap);

      return ChildForm::Create (*window_system, new_form, dock_state);
    }

///Загрузка конфигурации
    void LoadConfiguration (System::Xml::XmlNode^ plugin_configuration)
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ApplicationServerImpl::LoadConfiguration";

      try
      {
        System::Xml::XmlNode ^plugin_name = plugin_configuration->Attributes["Name"];

        if (!plugin_name)
        {
          AutoString node_path (get_node_path (plugin_configuration));

          throw xtl::format_operation_exception ("", "Can't load configuration from node %s, there is no 'Name' attribute in this node", 
                                                 node_path.Data ());
        }

        AutoString plugin_name_string (plugin_name->InnerText);

        PluginMap::iterator plugin_iter = plugins->find (plugin_name_string.Data ());

        if (plugin_iter == plugins->end ())
          return;

        plugin_iter->second.plugin->LoadConfiguration (plugin_configuration);
      }
      catch (System::Exception^ exception)
      {
        throw DotNetException (METHOD_NAME, exception);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
        throw;
      }
    }

  private:
    ref struct PropertyListenerEntry
    {
      System::String^          name_wc_mask;
      IPropertyListener^       listener;
      VarRegistryEventHandler* event_handler;

      PropertyListenerEntry (System::String^ in_name_wc_mask, IPropertyListener^ in_listener, common::VarRegistry* var_registry)
        : name_wc_mask (in_name_wc_mask), listener (in_listener), event_handler (new VarRegistryEventHandler (in_name_wc_mask, in_listener, var_registry))
        {}

      ~PropertyListenerEntry ()
      {
        delete event_handler;
      }

    };

    typedef stl::hash_map<stl::hash_key<const char*>, PluginEntry> PluginMap;
    typedef common::VarRegistryContainer<stl::string>              StringRegistryContainer;

  private:
    WindowSystem*            window_system;          //оконная система
    PluginMap*               plugins;                //добавленные плагины
    StringRegistryContainer* var_registry_container; //контейнер реестра переменных
    common::VarRegistry*     var_registry;           //реестр переменных

    ArrayList                property_listeners;     //слушатели событий свойств
    ArrayList                output_listeners;       //слушатели сообщений вывода
};

}

}

} 

/*
   Менеджер плагинов
*/

struct PluginManager::Impl
{   
  WindowSystem*        window_system;      //оконная система
  common::Log          log;                //лог
  ApplicationServerPtr application_server; //интерфейс приложения
  xtl::auto_connection console_connection; //соединение обработчика консоли 

  Impl (WindowSystem* in_window_system)
    : window_system (in_window_system), log (LOG_NAME)
  {
    console_connection = common::Console::RegisterEventHandler (xtl::bind (&PluginManager::Impl::ConsoleHandler, this, _1));
  }

  void ConsoleHandler (const char* message)
  {
    application_server->ProcessConsoleMessage (message);
  }
};

/*
   Конструктор/деструктор
*/

PluginManager::PluginManager (WindowSystem& in_window_system)
  : impl (new Impl (&in_window_system))
{
  try
  {
    impl->application_server = gcnew ApplicationServerImpl (&in_window_system);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::PluginManager::PluginManager", exception);
  }
}

PluginManager::~PluginManager ()
{
}

/*
   Загрузка/выгрузка плагинов
*/

void PluginManager::LoadPlugins (const char* wc_mask)
{
  if (!wc_mask)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::PluginManager::LoadPlugins", "wc_mask");

  common::FileList plugins_files = common::FileSystem::Search (wc_mask, common::FileSearch_Files);

  for (common::FileListIterator iter = plugins_files.GetIterator (); iter; ++iter)
  {
    try
    {
      System::String ^file_path = gcnew System::String (iter->name);

      Assembly^ assembly = Assembly::LoadFile (file_path);

      Type ^plugin_class = assembly->GetType ("tools.ui.windows_forms.Plugin");

      if (!plugin_class)
        throw xtl::format_operation_exception ("", "Class 'tools.ui.windows_forms.Plugin' not found");

      MethodInfo^ create_plugin_func = plugin_class->GetMethod ("InitPlugin", gcnew array<Type^> {IApplicationServer::typeid});

      if (!create_plugin_func)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' not found in class 'tools.ui.windows_forms.Plugin'");

      if (!create_plugin_func->IsStatic || !create_plugin_func->IsPublic)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' in class 'tools.ui.windows_forms.Plugin' must be static public");

      array<Object^>^ params = {impl->application_server};

      create_plugin_func->Invoke (nullptr, params);
    }
    catch (xtl::exception& exception)
    {
      impl->log.Printf ("Can't load plugin '%s', exception %s", iter->name, exception.what ());
    }
    catch (System::Exception^ exception)
    {
      impl->log.Printf ("Can't load plugin '%s', exception %s", iter->name, exception->ToString ());
    }
  }
}

void PluginManager::UnloadPlugins (const char* wc_mask)
{
  if (!wc_mask)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::PluginManager::UnloadPlugins", "wc_mask");

  impl->application_server->RemovePlugins (wc_mask);
}

/*
   Загрузка конфигурации
*/

void PluginManager::LoadConfiguration (System::Xml::XmlNode^ configuration)
{
  impl->application_server->LoadConfiguration (configuration);
}

/*
   Создание формы
*/

ChildForm::Pointer PluginManager::CreateForm (const char* plugin, const char* init_string, FormDockState dock_state)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::PluginManager::CreateForm";

  if (!plugin)
    throw xtl::make_null_argument_exception (METHOD_NAME, "plugin");

  if (!init_string)
    throw xtl::make_null_argument_exception (METHOD_NAME, "init_string");

  try
  {
    return impl->application_server->CreateForm (impl->window_system, plugin, init_string, dock_state);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}
