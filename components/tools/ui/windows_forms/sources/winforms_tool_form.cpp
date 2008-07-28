#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

namespace
{

/*
    Константы
*/

const char* TOOL_FORM_LIBRARY = "ToolForm"; //имя библиотеки шлюзов

}

/*
    Конструктор / деструктор
*/

ToolForm::ToolForm (IApplicationServer* server, const FormPtr& in_form)
  : application_server (server),
    form (in_form)
{
}

/*
    Изменение текста формы
*/

void ToolForm::SetText (const stl::string& text)
{
  form->Text = gcnew String (text.c_str ());
}

stl::string ToolForm::Text ()
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::ToolForm::Text");
}

/*
    Управление видимостью формы
*/

void ToolForm::Show ()
{
  form->Show ();
}

void ToolForm::Hide ()
{
  form->Hide ();
}

void ToolForm::SetVisible (bool state)
{
  form->Visible = state;
}

bool ToolForm::IsVisible ()
{
  return form->Visible;
}

/*
    Регистрация шлюзов
*/

void ToolForm::RegisterInvokers (script::Environment& environment)
{
  using namespace script;

  InvokerRegistry& lib = environment.CreateLibrary (TOOL_FORM_LIBRARY);

    //регистрация методов

  lib.Register ("set_Text", make_invoker (&ToolForm::SetText));
  lib.Register ("get_Text", make_invoker (&ToolForm::Text));
  lib.Register ("set_Visible", make_invoker (&ToolForm::SetVisible));
  lib.Register ("get_Visible", make_invoker (&ToolForm::IsVisible));

    //регистрация типов данных

  environment.RegisterType<ToolForm> (TOOL_FORM_LIBRARY);  
}

