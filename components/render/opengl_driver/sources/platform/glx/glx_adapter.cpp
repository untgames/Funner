#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Описание реализации адаптера
*/

typedef stl::vector<Output::Pointer> OutputArray;

struct Adapter::Impl
{
  Log           log;               //протокол работы OpenGL
  OutputManager output_manager;    //менеджер устройств вывода
  Library       library;           //библиотека точек входа OpenGL
  stl::string   name;              //имя адаптера
  
///Конструктор
  Impl ()  
    : name ("GLX")
  {
  }  
};

/*
    Конструктор / деструктор
*/

Adapter::Adapter ()
{
  try
  {
    //создание реализации
    
    impl = new Impl;
    
    impl->log.Printf ("...adapter successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::Adapter");
    
    throw;
  }
}

Adapter::~Adapter ()
{
  Log log = impl->log;

  log.Printf ("Destroy adapter '%s' (path='%s')", impl->name.c_str (), impl->library->GetName ());

  impl = 0;

  log.Printf ("...adapter successfully destroyed");
}

/*
    Имя адаптера / путь к модулю / описание
*/

const char* Adapter::GetName ()
{
  return impl->name.c_str ();
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::glx::Adapter::Adapter";
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->output_manager.GetOutputsCount ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  return impl->output_manager.GetOutput (index);
}

/*
    Запрос устройства вывода
*/

Output::Pointer Adapter::GetOutput (Window window)
{
  return impl->output_manager.FindContainingOutput (window);
}

/*
    Библиотека
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
