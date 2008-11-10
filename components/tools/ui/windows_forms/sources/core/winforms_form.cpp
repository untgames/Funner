#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Конструктор / деструктор
*/

Form::Form (tools::ui::windows_forms::WindowSystem& in_window_system)
  : window_system (in_window_system),
    menu_strip (0)
{
}

Form::~Form ()
{
}

/*
    Изменение текста формы
*/

void Form::SetText (const char* text)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::Form::SetText";

  if (!text)
    throw xtl::make_null_argument_exception (METHOD_NAME, "text");
    
  try
  {
    FormHandle ()->Text = gcnew String (text);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

const char* Form::Text ()
{
  try
  {
    return get_string (FormHandle ()->Text, text);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::Text", exception);
  }
}

/*
    Управление видимостью формы
*/

void Form::Show ()
{
  try
  {
    FormHandle ()->Show ();
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::Show", exception);
  }
}

void Form::Hide ()
{
  try
  {
    FormHandle ()->Hide ();
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::Hide", exception);
  }
}

void Form::SetVisible (bool state)
{
  try
  {
    FormHandle ()->Visible = state;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::SetVisible", exception);
  }
}

bool Form::IsVisible ()
{
  try
  {
    return FormHandle ()->Visible;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::IsVisible", exception);
  }
}

/*
    Установка меню
*/

void Form::SetMenuStrip (tools::ui::windows_forms::MenuStrip* in_menu_strip)
{
  try
  {
    if (menu_strip)
      FormHandle ()->Controls->Remove (menu_strip->Handle ());

    menu_strip = in_menu_strip;

    if (menu_strip)
      FormHandle ()->Controls->Add (menu_strip->Handle ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::SetMenuStrip", exception);
  }
}

/*
    Добавление / удаление панели кнопок
*/

void Form::Insert (windows_forms::ToolStrip* tool_strip)
{
  static const char* METHOD_NAME = "tools::ui::Form::Insert(tools::ui::windows_forms::ToolStrip*)";

  if (!tool_strip)
    throw xtl::make_null_argument_exception (METHOD_NAME, "tool_strip");
    
  try
  {    
    FormHandle ()->Controls->Add (tool_strip->Handle ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }  
}

void Form::Remove (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    return;
    
  try
  {
    FormHandle ()->Controls->Add (tool_strip->Handle ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Низкоуровневый дескриптор окна
*/

const void* Form::WindowHandle ()
{
  try
  {
    return (const void*)FormHandle ()->Handle;
  }
  catch (...)
  {
    return 0;
  }
}

/*
    Регистрация шлюзов
*/

void Form::RegisterInvokers (script::Environment& environment, const char* library_name)
{
  using namespace script;

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //регистрация свойств

  lib.Register ("set_Text", make_invoker (&Form::SetText));
  lib.Register ("get_Text", make_invoker (&Form::Text));
  lib.Register ("set_Visible", make_invoker (&Form::SetVisible));
  lib.Register ("get_Visible", make_invoker (&Form::IsVisible));
  lib.Register ("set_MenuStrip", make_invoker (&Form::SetMenuStrip));
  lib.Register ("get_MenuStrip", make_invoker (&Form::MenuStrip));  

    //регистрация методов

  lib.Register ("Add", make_invoker (&Form::Insert));
  lib.Register ("Remove", make_invoker (&Form::Remove));

    //регистрация типов данных

  environment.RegisterType<Form> (library_name);
}
