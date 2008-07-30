#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

template MenuStripItem;
template MenuStrip;
template ToolStripButton;
template ToolStrip;

/*
===================================================================================================
    MenuItem
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

template <class T>
MenuItem<T>::MenuItem (WindowSystem& in_window_system)
  : item (gcnew T),
    window_system (in_window_system)
{
  item->Click += make_event_handler (this, &MenuItem::OnClick);
}

template <class T>
MenuItem<T>::~MenuItem ()
{
}

/*
    Обработчик выбора пункта меню
*/

template <class T>
void MenuItem<T>::OnClick (System::Object^, System::EventArgs^)
{
  if (!on_click_command.empty ())
  {
    window_system.Execute (on_click_command.c_str ());
  }
}

/*
    Установка текста
*/

template <class T>
void MenuItem<T>::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::MenuItem::SetText", "text");

  item->Text = gcnew String (text);
}

template <class T>
const char* MenuItem<T>::Text ()
{
  return get_string (item->Text, text);
}

/*
    Подсказка
*/

template <class T>
void MenuItem<T>::SetTip (const char* tip)
{
  if (!tip)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::MenuItem::SetTip", "tip");

  item->ToolTipText = gcnew String (tip);
}

template <class T>
const char* MenuItem<T>::Tip ()
{
  return get_string (item->ToolTipText, tip);
}

/*
    Картинка
*/

template <class T>
void MenuItem<T>::SetImage (const char* in_image)
{
  if (!in_image)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::MenuItem::SetImage", "image");
    
  item->Image = gcnew System::Drawing::Bitmap (gcnew String (in_image));  
  image       = in_image;
}

/*
    Установка команды, выполняемой при выборе пункта меню
*/

template <class T>
void MenuItem<T>::SetOnClickCommand (const char* command)
{
  if (!command)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::MenuItem::SetOnClickCommand", "command");

  on_click_command = command;
}

/*
    Добавление вложений
*/

namespace tools
{

namespace ui
{

namespace windows_forms
{

void insert
 (MenuItem<System::Windows::Forms::ToolStripMenuItem>& parent,
  MenuItem<System::Windows::Forms::ToolStripMenuItem>& child)
{
  parent.Handle ()->DropDownItems->Add (child.Handle ());
}

}

}

}

/*
    Регистрация шлюзов
*/

template <class T>
void MenuItem<T>::RegisterInvokers (script::Environment& environment, const char* library_name)
{
  using namespace script;
  
    //создание библиотеки
  
  InvokerRegistry& lib = environment.CreateLibrary (library_name);
  
    //регистрация свойств
    
  lib.Register ("get_Text",    make_invoker (&MenuItem::Text));
  lib.Register ("set_Text",    make_invoker (&MenuItem::SetText));
  lib.Register ("get_Tip",     make_invoker (&MenuItem::Tip));
  lib.Register ("set_Tip",     make_invoker (&MenuItem::SetTip));
  lib.Register ("get_Image",   make_invoker (&MenuItem::Image));
  lib.Register ("set_Image",   make_invoker (&MenuItem::SetImage));
  lib.Register ("get_OnClick", make_invoker (&MenuItem::OnClickCommand));
  lib.Register ("set_OnClick", make_invoker (&MenuItem::SetOnClickCommand));

    //регистрация типов данных

  environment.RegisterType<MenuItem> (library_name);
}

/*
===================================================================================================
    MenuStrip
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

template <class T, class Item>
Menu<T, Item>::Menu ()
  : menu (gcnew T)
{
}

template <class T, class Item>
Menu<T, Item>::~Menu ()
{
}

/*
    Добавление пунктов меню
*/

template <class T, class Item>
void Menu<T, Item>::Insert (Item& item)
{
  menu->Items->Add (item.Handle ());  
}

/*
    Регистрация шлюзов
*/

template <class T, class Item>
void Menu<T, Item>::RegisterInvokers (script::Environment& environment, const char* library_name)
{
  using namespace script;
  
    //создание библиотеки
  
  InvokerRegistry& lib = environment.CreateLibrary (library_name);  

    //регистрация типов данных

  environment.RegisterType<Menu> (library_name);
}
