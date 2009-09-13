#include "shared.h"

#import <UIApplication.h>
#import <UITextField.h>
#import <UIWindow.h>

#import <UIColor.h>

using namespace script;

namespace
{

const char* VIRTUAL_KEYBOARD_LIBRARY = "VirtualKeyboard";
const char* COMPONENT_NAME           = "script.binds.VirtualKeyboard";
const char* BINDER_NAME              = "VirtualKeyboard";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс реализующий работу с клавиатурой
///////////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardView
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    KeyboardView ()
    {
      CGRect frame_rect;

      frame_rect.origin.x    = 1000;
      frame_rect.origin.y    = 1000;
      frame_rect.size.width  = 0;
      frame_rect.size.height = 0;

      text_field = [[UITextField alloc] initWithFrame:frame_rect];

      if (!text_field)
        throw xtl::format_operation_exception ("engine::script::binds::KeyboardView::KeyboardView", "Can't create needed text field");

      [[UIApplication sharedApplication].keyWindow addSubview: text_field];
    }

    ~KeyboardView ()
    {
      [text_field release];
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление клавиатурой
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowKeyboard ()
    {
      if (![text_field becomeFirstResponder])
        throw xtl::format_operation_exception ("engine::script::binds::KeyboardView::ShowKeyboard", "Can't show keyboard");
    }

    void HideKeyboard ()
    {
      if (![text_field resignFirstResponder])
        throw xtl::format_operation_exception ("engine::script::binds::KeyboardView::ShowKeyboard", "Can't hide keyboard");
    }

  private:
    UITextField *text_field; //вид, с помощью которого реализуется работа с клавиатурой
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон работы с клавиатурой
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<KeyboardView> KeyboardViewSingleton;

void show_keyboard ()
{
  KeyboardViewSingleton::Instance ()->ShowKeyboard ();
}

void hide_keyboard ()
{
  KeyboardViewSingleton::Instance ()->HideKeyboard ();
}

void bind_keyboard_library (script::Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (VIRTUAL_KEYBOARD_LIBRARY);

    //регистрация операций

  lib.Register ("Show", make_invoker (&show_keyboard));
  lib.Register ("Hide", make_invoker (&hide_keyboard));
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_keyboard_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> VirtualKeyboardScriptBind (COMPONENT_NAME);

}

}
