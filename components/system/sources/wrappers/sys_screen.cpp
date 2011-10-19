#include "shared.h"

using namespace syslib;

/*
===================================================================================================
    Экран
===================================================================================================
*/

/*
    Конструктор
*/

Screen::Screen (size_t in_screen_index)
  : screen_index (in_screen_index)
{
  try
  {
    if (screen_index > Platform::GetScreensCount ())
      throw xtl::make_range_exception ("", "screen_index", screen_index, Platform::GetScreensCount ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::Screen");
    throw;
  }
}

/*
    Имя
*/

const char* Screen::Name () const
{
  try
  {
    return Platform::GetScreenName (screen_index);
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
    Получение списка видео-режимов
*/

size_t Screen::ModesCount () const
{
  try
  {
    return Platform::GetScreenModesCount (screen_index);    
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
    Platform::GetScreenMode (screen_index, mode_index, mode_desc);
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
    Platform::SetScreenCurrentMode (screen_index, mode_desc);
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
    Platform::RestoreScreenDefaultMode (screen_index);
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
    Platform::GetScreenCurrentMode (screen_index, mode_desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetCurrentMode");
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
    Platform::SetScreenGammaRamp (screen_index, table);
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
    Platform::GetScreenGammaRamp (screen_index, table);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Screen::GetGammaRamp");
    throw;
  }
}

/*
    Обмен
*/

void Screen::Swap (Screen& screen)
{
  stl::swap (screen_index, screen.screen_index);
}

namespace syslib
{

void swap (Screen& screen1, Screen& screen2)
{
  screen1.Swap (screen2);
}

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
