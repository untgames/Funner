#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;
using namespace common;
using namespace System::Xml;

namespace
{

const char* LOG_NAME = "tools.ui.windows_forms.ConfigurationParser";

String^ get_node_path (XmlNode^ node)
{
  String^ parent_path;

  if (!node->ParentNode)
    return "";

  parent_path = get_node_path (node->ParentNode);

  if (parent_path->Length)
    return String::Concat (String::Concat (parent_path, "::"), node->Name);
  else
    return String::Concat (parent_path, node->Name);
}

}

/*
    Конструктор / деструктор
*/

ConfigurationParser::ConfigurationParser (const char* file_name, WindowSystem& in_window_system)
  : window_system (in_window_system),
    next_uid (0),
    log (LOG_NAME)
{  
  static const char* METHOD_NAME = "tools::ui::window_forms::ConfigurationParser::ConfigurationParser";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  System::Xml::XmlDocument ^xml_configuration;

  try
  {
      //загрузка файла

    InputFile xml_file (file_name);

    xtl::uninitialized_storage<char> file_content (xml_file.Size ());

    xml_file.Read (file_content.data (), file_content.size ());

    String ^xml_string = gcnew String (file_content.data (), 0, file_content.size ());

    xml_configuration = gcnew XmlDocument;

    xml_configuration->LoadXml (xml_string);
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
  
    //разбор конфигурации    

  XmlNode ^root_node = xml_configuration->DocumentElement;

  AutoString root_node_name (root_node->Name);

  if (xtl::xstrcmp ("UIConfig", root_node_name.Data ()))
    throw xtl::format_operation_exception (METHOD_NAME, "'UIConfig' tag missing");

  ParseConfiguration (root_node);
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

void ConfigurationParser::ParseConfiguration (XmlNode^ document)
{
  XmlNodeList ^node_list = document->SelectNodes ("MenuItem");

  for (int i = 0; i < node_list->Count; i++)
    ParseMenuStripItem (node_list[i]);

  node_list = document->SelectNodes ("MenuStrip");

  for (int i = 0; i < node_list->Count; i++)
    ParseMenuStrip (node_list[i]);

  node_list = document->SelectNodes ("ToolButton");

  for (int i = 0; i < node_list->Count; i++)
    ParseToolStripButton (node_list[i]);

  node_list = document->SelectNodes ("ToolStrip");

  for (int i = 0; i < node_list->Count; i++)
    ParseToolStrip (node_list[i]);
}

/*
    Разбор меню    
*/

MenuStripItem::Pointer ConfigurationParser::ParseMenuStripItem (XmlNode^ menu_iter)
{
  XmlNode ^source = menu_iter->Attributes ["Source"];

  if (source) //обработка инстанцированных элементов меню
  {
    AutoString source_string (source->InnerText);

    MenuStripItem* instance = window_system.MenuStripItems ().Find (source_string.Data ());

    AutoString node_path (get_node_path (menu_iter));

    if (!instance)
      log.Printf ("MenuItem '%s' not found, while parsing node %s", source_string.Data (), node_path.Data ());

    return instance;
  }
  
  XmlNode ^id       = menu_iter->Attributes ["Id"],
          ^text     = menu_iter->Attributes ["Text"],
          ^tip      = menu_iter->Attributes ["Tip"],
          ^image    = menu_iter->Attributes ["Image"],
          ^on_click = menu_iter->Attributes ["OnClick"];

  if (!text)
  {
    AutoString node_prefix (get_node_path (menu_iter));

    throw xtl::format_operation_exception ("tools::ui::windows_forms::ConfigurationParser::ParseMenuStripItem", 
                                           "No 'Text' attribute in node %s", node_prefix.Data ());
  }

    //создание элемента меню

  MenuStripItem::Pointer menu_item (new MenuStripItem (window_system), false);
  
    //установка свойств меню

  AutoString text_string (text->InnerText);

  menu_item->SetText (text_string.Data ());

  if (tip)
  {
    AutoString tip_string (tip->InnerText);

    menu_item->SetTip (tip_string.Data ());
  }
    
  if (image)
  {
    AutoString image_string (image->InnerText);

    menu_item->SetImage (image_string.Data ());
  }

  if (on_click)
  {
    AutoString on_click_string (on_click->InnerText);

    menu_item->SetOnClickCommand (on_click_string.Data ());        
  }

    //добавление вложенных элементов

  XmlNodeList ^node_list = menu_iter->SelectNodes ("MenuItem");

  for (int i = 0; i < node_list->Count; i++)
  {
    MenuStripItem::Pointer sub_menu_item = ParseMenuStripItem (node_list[i]);

    if (!sub_menu_item)
      continue;

    insert (*menu_item, *sub_menu_item);
  }

    //регистрация элемента в реестре

  if (id)
  {
    AutoString id_string (id->InnerText);

    window_system.MenuStripItems ().Register (id_string.Data (), menu_item);
  }
  else
    window_system.MenuStripItems ().Register (GenerateUid ().c_str (), menu_item);

  return menu_item;
}

MenuStrip::Pointer ConfigurationParser::ParseMenuStrip (XmlNode^ menu_iter)
{
  XmlNode ^id = menu_iter->Attributes ["Id"];

    //создание цепочки меню

  MenuStrip::Pointer menu_strip (new MenuStrip, false);  

    //добавление пунктов меню
    
  XmlNodeList ^node_list = menu_iter->SelectNodes ("MenuItem");

  for (int i = 0; i < node_list->Count; i++)
  {
    MenuStripItem::Pointer sub_menu_item = ParseMenuStripItem (node_list[i]);

    if (!sub_menu_item)
      continue;

    menu_strip->Insert (*sub_menu_item);
  }

    //регистрация элемента в реестре  

  if (id)
  {
    AutoString id_string (id->InnerText);

    window_system.MenuStrips ().Register (id_string.Data (), menu_strip);
  }
  else
    window_system.MenuStrips ().Register (GenerateUid ().c_str (), menu_strip);

  return menu_strip;
}

/*
    Разбор панелей кнопок
*/

ToolStripButton::Pointer ConfigurationParser::ParseToolStripButton (XmlNode^ button_iter)
{
  XmlNode ^source = button_iter->Attributes ["Source"];

  AutoString node_path (get_node_path (button_iter));

  if (source) //обработка инстанцированных элементов меню
  {
    AutoString source_string (source->InnerText);

    ToolStripButton* instance = window_system.ToolStripButtons ().Find (source_string.Data ());

    if (!instance)
      log.Printf ("ToolButton '%s' not found, while parsing node %s", source_string.Data (), node_path.Data ());

    return instance;
  }
  
  XmlNode ^id       = button_iter->Attributes ["Id"],
          ^text     = button_iter->Attributes ["Text"],
          ^tip      = button_iter->Attributes ["Tip"],
          ^image    = button_iter->Attributes ["Image"],
          ^on_click = button_iter->Attributes ["OnClick"];  

    //создание элемента меню

  ToolStripButton::Pointer button (new ToolStripButton (window_system), false);
  
  if (!text && !image)
  {
    log.Printf ("'Text' and 'Image' tags missing in node %s", node_path.Data ());
    return 0;
  }
  
    //установка свойств меню
    
  if (text)
  {
    AutoString text_string (text->InnerText);
    
    button->SetText (text_string.Data ());
  }

  if (tip)
  {
    AutoString tip_string (tip->InnerText);

    button->SetTip (tip_string.Data ());
  }
    
  if (image)
  {
    AutoString image_string (image->InnerText);

    button->SetImage (image_string.Data ());
  }

  if (on_click)
  {
    AutoString on_click_string (on_click->InnerText);
    
    button->SetOnClickCommand (on_click_string.Data ());        
  }

    //регистрация элемента в реестре

  if (id)
  {
    AutoString id_string (id->InnerText);

    window_system.ToolStripButtons ().Register (id_string.Data (), button);
  }
  else
    window_system.ToolStripButtons ().Register (GenerateUid ().c_str (), button);

  return button;
}

ToolStrip::Pointer ConfigurationParser::ParseToolStrip (XmlNode^ tool_strip_iter)
{
  XmlNode ^id = tool_strip_iter->Attributes ["Id"];
    
    //создание цепочки меню

  ToolStrip::Pointer tool_strip (new ToolStrip, false);  

    //добавление кнопок

  XmlNodeList ^node_list = tool_strip_iter->SelectNodes ("ToolButton");

  for (int i = 0; i < node_list->Count; i++)
  {
    ToolStripButton::Pointer button = ParseToolStripButton (node_list[i]);

    if (!button)
      continue;

    tool_strip->Insert (*button);
  }

    //регистрация элемента в реестре  

  if (id)
  {
    AutoString id_string (id->InnerText);

    window_system.ToolStrips ().Register (id_string.Data (), tool_strip);
  }
  else
    window_system.ToolStrips ().Register (GenerateUid ().c_str (), tool_strip);
  
  return tool_strip;
}
