#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;
using namespace common;

/*
    Конструктор / деструктор
*/

ConfigurationParser::ConfigurationParser (const char* file_name, WindowSystem& in_window_system)
  : window_system (in_window_system),
    parser (parse_log, file_name, "xml"),
    next_uid (0)
{  
  Parser::Iterator iter = parser.Root ()->First ("UIConfig");

  if (!iter)
  {
    parse_log.Error (iter, "'UIConfig' tag missing");
    return;
  }
  
    //разбор конфигурации

  ParseConfiguration (iter);

    //печать ошибок

  for (size_t i=0; i<parse_log.MessagesCount (); i++)
    printf ("%s\n", parse_log.Message (i));
}

ConfigurationParser::~ConfigurationParser ()
{
}

/*
    Получение уникального идентификатора
*/

stl::string ConfigurationParser::GenerateUid ()
{
  if (++next_uid == 0)
  {
    --next_uid;
    throw xtl::format_operation_exception ("tools::ui::ConfigurationParser::GenerateUid", "No free UID's");
  }
  
  return common::format ("Control%u", next_uid);
}

/*
    Разбор конфигурации
*/

void ConfigurationParser::ParseConfiguration (const Parser::Iterator& iter)
{
  for_each_child (iter, "MenuItem", xtl::bind (&ConfigurationParser::ParseMenuItem, this, _1));
  for_each_child (iter, "MenuStrip", xtl::bind (&ConfigurationParser::ParseMenuStrip, this, _1));
}

/*
    Разбор меню    
*/

ToolMenuItem::Pointer ConfigurationParser::ParseMenuItem (const Parser::Iterator& menu_iter)
{
  const char* source = get<const char*> (menu_iter, "Source");

  if (source) //обработка инстанцированных элементов меню
  {
    ToolMenuItem* instance = window_system.MenuItems ().Find (source);

    if (!instance)
      parse_log.Error (menu_iter, "MenuItem '%s' not found", source);

    return instance;
  }
  
  const char *id       = get<const char*> (menu_iter, "Id"),
             *text     = get<const char*> (menu_iter, "Text"),
             *tip      = get<const char*> (menu_iter, "Tip"),
             *on_click = get<const char*> (menu_iter, "OnClick");
  
  if (!text)
  {
    parse_log.Error (menu_iter, "'Text' tag missing");

    return 0;
  }

    //создание элемента меню

  ToolMenuItem::Pointer menu_item (new ToolMenuItem (window_system.ApplicationServer ()), false);
  
    //установка свойств меню
    
  menu_item->SetText (text);

  if (tip)
    menu_item->SetTip (tip);

  if (on_click)
    menu_item->SetOnClickCommand (on_click);

    //добавление вложенных элементов
    
  for (Parser::NamesakeIterator iter=menu_iter->First ("MenuItem"); iter; ++iter)
  {
    ToolMenuItem::Pointer sub_menu_item = ParseMenuItem (iter);

    if (!sub_menu_item)
      continue;

    menu_item->Insert (sub_menu_item);
  }

    //регистрация элемента в реестре

  window_system.MenuItems ().Register (id ? id : GenerateUid ().c_str (), menu_item);

  return menu_item;
}

ToolMenuStrip::Pointer ConfigurationParser::ParseMenuStrip (const Parser::Iterator& menu_iter)
{
  const char* id = get<const char*> (menu_iter, "Id");

    //создание цепочки меню

  ToolMenuStrip::Pointer menu_strip (new ToolMenuStrip, false);  

    //добавление пунктов меню
    
  for (Parser::NamesakeIterator iter=menu_iter->First ("MenuItem"); iter; ++iter)
  {
    ToolMenuItem::Pointer sub_menu_item = ParseMenuItem (iter);

    if (!sub_menu_item)
      continue;

    menu_strip->Insert (sub_menu_item);
  }    

    //регистрация элемента в реестре  

  window_system.MenuStrips ().Register (id ? id : GenerateUid ().c_str (), menu_strip);
  
  return menu_strip;
}
