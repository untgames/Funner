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
    window_system.LogPrintf ("%s\n", parse_log.Message (i));
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
  for_each_child (iter, "MenuItem", xtl::bind (&ConfigurationParser::ParseMenuStripItem, this, _1));
  for_each_child (iter, "MenuStrip", xtl::bind (&ConfigurationParser::ParseMenuStrip, this, _1));
  for_each_child (iter, "ToolButton", xtl::bind (&ConfigurationParser::ParseToolStripButton, this, _1));
  for_each_child (iter, "ToolStrip", xtl::bind (&ConfigurationParser::ParseToolStrip, this, _1));  
}

/*
    Разбор меню    
*/

MenuStripItem::Pointer ConfigurationParser::ParseMenuStripItem (const Parser::Iterator& menu_iter)
{
  const char* source = get<const char*> (menu_iter, "Source");

  if (source) //обработка инстанцированных элементов меню
  {
    MenuStripItem* instance = window_system.MenuStripItems ().Find (source);

    if (!instance)
      parse_log.Error (menu_iter, "MenuItem '%s' not found", source);

    return instance;
  }
  
  const char *id       = get<const char*> (menu_iter, "Id"),
             *text     = get<const char*> (menu_iter, "Text"),
             *tip      = get<const char*> (menu_iter, "Tip"),
             *image    = get<const char*> (menu_iter, "Image"),
             *on_click = get<const char*> (menu_iter, "OnClick");
  
  if (!text)
  {
    parse_log.Error (menu_iter, "'Text' tag missing");

    return 0;
  }

    //создание элемента меню

  MenuStripItem::Pointer menu_item (new MenuStripItem (window_system), false);
  
    //установка свойств меню
    
  menu_item->SetText (text);

  if (tip)
    menu_item->SetTip (tip);
    
  if (image)
    menu_item->SetImage (image);

  if (on_click)
    menu_item->SetOnClickCommand (on_click);        

    //добавление вложенных элементов
    
  for (Parser::NamesakeIterator iter=menu_iter->First ("MenuItem"); iter; ++iter)
  {
    MenuStripItem::Pointer sub_menu_item = ParseMenuStripItem (iter);

    if (!sub_menu_item)
      continue;

    insert (*menu_item, *sub_menu_item);
  }

    //регистрация элемента в реестре

  window_system.MenuStripItems ().Register (id ? id : GenerateUid ().c_str (), menu_item);

  return menu_item;
}

MenuStrip::Pointer ConfigurationParser::ParseMenuStrip (const Parser::Iterator& menu_iter)
{
  const char* id = get<const char*> (menu_iter, "Id");

    //создание цепочки меню

  MenuStrip::Pointer menu_strip (new MenuStrip, false);  

    //добавление пунктов меню
    
  for (Parser::NamesakeIterator iter=menu_iter->First ("MenuItem"); iter; ++iter)
  {
    MenuStripItem::Pointer sub_menu_item = ParseMenuStripItem (iter);

    if (!sub_menu_item)
      continue;

    menu_strip->Insert (*sub_menu_item);
  }    

    //регистрация элемента в реестре  

  window_system.MenuStrips ().Register (id ? id : GenerateUid ().c_str (), menu_strip);
  
  return menu_strip;
}

/*
    Разбор панелей кнопок
*/

ToolStripButton::Pointer ConfigurationParser::ParseToolStripButton (const Parser::Iterator& button_iter)
{
  const char* source = get<const char*> (button_iter, "Source");

  if (source) //обработка инстанцированных элементов меню
  {
    ToolStripButton* instance = window_system.ToolStripButtons ().Find (source);

    if (!instance)
      parse_log.Error (button_iter, "ToolButton '%s' not found", source);

    return instance;
  }
  
  const char *id       = get<const char*> (button_iter, "Id"),
             *text     = get<const char*> (button_iter, "Text"),
             *tip      = get<const char*> (button_iter, "Tip"),
             *image    = get<const char*> (button_iter, "Image"),
             *on_click = get<const char*> (button_iter, "OnClick");  

    //создание элемента меню

  ToolStripButton::Pointer button (new ToolStripButton (window_system), false);
  
  if (!text && !image)
  {
    parse_log.Error (button_iter, "'Text' and 'Image' tags missing");
    return 0;
  }
  
    //установка свойств меню
    
  if (text)
    button->SetText (text);

  if (tip)
    button->SetTip (tip);
    
  if (image)
    button->SetImage (image);

  if (on_click)
    button->SetOnClickCommand (on_click);        

    //регистрация элемента в реестре

  window_system.ToolStripButtons ().Register (id ? id : GenerateUid ().c_str (), button);

  return button;
}

ToolStrip::Pointer ConfigurationParser::ParseToolStrip (const Parser::Iterator& tool_strip_iter)
{
  const char* id = get<const char*> (tool_strip_iter, "Id");

    //создание цепочки меню

  ToolStrip::Pointer tool_strip (new ToolStrip, false);  

    //добавление кнопок

  for (Parser::NamesakeIterator iter=tool_strip_iter->First ("ToolButton"); iter; ++iter)
  {
    ToolStripButton::Pointer button = ParseToolStripButton (iter);

    if (!button)
      continue;

    tool_strip->Insert (*button);
  }    

    //регистрация элемента в реестре  

  window_system.ToolStrips ().Register (id ? id : GenerateUid ().c_str (), tool_strip);
  
  return tool_strip;
}
