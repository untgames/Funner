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

private ref class MainFormImpl: public System::Windows::Forms::Form
{
  public:
///Конструктор
    MainFormImpl ()
    {
      dock_panel = gcnew WeifenLuo::WinFormsUI::Docking::DockPanel;
      
      IsMdiContainer = true;

      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;

      Controls->Add (dock_panel);      

      ResumeLayout ();
    }
    
///Стыковочная панель
    WeifenLuo::WinFormsUI::Docking::DockPanel^ DockPanel () { return dock_panel; }

  private:
    WeifenLuo::WinFormsUI::Docking::DockPanel^ dock_panel; //стыковочная панель
};

}

}

}

/*
    Конструктор / деструктор
*/

MainForm::MainForm (tools::ui::windows_forms::WindowSystem& window_system)
  : Form (window_system)
{
  try
  {
    form = gcnew MainFormImpl;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MainForm::MainForm", exception);
  }
}

MainForm::~MainForm ()
{
  try
  {
    form = 0;
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Создание формы
*/

MainForm::Pointer MainForm::Create (tools::ui::windows_forms::WindowSystem& window_system)
{
  return Pointer (new MainForm (window_system), false);
}

/*
    Добавление дочерней формы
*/

void MainForm::Insert (WeifenLuo::WinFormsUI::Docking::DockContent^ sub_form)
{
  try
  {
    sub_form->Show (form->DockPanel ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MainForm::Insert", exception);
  }  
}

/*
    Получение ref-указателя формы
*/

System::Windows::Forms::Form^ MainForm::FormHandle ()
{
  return form;
}

/*
    Регистрация шлюзов
*/

void MainForm::RegisterInvokers (script::Environment& environment, const char* base_library_name, const char* library_name)
{
  using namespace script;  

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //наследование от Form

  lib.Register (environment.Library (base_library_name));  

    //регистрация типов данных

  environment.RegisterType<MainForm> (library_name);
}
