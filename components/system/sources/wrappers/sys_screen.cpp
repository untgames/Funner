#include "shared.h"

using namespace syslib;

/*
===================================================================================================
    Экран
===================================================================================================
*/

/*
    Описание реализации экрана
*/

struct Screen::Impl: public xtl::reference_counter
{
  screen_t handle; //платформо-зависимый дескриптор экрана
  
///Конструкторы
  Impl (size_t index)
    : handle ()
  {
    handle = Platform::CreateScreen (index);
    
    if (!handle)
      throw xtl::format_operation_exception ("", "Internal error: can't create screen (handle is null)");
  }
  
  Impl (screen_t in_handle)
    : handle (in_handle)
  {
    if (!handle)
      throw xtl::format_operation_exception ("", "Internal error: can't create screen (handle is null)");
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
      Platform::DestroyScreen (handle);
    }
    catch (...)
    {
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Screen::Screen (size_t screen_index)
{
  try
  {
    impl = new Impl (screen_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Screen");
    throw;
  }
}

Screen::Screen (Impl* in_impl)
  : impl (in_impl)
{
}

Screen::Screen (const Screen& screen)
  : impl (screen.impl)
{
  addref (impl);
}

Screen::~Screen ()
{
  release (impl);
}

Screen& Screen::operator = (const Screen& screen)
{
  Screen (screen).Swap (*this);

  return *this;
}

/*
    Имя
*/

const char* Screen::Name () const
{
  try
  {
    return Platform::GetScreenName (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Name");
    throw;
  }
}

/*
    Размеры экрана
*/

size_t Screen::Width () const
{
  try
  {
    ScreenModeDesc desc;
    
    memset (&desc, 0, sizeof (desc));
    
    GetCurrentMode (desc);
    
    return desc.width;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Width");
    throw;
  }
}

size_t Screen::Height () const
{
  try
  {
    ScreenModeDesc desc;
    
    memset (&desc, 0, sizeof (desc));
    
    GetCurrentMode (desc);
    
    return desc.height;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Height");
    throw;
  }
}

/*
    Количество бит на пиксель
*/

size_t Screen::ColorBits () const
{
  try
  {
    ScreenModeDesc desc;
    
    memset (&desc, 0, sizeof (desc));
    
    GetCurrentMode (desc);
    
    return desc.color_bits; 
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::ColorBits");
    throw;
  }
}

/*
    Частота обновления
*/

size_t Screen::RefreshRate () const
{
  try
  {
    ScreenModeDesc desc;
    
    memset (&desc, 0, sizeof (desc));
    
    GetCurrentMode (desc);
    
    return desc.refresh_rate;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::RefreshRate");
    throw;
  }
}

/*
   Плотность пикселей на дюйм
*/

size_t Screen::Xdpi () const
{
  try
  {
    ScreenModeDesc desc;

    memset (&desc, 0, sizeof (desc));

    GetCurrentMode (desc);

    return desc.xdpi;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Xdpi");
    throw;
  }
}

size_t Screen::Ydpi () const
{
  try
  {
    ScreenModeDesc desc;

    memset (&desc, 0, sizeof (desc));

    GetCurrentMode (desc);

    return desc.ydpi;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Ydpi");
    throw;
  }
}

/*
    Получение списка видео-режимов
*/

size_t Screen::ModesCount () const
{
  try
  {
    return Platform::GetScreenModesCount (impl->handle);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::ModesCount");
    throw;
  }
}

void Screen::GetMode (size_t mode_index, ScreenModeDesc& mode_desc) const
{
  try
  {
    Platform::GetScreenMode (impl->handle, mode_index, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetMode");
    throw;
  }
}

/*
    Установка текущего видео-режима
*/

void Screen::SetCurrentMode (const ScreenModeDesc& mode_desc)
{
  try
  {
    Platform::SetScreenCurrentMode (impl->handle, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::SetCurrentMode");
    throw;
  }
}

void Screen::RestoreDefaultMode ()
{
  try
  {
    ScreenModeDesc mode;

    GetDefaultMode (mode);
    SetCurrentMode (mode);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::RestoreDefaultMode");
    throw;
  }
}

void Screen::GetCurrentMode (ScreenModeDesc& mode_desc) const
{
  try
  {
    Platform::GetScreenCurrentMode (impl->handle, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetCurrentMode");
    throw;
  }
}

void Screen::GetDefaultMode (ScreenModeDesc& mode_desc) const
{
  try
  {
    Platform::GetScreenDefaultMode (impl->handle, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetDefaultMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией
*/

void Screen::SetGammaRamp (const Color3f table [256])
{
  try
  {
    Platform::SetScreenGammaRamp (impl->handle, table);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::SetGammaRamp");
    throw;
  }
}

void Screen::GetGammaRamp (Color3f table [256]) const
{
  try
  {
    Platform::GetScreenGammaRamp (impl->handle, table);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetGammaRamp");
    throw;
  }
}

/*
    Платформо-зависимый дескриптор экрана
*/

const void* Screen::Handle () const
{
  try
  {
    return Platform::GetNativeScreenHandle (impl->handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Handle");
    throw;
  }
}

/*
    Получение платформо-зависимых свойств экрана
*/

void Screen::GetProperties (common::PropertyMap& properties)
{
  try
  {
    properties.Clear ();
    
    return Platform::GetScreenProperties (impl->handle, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetProperties");
    throw;
  }
}

/*
    Поиск экрана вмещающего окно
*/

Screen Screen::ContainingScreen (const void* window_native_handle)
{
  try
  {
    return Screen (new Impl (Platform::FindContainingScreen (window_native_handle)));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Screen::ContainingScreen(const void*)");
    throw;
  }
}

/*
    Обмен
*/

void Screen::Swap (Screen& screen)
{
  stl::swap (impl, screen.impl);
}

namespace syslib
{

void swap (Screen& screen1, Screen& screen2)
{
  screen1.Swap (screen2);
}

}

/*
    Сравнение
*/

bool Screen::operator == (const Screen& screen) const
{
  return impl->handle == screen.impl->handle;
}

bool Screen::operator != (const Screen& screen) const
{
  return !(*this == screen);
}

/*
===================================================================================================
    Менеджер экранов
===================================================================================================
*/

/*
    Перечисление экранов
*/

size_t ScreenManager::ScreensCount ()
{
  try
  {
    return Platform::GetScreensCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenManager::ScreensCount");
    throw;
  }
}

Screen ScreenManager::Screen (size_t index)
{
  try
  {
    return syslib::Screen (index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenManager::Screen");
    throw;
  }
}
