#include "shared.h"

namespace
{

/*
    Главная форма
*/

private ref class MainForm: public Form
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
ToolForm::Pointer create_main_form (IApplicationServer* server)
{
  return ToolForm::Pointer (new ToolForm (server, gcnew MainForm), false);
}

}

}

}
