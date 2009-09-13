#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::iphone;

/*
    Описание реализации адаптера
*/

struct Adapter::Impl
{
  Log    log;    //протокол
  Output output; //устройство вывода

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
    exception.touch ("render::low_level::opengl::iphone::Adapter::Adapter");
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
  return "iPhoneAdapter";
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::opengl::iphone::Adapter";
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return 1;
}

IOutput* Adapter::GetOutput (size_t index)
{
  if (index > 0)
    throw xtl::make_range_exception ("render::low_level::opengl::iphone::Adapter::GetOutput", "index", index, 0u, 0u);

  return &impl->output;
}
