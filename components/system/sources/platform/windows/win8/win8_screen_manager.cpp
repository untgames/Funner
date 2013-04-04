#include "shared.h"

namespace syslib
{

/*
    Перечисление экранов
*/

size_t WindowsScreenManager::GetScreensCount ()
{
  return 0;
}

/*
    Создание / удаление экрана
*/

screen_t WindowsScreenManager::CreateScreen  (size_t screen_index)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::CreateScreen");
  
  return 0;
}

void WindowsScreenManager::DestroyScreen (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::DestroyScreen");
}

/*
    Имя экрана
*/

const char* WindowsScreenManager::GetScreenName (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenName");
  
  return "";
}

/*
    Получение списка видео-режимов экрана
*/

size_t WindowsScreenManager::GetScreenModesCount (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenModesCount");

  return 0;
}

void WindowsScreenManager::GetScreenMode (screen_t, size_t mode_index, ScreenModeDesc& mode_desc)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenMode");
}

/*
    Установка текущего видео-режима экрана
*/

void WindowsScreenManager::SetScreenCurrentMode (screen_t, const ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::SetScreenCurrentMode");
}

void WindowsScreenManager::GetScreenCurrentMode (screen_t, ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenCurrentMode");
}

void WindowsScreenManager::GetScreenDefaultMode (screen_t, ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenDefaultMode");
}

/*
    Управление гамма-коррекцией экрана
*/

void WindowsScreenManager::SetScreenGammaRamp (screen_t, const Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::SetScreenGammaRamp");
}

void WindowsScreenManager::GetScreenGammaRamp (screen_t, Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenGammaRamp");
}

/*
    Поиск экрана вмещающего окно
*/

screen_t WindowsScreenManager::FindContainingScreen (const void*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::FindContainingScreen");
  
  return 0;
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* WindowsScreenManager::GetNativeScreenHandle (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetNativeScreenHandle");
  
  return 0;
}

/*
    Получение платформо-зависимых свойств экрана
*/

void WindowsScreenManager::GetScreenProperties (screen_t, common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenProperties");
}

}
