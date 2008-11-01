#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

/*
    Описание реализации адаптера
*/

struct Adapter::Impl
{
  Log           log;            //протокол
  OutputManager output_manager; //менеджер устройств вывода

  Impl () {}
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
    exception.touch ("render::low_level::opengl::macosx::Adapter::Adapter");
    throw;
  }
}

Adapter::~Adapter ()
{
}

/*
   Имя адаптера / описание / путь к модулю
*/

const char* Adapter::GetPath ()
{
  return "";
}

const char* Adapter::GetName ()
{
  return "MacOSXAdapter";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::macosx::Adapter";
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
