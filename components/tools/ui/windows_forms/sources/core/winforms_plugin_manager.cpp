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
    ApplicationServerImpl ()
    {
      plugins = new PluginMap;

      var_registry_container = new StringRegistryContainer;

      var_registry_container->Mount (REGISTRY_BRANCH);

      var_registry = new common::VarRegistry (REGISTRY_BRANCH);
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
    }

///Получение / установка значения свойства
    virtual void SetProperty (System::String^ const name, System::String^ const value)
    {
      AutoString var_name (name), var_value (value);

      var_registry->SetValue (var_name.Data (), xtl::any (stl::string (var_value.Data ()), true));
    }

    virtual void GetProperty (System::String^ const name, System::String^ value)
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

      PropertyListenerEntry^ new_listener = gcnew PropertyListenerEntry (name_wc_mask, listener);

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

///Создание формы
    ChildForm::Pointer CreateForm (WindowSystem* window_system, const char* plugin, const char* init_string)
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ApplicationServerImpl::CreateForm";

      PluginMap::iterator plugin_iter = plugins->find (plugin);

      if (plugin_iter == plugins->end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't create form with plugin %s, plugin not registered", plugin);

      System::String^ init_string_wrap = gcnew System::String (init_string);

      System::Windows::Forms::Form^ new_form = plugin_iter->second.plugin->CreateForm (init_string_wrap);

      return ChildForm::Create (*window_system, new_form, FormDockState_Default);
    }

  private:
    void VarRegistryEventHandler (const char* var_name, common::VarRegistryEvent event)
    {
    }

  private:
    ref struct PropertyListenerEntry
    {
      System::String^    name_wc_mask;
      IPropertyListener^ listener;

      PropertyListenerEntry (System::String^ in_name_wc_mask, IPropertyListener^ in_listener) 
        : name_wc_mask (in_name_wc_mask), listener (in_listener)
        {}
    };

    typedef stl::hash_map<stl::hash_key<const char*>, PluginEntry> PluginMap;
    typedef common::VarRegistryContainer<stl::string>              StringRegistryContainer;

  private:
    PluginMap*               plugins;                //добавленные плагины
    StringRegistryContainer* var_registry_container; //контейнер реестра переменных
    common::VarRegistry*     var_registry;           //реестр переменных
    ArrayList                property_listeners;     //слушатели событий свойств
};

}

}

} 

/*
   Менеджер плагинов
*/

/*
   Конструктор/деструктор
*/

PluginManager::PluginManager (WindowSystem& in_window_system)
  : window_system (&in_window_system), log (LOG_NAME)
{
  try
  {
    application_server = gcnew ApplicationServerImpl ();
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
      common::InputFile plugin_file (iter->name);

      array<unsigned char>^ raw_assembly = gcnew array<unsigned char> ((int)plugin_file.Size ());

      xtl::uninitialized_storage<unsigned char> plugin_file_content (plugin_file.Size ());

      plugin_file.Read (plugin_file_content.data (), plugin_file_content.size ());

      for (int i = 0; i < (int)plugin_file_content.size (); i++)
        raw_assembly->SetValue (plugin_file_content.data ()[i], i);

      Assembly^ assembly = Assembly::Load (raw_assembly);

      Type ^plugin_class = assembly->GetType ("tools.ui.windows_forms.Plugin");

      if (!plugin_class)
        throw xtl::format_operation_exception ("", "Class 'tools.ui.windows_forms.Plugin' not found");

      MethodInfo^ create_plugin_func = plugin_class->GetMethod ("InitPlugin", gcnew array<Type^> {IApplicationServer::typeid});

      if (!create_plugin_func)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' not found in class 'tools.ui.windows_forms.Plugin'");

      if (!create_plugin_func->IsStatic || !create_plugin_func->IsPublic)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' in class 'tools.ui.windows_forms.Plugin' must be static public");

      array<Object^>^ params = {application_server};

      create_plugin_func->Invoke (nullptr, params);
    }
    catch (xtl::exception& exception)
    {
      log.Printf ("Can't load plugin '%s', exception %s", iter->name, exception.what ());
    }
    catch (System::Exception^ exception)
    {
      log.Printf ("Can't load plugin '%s', exception %s", iter->name, exception->ToString ());
    }
  }
}

void PluginManager::UnloadPlugins (const char* wc_mask)
{
  if (!wc_mask)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::PluginManager::UnloadPlugins", "wc_mask");

  application_server->RemovePlugins (wc_mask);
}

/*
   Загрузка конфигурации
*/

void PluginManager::LoadConfiguration (System::Xml::XmlNode^ configuration)
{
}

/*
   Создание формы
*/

ChildForm::Pointer PluginManager::CreateForm (const char* plugin, const char* init_string)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::PluginManager::CreateForm";

  if (!plugin)
    throw xtl::make_null_argument_exception (METHOD_NAME, "plugin");

  if (!init_string)
    throw xtl::make_null_argument_exception (METHOD_NAME, "init_string");

  try
  {
    return application_server->CreateForm (window_system, plugin, init_string);
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
