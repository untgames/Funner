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
  : window_system (in_window_system)
{
  try
  {
    item = gcnew T;

    item->Click += make_event_handler (this, &MenuItem::OnClick);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MenuItem<T>::MenuItem", exception);
  }
}

template <class T>
MenuItem<T>::~MenuItem ()
{
  try
  {
    item = 0;
  }
  catch (...)
  {
    //подавление всех исключений
  }
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
  static const char* METHOD_NAME = "tools::ui::windows_forms::MenuItem::SetText";

  if (!text)
    throw xtl::make_null_argument_exception (METHOD_NAME, "text");

  try
  {
    item->Text = gcnew String (text);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

template <class T>
const char* MenuItem<T>::Text ()
{
  try
  {
    return get_string (item->Text, text);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MenuItem<T>::Text", exception);
  }
}

/*
    Подсказка
*/

template <class T>
void MenuItem<T>::SetTip (const char* tip)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::MenuItem::SetTip";

  if (!tip)
    throw xtl::make_null_argument_exception (METHOD_NAME, "tip");

  try
  {
    item->ToolTipText = gcnew String (tip);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

template <class T>
const char* MenuItem<T>::Tip ()
{
  try
  {
    return get_string (item->ToolTipText, tip);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MenuItem<T>::Tip", exception);
  }
}

/*
    Картинка
*/

template <class T>
void MenuItem<T>::SetImage (const char* in_image)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::MenuItem::SetImage";

  if (!in_image)
    throw xtl::make_null_argument_exception (METHOD_NAME, "image");
    
  try
  {    
    item->Image = gcnew System::Drawing::Bitmap (gcnew String (in_image));  
    image       = in_image;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
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
  try
  {
    parent.Handle ()->DropDownItems->Add (child.Handle ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::insert(MenuItem<System::Windows::Forms::ToolStripMenuItem>&,MenuItem<System::Windows::Forms::ToolStripMenuItem>&)",
      exception);
  }
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
{
  try
  {
    menu = gcnew T;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Menu<T,Item>::Menu", exception);
  }  
}

template <class T, class Item>
Menu<T, Item>::~Menu ()
{
  try
  {
    menu = 0;
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Добавление пунктов меню
*/

template <class T, class Item>
void Menu<T, Item>::Insert (Item& item)
{
  try
  {
    menu->Items->Add (item.Handle ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Menu<T,Item>::Insert", exception);
  }
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
