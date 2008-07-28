#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Константы
*/

namespace
{

const char* INTERPERTER_NAME = "lua"; //имя интерпретатора

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
  if (!file_name_mask)
    throw xtl::make_null_argument_exception ("tools::ui::WindowSystem::LoadConfiguration", "file_name_mask");

  using namespace common;
  
  ParseLog parse_log;

  for (FileListIterator i=FileSystem::Search (file_name_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
    LoadConfigurationFile (i->name, parse_log);
}

void WindowSystem::LoadConfigurationFile (const char* file_name, common::ParseLog& log)
{
  try
  {
    using namespace common;
    
    Parser parser (log, file_name, "xml");
    
    Parser::Iterator iter = parser.Root ()->First ("UIConfig");
    
    if (!iter)
    {
      log.Error (iter, "'UIConfig' tag missing");
      return;
    }
    
//    ParseConfiguration (iter);
  }
  catch (xtl::exception& exception)
  {
      //!!!вставить протоколирование!!!
    throw;
  }
}

//void WindowSystem::ParseConfiguration (Parser::Iterator iter, ParseLog& log)
//{
//  
//}

/*
    Регистрация шлюзов
*/

void WindowSystem::RegisterInvokers ()
{
  try
  {
    using namespace script;    
    
    ToolForm::RegisterInvokers (*shell_environment);    

    InvokerRegistry& lib = shell_environment->Library ("static.Application");

    lib.Register ("get_MainForm", make_const (main_form));
    lib.Register ("LoadConfiguration", make_invoker<void (const char*)> (xtl::bind (&WindowSystem::LoadConfiguration, this, _1)));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::RegisterInvokers");
    throw;
  }
}
