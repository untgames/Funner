#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Утилиты
*/

typedef void (Form::*CancelEventMemFn)(Object^, System::ComponentModel::CancelEventArgs^);

private ref class CancelEventHandlerDispatcher
{
  public:
    CancelEventHandlerDispatcher (Form* in_form, CancelEventMemFn in_fn) : form (in_form), fn (in_fn) { }

    void Process (Object^ sender, System::ComponentModel::CancelEventArgs^ args)
    {
      ((*form).*fn)(sender, args);
    }

  private:
    Form*            form;
    CancelEventMemFn fn;    
};

/*
    Описание реализации формы
*/

typedef msclr::gcroot<System::Windows::Forms::Form^>                FormPtr;
typedef msclr::gcroot<System::EventHandler^>                        EventHandlerPtr;
typedef msclr::gcroot<System::ComponentModel::CancelEventHandler^>  CancelEventHandlerPtr;

struct Form::Impl
{
  windows_forms::WindowSystem& window_system;                  //оконная система
  windows_forms::MenuStrip*    menu_strip;                     //цепочка меню формы
  FormPtr                      form_handle;                    //ref-указатель на форму
  AutoString                   text;                           //текст формы
  bool                         is_close_canceled;              //было ли отменено закрытие формы
  stl::string                  event_handlers [FormEvent_Num]; //обработчики событий формы
  CancelEventHandlerPtr        on_closing_handler;             //обработчик закрытия формы
  EventHandlerPtr              on_closed_handler;              //обработчик удаления формы
  
  Impl (windows_forms::WindowSystem& in_window_system) :
    window_system (in_window_system),
    menu_strip (0) {}
};

/*
    Конструктор / деструктор
*/

Form::Form (tools::ui::windows_forms::WindowSystem& in_window_system)
  : impl (new Impl (in_window_system))
{
  impl->on_closed_handler  = make_event_handler (this, &Form::OnClosed);
  impl->on_closing_handler = gcnew System::ComponentModel::CancelEventHandler (gcnew CancelEventHandlerDispatcher (this, &Form::OnClosing), &CancelEventHandlerDispatcher::Process);
}

Form::~Form ()
{
  try
  {
    SetHandle (nullptr);
  }  
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Получение / установка ref-указателя формы    
*/

void Form::SetHandle (System::Windows::Forms::Form^ handle)
{
  if (impl->form_handle)
  {
    impl->form_handle->Closed  -= impl->on_closed_handler;
    impl->form_handle->Closing -= impl->on_closing_handler;
  }

  impl->form_handle = handle;  

  if (handle)
  {
    handle->Closed  += impl->on_closed_handler;
    handle->Closing += impl->on_closing_handler;
  }
}

System::Windows::Forms::Form^ Form::Handle ()
{
  return impl->form_handle;
}
    
System::Windows::Forms::Form^ Form::CheckedHandle ()
{
  if (!impl->form_handle)
    throw xtl::format_operation_exception ("tools::ui::windows_forms::Form::CheckedHandle", "Form is closed");
    
  return impl->form_handle;
}

/*
    Получение оконной системы
*/

windows_forms::WindowSystem& Form::WindowSystem ()
{
  return impl->window_system;
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
    CheckedHandle ()->Text = gcnew String (text);
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
    return get_string (CheckedHandle ()->Text, impl->text);
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
    CheckedHandle ()->Show ();
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
    CheckedHandle ()->Hide ();
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
    CheckedHandle ()->Visible = state;
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
    return CheckedHandle ()->Visible;
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
    if (impl->menu_strip)
      CheckedHandle ()->Controls->Remove (impl->menu_strip->Handle ());

    impl->menu_strip = in_menu_strip;

    if (impl->menu_strip)
      CheckedHandle ()->Controls->Add (impl->menu_strip->Handle ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::SetMenuStrip", exception);
  }
}

windows_forms::MenuStrip* Form::MenuStrip ()
{
  return impl->menu_strip;
}

/*
    Добавление / удаление панели кнопок
*/

void Form::Insert (windows_forms::ToolStrip* tool_strip)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::Form::Insert(tools::ui::windows_forms::ToolStrip*)";

  if (!tool_strip)
    throw xtl::make_null_argument_exception (METHOD_NAME, "tool_strip");
    
  try
  {    
    CheckedHandle ()->Controls->Add (tool_strip->Handle ());
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
    CheckedHandle ()->Controls->Add (tool_strip->Handle ());
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
    return (const void*)CheckedHandle ()->Handle;
  }
  catch (...)
  {
    return 0;
  }
}

/*
    Обработка закрытия формы
*/

void Form::Close ()
{
  try
  {
    impl->is_close_canceled = false;
    
    CheckedHandle ()->Close ();
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::Form::Close", exception);
  }
}

void Form::CancelClose ()
{
  impl->is_close_canceled = true;
}

void Form::OnClosed (Object^, EventArgs^)
{
  const char* command = impl->event_handlers [FormEvent_OnClosed].c_str ();

  if (!*command)
    return;

  try
  {
    impl->window_system.Execute (command);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::Form::OnClosed");
    throw;
  }
}

void Form::OnClosing (Object^, System::ComponentModel::CancelEventArgs^ args)
{
  const char* command = impl->event_handlers [FormEvent_OnClosing].c_str ();

  if (!*command)
    return;

  impl->is_close_canceled = false;

  try
  {
    impl->window_system.Execute (command);

    args->Cancel = impl->is_close_canceled;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::Form::OnClosing");
    throw;
  }
}

/*
    Подписка на события формы
*/

void Form::SetEventHandler (FormEvent event, const char* command)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::Form::SetEventHandler";

  if (event < 0 || event >= FormEvent_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "event", event);

  if (!command)
    throw xtl::make_null_argument_exception (METHOD_NAME, "command");
    
  impl->event_handlers [event] = command;
}

const char* Form::EventHandler (FormEvent event) const
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::Form::SetEventHandler";

  if (event < 0 || event >= FormEvent_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "event", event);

  return impl->event_handlers [event].c_str ();
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
  lib.Register ("set_OnClosing", make_invoker<void (Form&, const char*)> (xtl::bind (&Form::SetEventHandler, _1, FormEvent_OnClosing, _2)));
  lib.Register ("get_OnClosing", make_invoker<const char* (Form&)> (xtl::bind (&Form::EventHandler, _1, FormEvent_OnClosing)));
  lib.Register ("set_OnClosed", make_invoker<void (Form&, const char*)> (xtl::bind (&Form::SetEventHandler, _1, FormEvent_OnClosed, _2)));
  lib.Register ("get_OnClosed", make_invoker<const char* (Form&)> (xtl::bind (&Form::EventHandler, _1, FormEvent_OnClosed)));

    //регистрация методов

  lib.Register ("Add", make_invoker (&Form::Insert));
  lib.Register ("Remove", make_invoker (&Form::Remove));
  lib.Register ("Close", make_invoker (&Form::Close));
  lib.Register ("CancelClose", make_invoker (&Form::CancelClose));

    //регистрация типов данных

  environment.RegisterType<Form> (library_name);
}
