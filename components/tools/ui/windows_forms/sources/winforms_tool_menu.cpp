#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
===================================================================================================
    ToolMenuItem
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

ToolMenuItem::ToolMenuItem (IApplicationServer& in_server)
  : item (gcnew ToolStripMenuItem),
    server (in_server)
{
  item->Click += make_event_handler (this, &ToolMenuItem::OnClick);
}

ToolMenuItem::~ToolMenuItem ()
{
}

/*
    Обработчик выбора пункта меню
*/

void ToolMenuItem::OnClick (System::Object^, System::EventArgs^)
{
  if (!on_click_command.empty ())
    server.ExecuteCommand (on_click_command.c_str ());
}

/*
    Установка текста
*/

void ToolMenuItem::SetText (const stl::string& text)
{
  item->Text = gcnew String (text.c_str ());
}

stl::string ToolMenuItem::Text ()
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::ToolMenuItem::Text");
}

/*
    Подсказка
*/

void ToolMenuItem::SetTip (const stl::string& text)
{
  item->ToolTipText = gcnew String (text.c_str ());
}

stl::string ToolMenuItem::Tip ()
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::ToolMenuItem::Tip");
}

/*
    Установка команды, выполняемой при выборе пункта меню
*/

void ToolMenuItem::SetOnClickCommand (const stl::string& command)
{
  on_click_command = command;
}

/*
    Добавление вложений
*/

void ToolMenuItem::Insert (const ToolMenuItem::Pointer& sub_item)
{
  item->DropDownItems->Add (sub_item->Handle ());
}

/*
    Регистрация шлюзов
*/

void ToolMenuItem::RegisterInvokers (script::Environment&)
{
}

/*
===================================================================================================
    ToolMenuStrip
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

ToolMenuStrip::ToolMenuStrip ()
  : menu (gcnew MenuStrip)
{
}

ToolMenuStrip::~ToolMenuStrip ()
{
}

/*
    Добавление пунктов меню
*/

void ToolMenuStrip::Insert (const ToolMenuItem::Pointer& item)
{
  menu->Items->Add (item->Handle ());  
}

/*
    Регистрация шлюзов
*/

void ToolMenuStrip::RegisterInvokers (script::Environment&)
{
}
