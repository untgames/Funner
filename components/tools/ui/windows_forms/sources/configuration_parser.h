///////////////////////////////////////////////////////////////////////////////////////////////////
///Парсер конфигураций
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationParser
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConfigurationParser  (const char* file_name, WindowSystem& window_system);
    ~ConfigurationParser ();

  private:
    void                     ParseConfiguration   (const common::Parser::Iterator&);
    MenuStripItem::Pointer   ParseMenuStripItem   (const common::Parser::Iterator&);
    MenuStrip::Pointer       ParseMenuStrip       (const common::Parser::Iterator&);
    ToolStripButton::Pointer ParseToolStripButton (const common::Parser::Iterator&);
    ToolStrip::Pointer       ParseToolStrip       (const common::Parser::Iterator&);    
    stl::string              GenerateUid          ();

  private:
    WindowSystem&    window_system; //оконная система
    common::ParseLog parse_log;     //протокол разбора
    common::Parser   parser;        //парсер
    size_t           next_uid;      //следующий номер уникального идентификатора
};
