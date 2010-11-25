#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Константы
*/

const size_t OUTPUT_ARRAY_RESERVE = 8; //резервируемое количество устройств вывода

}

/*
    Описание реализации адаптера
*/

typedef stl::vector<Output*> OutputArray;

struct Adapter::Impl
{
  Log         log;     //протокол работы OpenGL
  OutputArray outputs; //зарегистрированные устройства вывода
  Library     library; //библиотека точек входа OpenGL
  
///Конструктор
  Impl ()
  {
    outputs.reserve (OUTPUT_ARRAY_RESERVE);
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

  return impl->outputs [index];
}

/*
    Регистрация устройств вывода
*/

void Adapter::RegisterOutput (Output* output)
{
  if (!output)
    return;

  impl->outputs.push_back (output);
}

void Adapter::UnregisterOutput (Output* output)
{
  if (!output)
    return;
    
  for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
    if (*iter == output)
    {
      impl->outputs.erase (iter);

      return;
    }
}

/*
    Запрос устройства вывода
*/

Output::Pointer Adapter::GetOutput (const void* window_handle)
{
  try
  {
    if (!window_handle)
      throw xtl::make_null_argument_exception ("", "window_handle");
      
      //поиск устройства вывода среди уже созданных
      
    for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
      if ((*iter)->GetWindow ().Handle () == window_handle)
        return *iter;

      //создание нового устройства вывода

    return Output::Pointer (new Output (this, window_handle), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Adapter::GetOutput");
    throw;
  }  
}

/*
    Библиотека
*/

ILibrary& Adapter::GetLibrary ()
{
  return impl->library;
}
