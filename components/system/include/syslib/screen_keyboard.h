#ifndef SYSLIB_SCREEN_KEYBOARD_HEADER
#define SYSLIB_SCREEN_KEYBOARD_HEADER

#include <stl/auto_ptr.h>

namespace syslib
{

//forward declaration
class Window;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип экранной клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ScreenKeyboardType
{
  ScreenKeyboardType_Ascii,
  ScreenKeyboardType_AsciiAutocapitalized,
  ScreenKeyboardType_Number,
  ScreenKeyboardType_NumberPunctuation,

  ScreenKeyboardType_PlatformSpecific,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Экранная клавиатура
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScreenKeyboard
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScreenKeyboard  (syslib::Window& window, ScreenKeyboardType type, const char* platform_specific = "");
    ~ScreenKeyboard ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Показана ли клавиатура
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsShown () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Показ и скрытие клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Show ();
    void Hide ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка поддержки клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsSupported (ScreenKeyboardType type);
    static bool IsSupported (const char* keyboard_name);

  private:
    ScreenKeyboard (const ScreenKeyboard&); //no implementation
    ScreenKeyboard& operator = (const ScreenKeyboard&); //no implementation

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
