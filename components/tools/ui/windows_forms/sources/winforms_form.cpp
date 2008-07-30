#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

namespace
{

/*
    Константы
*/

const char* FORM_LIBRARY = "Form"; //имя библиотеки шлюзов

}

/*
    Конструктор / деструктор
*/

Form::Form (WindowSystem& in_window_system, const HandlePtr& in_form)
  : window_system (in_window_system),
    form (in_form),
    menu_strip (0)
{
}

/*
    Изменение текста формы
*/

void Form::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::Form::SetText", "text");

  form->Text = gcnew String (text);
}

const char* Form::Text ()
{
  return get_string (form->Text, text);
}

/*
    Управление видимостью формы
*/

void Form::Show ()
{
  form->Show ();
}

void Form::Hide ()
{
  form->Hide ();
}

void Form::SetVisible (bool state)
{
  form->Visible = state;
}

bool Form::IsVisible ()
{
  return form->Visible;
}

/*
    Установка меню
*/

void Form::SetMenuStrip (tools::ui::windows_forms::MenuStrip* in_menu_strip)
{
  if (menu_strip)
    form->Controls->Remove (menu_strip->Handle ());

  menu_strip = in_menu_strip;

  if (menu_strip)
    form->Controls->Add (menu_strip->Handle ());
}

/*
    Добавление / удаление панели кнопок
*/

void Form::Insert (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    throw xtl::make_null_argument_exception ("tools::ui::Form::Insert(tools::ui::windows_forms::ToolStrip*)", "tool_strip");
    
  form->Controls->Add (tool_strip->Handle ());
}

void Form::Remove (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    return;
    
  form->Controls->Add (tool_strip->Handle ());
}

/*
    Регистрация шлюзов
*/

void Form::RegisterInvokers (script::Environment& environment)
{
  using namespace script;

  InvokerRegistry& lib = environment.CreateLibrary (FORM_LIBRARY);

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

  environment.RegisterType<Form> (FORM_LIBRARY);  
}

