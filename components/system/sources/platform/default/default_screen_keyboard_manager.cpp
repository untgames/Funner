#include "shared.h"

namespace syslib
{

/*
    Создание и уничтожение экранной клавиатуры
*/

struct screen_keyboard_handle
{
};

screen_keyboard_t DefaultScreenKeyboardManager::CreateScreenKeyboard (window_t, ScreenKeyboardType type, const char* platform_specific_name)
{
  static const char* METHOD_NAME = "syslib::DefaultScreenKeyboardManager::CreateScreenKeyboard";

  if (!IsScreenKeyboardSupported (type))
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  if (!platform_specific_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "platform_specific_name");

  return new screen_keyboard_handle;
}

void DefaultScreenKeyboardManager::DestroyScreenKeyboard (screen_keyboard_t handle)
{
  delete handle;
}

/*
    Показ и скрытие клавиатуры
*/

void DefaultScreenKeyboardManager::ShowScreenKeyboard (screen_keyboard_t)
{
}

void DefaultScreenKeyboardManager::HideScreenKeyboard (screen_keyboard_t)
{
}

/*
    Проверка поддержки клавиатуры
*/

bool DefaultScreenKeyboardManager::IsScreenKeyboardSupported (ScreenKeyboardType type)
{
  return type >= 0 && type < ScreenKeyboardType_PlatformSpecific;
}

bool DefaultScreenKeyboardManager::IsScreenKeyboardSupported (const char*)
{
  return false;
}

}
