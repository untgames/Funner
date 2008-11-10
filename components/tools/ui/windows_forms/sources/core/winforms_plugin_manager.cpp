#include "shared.h"

#using <funner.tools.ui.windows_forms.PluginApi.dll>

using namespace System::Reflection;
using namespace System::Collections;
using namespace tools::ui::windows_forms;

namespace
{

const char* LOG_NAME = "tools.ui.windows_forms.PluginManager";

typedef msclr::gcroot<Assembly^>  AssemblyPtr;

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
      printf ("ApplicationServerImpl::ApplicationServerImpl\n");
    }

    ~ApplicationServerImpl ()
    {
      printf ("ApplicationServerImpl::~ApplicationServerImpl\n");
    }

///Добавление плагина
    virtual void AddPlugin (System::String^ const plugin_name, IPlugin^ plugin)
    {
      AutoString plugin_name_string (plugin_name);

      printf ("ApplicationServerImpl::AddPlugin %s\n", plugin_name_string.Data ());
    }

///Выполнение команды на стороне приложения
    virtual void ExecuteCommand (System::String^ const command)
    {
    }

///Получение / установка значения свойства
    virtual void SetProperty (System::String^ const name, System::String^ const value)
    {
    }

    virtual void GetProperty (System::String^ const name, System::String^ value)
    {
    }

///Проверка наличия свойства / проверка режима "только чтения" свойства
    virtual System::Boolean IsPropertyPresent (System::String^ const name)
    {
      return false;
    }

///Регистрация слушателей событий свойств
    virtual void RegisterPropertyListener (System::String^ const name_wc_mask, IPropertyListener^ listener)
    {
    }

    virtual void UnregisterPropertyListener (System::String^ const name_wc_mask, IPropertyListener^ listener)
    {
    }

    virtual void UnregisterAllPropertyListeners (System::String^ const name_wc_mask)
    {
    }

    virtual void UnregisterAllPropertyListeners ()
    {
    }
};

}

}

} 

struct PluginManager::Plugin: public xtl::reference_counter
{
  AssemblyPtr  assembly;
  stl::string  name;

  Plugin (const char* file_name, ApplicationServerImpl^ application_server)
    : name (file_name)
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    try
    {
      String ^file_name_string = gcnew String (file_name);

      common::InputFile plugin_file (file_name);

      array<unsigned char>^ raw_assembly = gcnew array<unsigned char> ((int)plugin_file.Size ());

      xtl::uninitialized_storage<unsigned char> plugin_file_content (plugin_file.Size ());

      plugin_file.Read (plugin_file_content.data (), plugin_file_content.size ());

      for (int i = 0; i < (int)plugin_file_content.size (); i++)
        raw_assembly->SetValue (plugin_file_content.data ()[i], i);

      assembly = Assembly::Load (raw_assembly);

      Type ^plugin_class = assembly->GetType ("tools.ui.windows_forms.Plugin");

      if (!plugin_class)
        throw xtl::format_operation_exception ("", "Class 'tools.ui.windows_forms.Plugin' not found");

      MethodInfo^ create_plugin_func = plugin_class->GetMethod ("InitPlugin");

      if (!create_plugin_func)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' not found in class 'tools.ui.windows_forms.Plugin'");

      if (!create_plugin_func->IsStatic || !create_plugin_func->IsPublic)
        throw xtl::format_operation_exception ("", "Method 'InitPlugin (IApplicationServer)' in class 'tools.ui.windows_forms.Plugin' must be static public");

      array<Object^>^ params = {application_server};

      create_plugin_func->Invoke (nullptr, params);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("tools::ui::windows_forms::PluginManager::Plugin::Plugin");
      throw;
    }
    catch (System::Exception^ exception)
    {
      throw DotNetException ("tools::ui::windows_forms::PluginManager::Plugin::Plugin", exception);
    }
  }
};

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
      plugins.push_back (PluginPtr (new Plugin (iter->name, application_server), false));
    }
    catch (xtl::exception& exception)
    {
      log.Printf ("Can't load plugin '%s', exception %s", iter->name, exception.what ());
    }
  }
}

bool PluginManager::UnloadPredicate (PluginPtr plugin, const char* wc_mask)
{
  return common::wcmatch (plugin->name.c_str (), wc_mask);
}

void PluginManager::UnloadPlugins (const char* wc_mask)
{
  if (!wc_mask)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::PluginManager::UnloadPlugins", "wc_mask");

  plugins.remove_if (xtl::bind (&PluginManager::UnloadPredicate, this, _1, wc_mask));
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

ChildForm::Pointer PluginManager::CreateForm (const char* plugin, const char* form_type)
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::PluginManager::CreateForm");
}
