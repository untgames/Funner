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
    
  private:
    void InitializeComponent ()
    {
      ShowHint = WeifenLuo::WinFormsUI::DockState::DockLeftAutoHide;      
    }    
};

private ref class MainForm: public System::Windows::Forms::Form
{
  public:
///Конструктор
    MainForm ()
    {
      IsMdiContainer = true;      
      dock_panel     = gcnew WeifenLuo::WinFormsUI::DockPanel;
      
      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;

      Controls->Add (dock_panel);

      TestForm^ form = gcnew TestForm;      

      form->Show (dock_panel);

      ResumeLayout (false);
      PerformLayout ();      
    }

///Деструктор
    ~MainForm ()
    {
    }
    
  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //стыковочная панель
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
