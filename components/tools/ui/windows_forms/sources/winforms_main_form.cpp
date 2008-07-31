#include "shared.h"

namespace
{

/*
    Главная форма
*/

private ref class TestForm: public WeifenLuo::WinFormsUI::DockContent
{
  public:
    TestForm ()
    {
      InitializeComponent ();
    }

    ~TestForm ()
    {
      if (components)
      {
        delete components;
      }
    }
    
  private:
    System::ComponentModel::Container^ components;

    void InitializeComponent ()
    {
      components = gcnew System::ComponentModel::Container;

      Text = "Fucking dock!";
      ShowHint = WeifenLuo::WinFormsUI::DockState::DockRight/*AutoHide*/;
      HideOnClose = true;

      ResumeLayout (false);
    }    
};

private ref class MainForm: public System::Windows::Forms::Form
{
  public:
///Конструктор
    MainForm ()
    {
      components = gcnew System::ComponentModel::Container;
      dock_panel = gcnew WeifenLuo::WinFormsUI::DockPanel;
      form       = gcnew TestForm;

      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;

      form->Show (dock_panel);

      IsMdiContainer = true;
      MinimumSize = System::Drawing::Size (640, 480);
      StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
      Controls->Add (dock_panel);

      ResumeLayout (false);
      PerformLayout ();
    }

///Деструктор
    ~MainForm ()
    {
      if (components)
      {
        delete components;
      }
    }

    void ShowDock ()
    {
      form->Show (dock_panel);
    }
    
  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //стыковочная панель
    System::ComponentModel::Container^ components;
    TestForm^ form;
};

}

namespace tools
{

namespace ui
{

namespace windows_forms
{

///Создание главной формы
Form::Pointer create_main_form (WindowSystem& window_system)
{
  return Form::Pointer (new Form (window_system, gcnew MainForm), false);
}

}

}

}
