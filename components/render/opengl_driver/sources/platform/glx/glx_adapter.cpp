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
  Log           log;     //протокол работы OpenGL
  OutputArray   outputs; //зарегистрированные устройства вывода
  Library       library; //библиотека точек входа OpenGL
  
///Конструктор
  Impl ()  
  {
    //соединение с дисплеем
    Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
    
    size_t screens_count = 0;
    
    {
      DisplayLock lock (display);

      screens_count = XScreenCount (display);
    }
    
    outputs.reserve (screens_count);
    
    for (size_t screen_number=0; screen_number < screens_count; screen_number++)
      outputs.push_back (Output::Pointer (new Output (display, screen_number), false));
  }  
};

/*
    Конструктор / деструктор
*/

Adapter::Adapter ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
    Имя адаптера / путь к модулю / описание
*/

const char* Adapter::GetName ()
{
  return "GLX";
}

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::glx::Adapter";
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return impl->outputs.size ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return get_pointer (impl->outputs [index]);
}

/*
    Запрос устройства вывода
*/

Output::Pointer Adapter::GetOutput (const void* window_handle)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Adapter::GetOutput(const void*)");
}

/*
    Библиотека
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
