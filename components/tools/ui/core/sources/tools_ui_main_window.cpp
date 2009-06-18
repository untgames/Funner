#include "shared.h"

using namespace tools::ui;

/*
    Описанеи реализации главного окна
*/

struct MainWindow::Impl
{
  stl::string           profile;            //имя профиля
  stl::string           application_server; //имя сервера приложения
  CustomWindowSystemPtr window_system;      //оконная система
};

/*
    Конструкторы / деструктор
*/

MainWindow::MainWindow ()
  : impl (new Impl)
{
}

MainWindow::MainWindow (const char* application_server, const char* window_system_profile)
  : impl (new Impl)
{
  try
  {
    impl->window_system      = WindowSystemManagerSingleton::Instance ()->CreateWindowSystem (application_server, window_system_profile);
    impl->profile            = window_system_profile;
    impl->application_server = application_server;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::MainWindow");
    throw;
  }
}

MainWindow::~MainWindow ()
{
}

/*
    Профиль главного окна / имя сервера приложения
*/

const char* MainWindow::Profile () const
{
  return impl->profile.c_str ();
}

const char* MainWindow::ApplicationServer () const
{
  return impl->application_server.c_str ();
}

/*
    Выполнение команды на стороне оконной системы
*/

void MainWindow::ExecuteCommand (const char* command)
{
  try
  {
    if (!command)
      throw xtl::make_null_argument_exception ("", "command");

    if (!impl->window_system)
      throw xtl::format_operation_exception ("", "No window system binded");
      
    impl->window_system->Execute (command, command, strlen (command));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::ExecuteCommand");
    throw;
  }
}

void MainWindow::ExecuteFile (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    if (!impl->window_system)
      throw xtl::format_operation_exception ("", "No window system binded");

    stl::string buffer;

    common::FileSystem::LoadTextFile (file_name, buffer);

    impl->window_system->Execute (file_name, buffer.c_str (), buffer.size ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::ExecuteFile");
    throw;
  }
}

/*
    Установка функции протоколирования
*/

void MainWindow::SetLogHandler (const LogFunction& log)
{
  try
  {
    if (!impl->window_system)
      throw xtl::format_operation_exception ("", "No window system binded");
      
    impl->window_system->SetLogHandler (log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::SetLogHandler");
    throw;
  }
}

const MainWindow::LogFunction& MainWindow::LogHandler () const
{
  try
  {
    if (!impl->window_system)
      throw xtl::format_operation_exception ("", "No window system binded");
      
    return impl->window_system->GetLogHandler ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::LogHandler");
    throw;
  }
}

/*
    Перезапуск
*/

void MainWindow::Restart ()
{
  try
  {
    if (!impl->window_system)
      return;      
      
    stl::string profile            = impl->profile,
                application_server = impl->application_server;
                
    MainWindow ().Swap (*this);
    MainWindow (application_server.c_str (), profile.c_str ()).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::MainWindow::Restart");
    throw;
  }
}

/*
    Обмен
*/

void MainWindow::Swap (MainWindow& wnd)
{
  stl::swap (impl, wnd.impl);
}

namespace tools
{

namespace ui
{

void swap (MainWindow& wnd1, MainWindow& wnd2)
{
  wnd1.Swap (wnd2);
}

}

}
