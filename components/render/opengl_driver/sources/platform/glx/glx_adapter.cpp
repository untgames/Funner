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
  Log               log;               //протокол работы OpenGL
  OutputManager     output_manager;    //менеджер устройств вывода
  AdapterLibraryPtr library;           //библиотека адаптера
  stl::string       name;              //имя адаптера
  
///Конструктор
  Impl (const char* in_name, const char* in_dll_path, const char* init_string)  
    : library (LibraryManager::LoadLibrary (in_dll_path))
    , name (in_name)
  {
  }  
};

/*
    Конструктор / деструктор
*/

Adapter::Adapter (const char* name, const char* dll_path, const char* init_string)
{
  try
  {
      //проверка корректности аргументов    
    
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");
      
    if (!init_string)
      throw xtl::make_null_argument_exception ("", "init_string");

      //создание реализации
    
    impl = new Impl (name, dll_path, init_string);
    
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
  
  log.Printf ("Destroy adapter '%s'", impl->name.c_str ());
  
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

IAdapterLibrary& Adapter::GetLibrary ()
{
  return *impl->library;
}
