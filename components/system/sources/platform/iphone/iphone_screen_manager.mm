#include "shared.h"

using namespace syslib;

namespace
{

const float DEFAULT_DPI = 160.f;

void fill_screen_mode_desc (UIScreen* screen, UIScreenMode* mode, ScreenModeDesc& mode_desc)
{
  float scale = 1;

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    scale = screen.scale;

  memset (&mode_desc, 0, sizeof (mode_desc));

  mode_desc.width  = screen.bounds.size.width * scale;  //!!!!!!TODO multimode screen compatibility - needs testing on device with multimode support
  mode_desc.height = screen.bounds.size.height * scale;
  mode_desc.xdpi   = DEFAULT_DPI * scale;
  mode_desc.ydpi   = DEFAULT_DPI * scale;
}

}

/*
    Создание / удаление экрана
*/

screen_t IPhoneScreenManager::CreateScreen  (size_t screen_index)
{
  NSArray* screens = [UIScreen screens];

  if (screen_index >= [screens count])
    throw xtl::make_range_exception ("syslib::IPhoneScreenManager::CreateScreen", "screen_index", screen_index, 0u, [screens count]);

  return (screen_t)[[screens objectAtIndex:screen_index] retain];
}

void IPhoneScreenManager::DestroyScreen (screen_t screen)
{
  [(UIScreen*)screen release];
}

/*
    Перечисление экранов
*/

size_t IPhoneScreenManager::GetScreensCount ()
{
  return [[UIScreen screens] count];
}

/*
    Имя экрана
*/

const char* IPhoneScreenManager::GetScreenName (screen_t screen)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetScreenName", "screen");

  return [[(UIScreen*)screen description] UTF8String];
}

/*
    Получение списка видео-режимов экрана
*/

size_t IPhoneScreenManager::GetScreenModesCount (screen_t screen)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetScreenModesCount", "screen");

  return [[(UIScreen*)screen availableModes] count];
}

void IPhoneScreenManager::GetScreenMode (screen_t screen, size_t mode_index, ScreenModeDesc& mode_desc)
{
  static const char* METHOD_NAME = "syslib::IPhoneScreenManager::GetScreenMode";

  if (!screen)
    throw xtl::make_null_argument_exception (METHOD_NAME, "screen");

  UIScreen *ui_screen       = (UIScreen*)screen;
  NSArray  *available_modes = [ui_screen availableModes];

  if (mode_index >= [available_modes count])
    throw xtl::make_range_exception (METHOD_NAME, "mode_index", mode_index, 0u, [available_modes count]);

  fill_screen_mode_desc (ui_screen, [available_modes objectAtIndex:mode_index], mode_desc);
}

/*
    Установка текущего видео-режима экрана
*/

void IPhoneScreenManager::SetScreenCurrentMode (screen_t screen, const ScreenModeDesc& mode_desc)
{
  static const char* METHOD_NAME = "syslib::IPhoneScreenManager::SetScreenCurrentMode";

  if (!screen)
    throw xtl::make_null_argument_exception (METHOD_NAME, "screen");

  UIScreen *ui_screen       = (UIScreen*)screen;
  NSArray  *available_modes = [ui_screen availableModes];

  for (UIScreenMode* mode in available_modes)
  {
    if (mode.size.width == mode_desc.width && mode.size.height == mode_desc.height)
    {
      ui_screen.currentMode = mode;
      return;
    }
  }

  throw xtl::make_argument_exception (METHOD_NAME, "mode_desc", "Incompatible screen mode");
}

void IPhoneScreenManager::GetScreenCurrentMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetScreenCurrentMode", "screen");

  UIScreen *ui_screen = (UIScreen*)screen;

  fill_screen_mode_desc (ui_screen, ui_screen.currentMode, mode_desc);
}

void IPhoneScreenManager::GetScreenDefaultMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetScreenDefaultMode", "screen");

  UIScreen *ui_screen = (UIScreen*)screen;

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.3" options:NSNumericSearch] != NSOrderedAscending)
    fill_screen_mode_desc (ui_screen, ui_screen.preferredMode, mode_desc);
  else
    fill_screen_mode_desc (ui_screen, [[ui_screen availableModes] objectAtIndex:0], mode_desc);
}

/*
    Управление гамма-коррекцией экрана
*/

void IPhoneScreenManager::SetScreenGammaRamp (screen_t screen, const Color3f table [256])
{
  throw xtl::format_not_supported_exception ("syslib::IPhoneScreenManager::SetScreenGammaRamp", "Gamma ramp not supported on iOS platform");
}

void IPhoneScreenManager::GetScreenGammaRamp (screen_t screen, Color3f table [256])
{
  for (size_t i = 0; i < 256; i++)
  {
    float value = i / 255.f;

    table[i].red   = value;
    table[i].green = value;
    table[i].blue  = value;
  }
}

/*
    Поиск экрана вмещающего окно
*/

screen_t IPhoneScreenManager::FindContainingScreen (const void* window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::FindContainingScreen", "window");

  return (screen_t)[((UIWindow*)window).screen retain];
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* IPhoneScreenManager::GetNativeScreenHandle (screen_t screen)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetNativeScreenHandle", "screen");

  return screen;
}

/*
    Получение платформо-зависимых свойств экрана
*/

void IPhoneScreenManager::GetScreenProperties (screen_t screen, common::PropertyMap& properties)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::IPhoneScreenManager::GetScreenProperties", "screen");
}
