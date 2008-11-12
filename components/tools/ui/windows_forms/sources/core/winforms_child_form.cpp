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

private ref class ChildFormImpl: public WeifenLuo::WinFormsUI::Docking::DockContent
{
  public:
///Конструктор
    ChildFormImpl (WindowSystem& in_window_system, ICustomChildWindow& in_child_window) :
      window_system (in_window_system), child_window (in_child_window)
    {
        //инициализация свойств формы

      Text         = "Form";
      ShowHint     = WeifenLuo::WinFormsUI::Docking::DockState::Hidden;
      HideOnClose  = true;

        //подписка на события формы

      HandleCreated   += gcnew EventHandler (this, &ChildFormImpl::OnCreateHandle);
      HandleDestroyed += gcnew EventHandler (this, &ChildFormImpl::OnDestroyHandle);
      Resize          += gcnew EventHandler (this, &ChildFormImpl::OnResize);
    }    

  private:
///Обновление родительского окна
    void UpdateParent ()
    {
      try
      {
        try
        {
          child_window.SetParent ((const void*)Handle);
        }
        catch (System::Exception^ exception)
        {
          throw DotNetException ("tools::ui::windows_forms::ChildFormImpl::UpdateParent", exception);
        }
      }
      catch (std::exception& exception)
      {
        window_system.LogPrintf ("%s", exception.what ());
      }
      catch (...)
      {
        window_system.LogPrintf ("Unknown exception at tools::ui::windows_forms::ChildFormImpl::UpdateParent");
      }      
    }

///Обновление размеров
    void UpdateRect ()
    {
      try
      {
        try
        {
          System::Drawing::Rectangle^ rect = ClientRectangle;

          child_window.SetPosition (rect->Left, rect->Top);
          child_window.SetSize     (rect->Width, rect->Height);
        }
        catch (System::Exception^ exception)
        {
          throw DotNetException ("tools::ui::windows_forms::ChildFormImpl::UpdateRect", exception);
        }
      }
      catch (std::exception& exception)
      { 
        window_system.LogPrintf ("%s", exception.what ());
      }
      catch (...)
      {
        window_system.LogPrintf ("Unknown exception at tools::ui::windows_forms::ChildFormImpl::UpdateRect");
      }
    }

///Обработка события создания дескриптора формы
    void OnCreateHandle (System::Object^, EventArgs^)
    {
//      printf ("ChildFormImpl::OnCreateHandle (%p)\n", (const void*)Handle);
      
        //обновление параметров

      UpdateParent ();
      UpdateRect ();

        //отображение окна

      child_window.Show (true);
    }

///Обработка события уничтожения дескриптора формы
    void OnDestroyHandle (System::Object^, EventArgs^)
    {
//      printf ("ChildFormImpl::OnDestroyHandle (%p)\n", (const void*)Handle);

      child_window.Show (false);
      child_window.SetParent (0);
    }

///Обработка события изменения размеров формы
    void OnResize (Object^, EventArgs^)
    {
      UpdateRect ();
    }        

  private:
    WindowSystem&       window_system;
    ICustomChildWindow& child_window;
};

}

}

}

struct ChildForm::Impl
{
  typedef msclr::gcroot<WeifenLuo::WinFormsUI::Docking::DockContent^> HandlePtr;
  typedef xtl::com_ptr<ICustomChildWindow>                            ChildWindowPtr;
  
  HandlePtr      form;         //дескриптор формы
  ChildWindowPtr child_window; //дочернее окно, включенное в форму

