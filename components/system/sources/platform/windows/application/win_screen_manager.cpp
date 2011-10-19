#include "../shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_SCREEN_ARRAY_RESERVE = 8; //резервируемое количество экранов

/*
    Менеджер экранов
*/

struct ScreenDesc
{
  
};

struct ScreenManagerImpl
{
};

typedef common::Singleton<ScreenManagerImpl> ScreenManagerSingleton;

}

/*
    Перечисление экранов
*/

size_t WindowsScreenManager::GetScreensCount ()
{
  return 0;
}

/*
    Имя экрана
*/

const char* WindowsScreenManager::GetScreenName (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenName");
}

/*
    Получение списка видео-режимов экрана
*/

size_t WindowsScreenManager::GetScreenModesCount (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenModesCount");
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

void WindowsScreenManager::RestoreScreenDefaultMode (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::RestoreScreenDefaultMode");
}

void WindowsScreenManager::GetScreenCurrentMode (screen_t, ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenCurrentMode");
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

screen_t WindowsScreenManager::FindContainingScreen (const void* native_window_handle)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::FindContainingScreen");
}
