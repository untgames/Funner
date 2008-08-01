#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Описание реализации главной формы
*/

namespace tools
{

namespace ui
{

namespace windows_forms
{

private ref class TestForm: public WeifenLuo::WinFormsUI::DockContent
{
  public:
    TestForm ()
    {
      InitializeComponent ();
    }
    
  private:
    void InitializeComponent ()
    {
      Text        = "Fucking dock!";
      ShowHint    = WeifenLuo::WinFormsUI::DockState::DockRight/*AutoHide*/;
      HideOnClose = true;
    }    
};

private ref class MainFormImpl: public System::Windows::Forms::Form
{
  public:
///Конструктор
    MainFormImpl ()
    {
      InitializeComponent ();
      IsMdiContainer = true;      
    }

///Деструктор
    ~MainFormImpl ()
    {
    }
    
    void InitializeComponent ()
    {
      dock_panel = gcnew WeifenLuo::WinFormsUI::DockPanel;
      
      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;

      Controls->Add (dock_panel);
      
      form = gcnew TestForm;      

      form->Show (dock_panel);

      ResumeLayout ();
    }

  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //стыковочная панель
    TestForm^                         form;
};

}

}

}

/*
    Конструктор / деструктор
*/

MainForm::MainForm (WindowSystem& in_window_system)
  : window_system (in_window_system),
    menu_strip (0)
{
  form = gcnew MainFormImpl;
}

MainForm::~MainForm ()
{
}

/*
    Создание формы
*/

MainForm::Pointer MainForm::Create (WindowSystem& window_system)
{
  return Pointer (new MainForm (window_system), false);
}

/*
    Изменение текста формы
*/

void MainForm::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("tools::ui::windows_MainForms::MainForm::SetText", "text");

  form->Text = gcnew String (text);
}

const char* MainForm::Text ()
{
  return get_string (form->Text, text);
}

/*
    Управление видимостью формы
*/

void MainForm::Show ()
{
  form->Show ();
}

void MainForm::Hide ()
{
  form->Hide ();
}

void MainForm::SetVisible (bool state)
{
  form->Visible = state;
}

bool MainForm::IsVisible ()
{
  return form->Visible;
}

/*
    Установка меню
*/

void MainForm::SetMenuStrip (tools::ui::windows_forms::MenuStrip* in_menu_strip)
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

void MainForm::Insert (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    throw xtl::make_null_argument_exception ("tools::ui::MainForm::Insert(tools::ui::windows_MainForms::ToolStrip*)", "tool_strip");
    
  form->Controls->Add (tool_strip->Handle ());
}

void MainForm::Remove (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    return;
    
  form->Controls->Add (tool_strip->Handle ());
}

/*
    Регистрация шлюзов
*/

void MainForm::RegisterInvokers (script::Environment& environment, const char* library_name)
{
  using namespace script;

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //регистрация свойств

  lib.Register ("set_Text", make_invoker (&MainForm::SetText));
  lib.Register ("get_Text", make_invoker (&MainForm::Text));
  lib.Register ("set_Visible", make_invoker (&MainForm::SetVisible));
  lib.Register ("get_Visible", make_invoker (&MainForm::IsVisible));
  lib.Register ("set_MenuStrip", make_invoker (&MainForm::SetMenuStrip));
  lib.Register ("get_MenuStrip", make_invoker (&MainForm::MenuStrip));  

    //регистрация методов

  lib.Register ("Add", make_invoker (&MainForm::Insert));
  lib.Register ("Remove", make_invoker (&MainForm::Remove));

    //регистрация типов данных

  environment.RegisterType<MainForm> (library_name);
}
