#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Константы
*/

namespace
{

const char* INTERPERTER_NAME                   = "lua";                 //имя интерпретатора
const char* MENU_STRIP_COLLECTION_LIBRARY_NAME = "MenuStripCollection"; //имя библиотеки коллекции цепочек меню
const char* MENU_ITEM_COLLECTION_LIBRARY_NAME  = "MenuItemCollection";  //имя библиотеки коллекции элементов меню

}

/*
    Конструктор / деструктор
*/

WindowSystem::WindowSystem (IApplicationServer* server)
  : application_server (server),
    shell_environment (new script::Environment),
    shell (INTERPERTER_NAME, shell_environment)
{
  try
  {
      //создание главной формы

    main_form = create_main_form (server);

    main_form->Show ();    
    
      //регистрация шлюзов
      
    RegisterInvokers ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::WindowSystem");
    throw;
  }
}

WindowSystem::~WindowSystem ()
{
}

/*
    Подсчёт ссылок
*/

void WindowSystem::AddRef ()
{
  addref (this);
}

void WindowSystem::Release ()
{
  release (this);
}

/*
    Выполнение команды на стороне оконной системы
*/

namespace
{

void print_errors (const char* message)
{
  printf ("%s\n", message);
}

}

void WindowSystem::ExecuteCommand (const char* command)
{
  try
  {
    shell.Execute (command, &print_errors);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::WindowSystem::ExecuteCommand");
    throw;
  }
}

/*
    Загрузка файлов конфигурации
*/

void WindowSystem::LoadConfiguration (const char* file_name_mask)
{
  using namespace common;

  if (!file_name_mask)
    throw xtl::make_null_argument_exception ("tools::ui::WindowSystem::LoadConfiguration", "file_name_mask");

  for (FileListIterator i=FileSystem::Search (file_name_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
    ConfigurationParser (i->name, *this);
}

/*
    Регистрация шлюзов
*/

void WindowSystem::RegisterInvokers ()
{
  try
  {
    using namespace script;    

      //регистрация шлюзов контролов

    ToolForm::RegisterInvokers          (*shell_environment);    
    ToolMenuItem::RegisterInvokers      (*shell_environment);
    ToolMenuStrip::RegisterInvokers     (*shell_environment);
    MenuItemRegistry::RegisterInvokers  (*shell_environment, MENU_ITEM_COLLECTION_LIBRARY_NAME);
    MenuStripRegistry::RegisterInvokers (*shell_environment, MENU_STRIP_COLLECTION_LIBRARY_NAME);

      //создание глобальной точки входа

    InvokerRegistry& lib = shell_environment->Library ("static.Application");

      //регистрация свойств

    lib.Register ("get_MainForm",   make_const (main_form));
    lib.Register ("get_MenuItems",  make_const (xtl::ref (menu_items)));
    lib.Register ("get_MenuStrips", make_const (xtl::ref (menu_strips)));

      //регистрация методов

    lib.Register ("LoadConfiguration", make_invoker<void (const char*)> (xtl::bind (&WindowSystem::LoadConfiguration, this, _1)));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::RegisterInvokers");
    throw;
  }
}