  ~Impl ()
  {
    try
    {
      form = 0;
    }
    catch (...)
    {
      //подавление исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

ChildForm::ChildForm (tools::ui::windows_forms::WindowSystem& window_system, const char* init_string, FormDockState dock_state)
  : Form (window_system), impl (new Impl)
{
  static const char* METHOD_NAME = "tools::ui::ChildForm::ChildForm";

  try
  {
      //создание пользовательского дочернего окна

    IApplicationServer& application_server = window_system.ApplicationServer ();

    impl->child_window = Impl::ChildWindowPtr (application_server.CreateChildWindow (init_string), false);

      //создание контейнера

    impl->form = gcnew ChildFormImpl (window_system, *(impl->child_window));
    
      //установка дока

    SetDockState (dock_state);

      //отображение

    window_system.MainForm ().Insert (impl->form);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

ChildForm::~ChildForm ()
{
}

/*
    Создание формы
*/

ChildForm::Pointer ChildForm::Create (tools::ui::windows_forms::WindowSystem& window_system, const char* init_string, FormDockState dock_state)
{
  return Pointer (new ChildForm (window_system, init_string, dock_state), false);
}

ChildForm::Pointer ChildForm::Create (tools::ui::windows_forms::WindowSystem& window_system, System::Windows::Forms::Form^ child, FormDockState dock_state)
{
  ChildForm::Pointer new_form (new ChildForm (window_system, "", dock_state), false);

  child->Hide ();                             //необходимо скрыть форму перед установкой родителя

  child->TopLevel = false;                    //необходимо, чтобы форму можно было установить ребёнком панели
  child->Parent   = new_form->impl->form;
  
  child->Show ();

  return new_form;
}

/*
    Режим стыковки формы
*/

void ChildForm::SetDockState (FormDockState state)
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::ChildForm::SetDockState";

  try
  {
    WeifenLuo::WinFormsUI::Docking::DockState dock_state;

    switch (state)
    {
      case FormDockState_Bottom:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockBottom;
        break;
      case FormDockState_BottomAutoHide:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockBottomAutoHide;
        break;
      case FormDockState_Left:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockLeft;
        break;
      case FormDockState_LeftAutoHide:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockLeftAutoHide;
        break;
      case FormDockState_Right:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockRight;
        break;
      case FormDockState_RightAutoHide:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockRightAutoHide;
        break;
      case FormDockState_Top:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockTop;
        break;
      case FormDockState_TopAutoHide:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::DockTopAutoHide;
        break;
      case FormDockState_Document:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::Document;
        break;
      case FormDockState_Float:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::Float;
        break;
      case FormDockState_Hidden:
        dock_state = WeifenLuo::WinFormsUI::Docking::DockState::Hidden;
        break;
      default:
        throw xtl::make_argument_exception ("", "state", state);
    }    

    impl->form->ShowHint = dock_state;
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

FormDockState ChildForm::DockState ()
{
  static const char* METHOD_NAME = "tools::ui::windows_forms::ChildForm::DockState";

  try
  {
    switch (impl->form->ShowHint)
    {
      case WeifenLuo::WinFormsUI::Docking::DockState::DockBottom:           return FormDockState_Bottom;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockBottomAutoHide:   return FormDockState_BottomAutoHide;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockLeft:             return FormDockState_Left;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockLeftAutoHide:     return FormDockState_LeftAutoHide;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockRight:            return FormDockState_Right;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockRightAutoHide:    return FormDockState_RightAutoHide;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockTop:              return FormDockState_Top;
      case WeifenLuo::WinFormsUI::Docking::DockState::DockTopAutoHide:      return FormDockState_TopAutoHide;
      case WeifenLuo::WinFormsUI::Docking::DockState::Document:             return FormDockState_Document;
      case WeifenLuo::WinFormsUI::Docking::DockState::Float:                return FormDockState_Float;
      default:
      case WeifenLuo::WinFormsUI::Docking::DockState::Hidden:               return FormDockState_Hidden;
    }    
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }  
}

/*
    Получение ref-указателя формы
*/

System::Windows::Forms::Form^ ChildForm::FormHandle ()
{
  return impl->form;
}

/*
    Регистрация шлюзов
*/

void ChildForm::RegisterInvokers (script::Environment& environment, const char* base_library_name, const char* library_name, const char* dock_state_library_name)
{
  using namespace script;  

  InvokerRegistry& child_form_lib = environment.CreateLibrary (library_name);
  InvokerRegistry& dock_state_lib = environment.CreateLibrary (dock_state_library_name);

    //регистрация перечисления режимов стыковки формы

  dock_state_lib.Register ("get_Bottom",         make_const (FormDockState_Bottom));
  dock_state_lib.Register ("get_BottomAutoHide", make_const (FormDockState_BottomAutoHide));
  dock_state_lib.Register ("get_Left",           make_const (FormDockState_Left));
  dock_state_lib.Register ("get_LeftAutoHide",   make_const (FormDockState_LeftAutoHide)); 
  dock_state_lib.Register ("get_Right",          make_const (FormDockState_Right));
  dock_state_lib.Register ("get_RightAutoHide",  make_const (FormDockState_RightAutoHide));
  dock_state_lib.Register ("get_Top",            make_const (FormDockState_Top));
  dock_state_lib.Register ("get_TopAutoHide",    make_const (FormDockState_TopAutoHide));
  dock_state_lib.Register ("get_Document",       make_const (FormDockState_Document));
  dock_state_lib.Register ("get_Center",         make_const (FormDockState_Document));
  dock_state_lib.Register ("get_Float",          make_const (FormDockState_Float));
  dock_state_lib.Register ("get_Hidden",         make_const (FormDockState_Hidden));

    //наследование от Form

  child_form_lib.Register (environment.Library (base_library_name));
  
    //регистрация свойств
    
  child_form_lib.Register ("get_DockState", make_invoker (&ChildForm::DockState));
  child_form_lib.Register ("set_DockState", make_invoker (&ChildForm::SetDockState));

    //регистрация типов данных

  environment.RegisterType<ChildForm> (library_name);
}
