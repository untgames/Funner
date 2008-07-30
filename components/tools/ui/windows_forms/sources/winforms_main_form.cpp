#include "shared.h"

namespace
{

/*
    Главная форма
*/

private ref class MainForm: public System::Windows::Forms::Form
{
  public:
///Конструктор
    MainForm ()
    {
      IsMdiContainer = true;
    }

///Деструктор
    ~MainForm ()
    {
    }
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
