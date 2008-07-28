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
ToolFormPtr create_main_form (IApplicationServer* server)
{
  return ToolFormPtr (new ToolForm (server, gcnew MainForm), false);
}

}

}

}
