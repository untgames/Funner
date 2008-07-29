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
    void                   ParseConfiguration (const common::Parser::Iterator&);
    ToolMenuItem::Pointer  ParseMenuItem      (const common::Parser::Iterator&);
    ToolMenuItem::Pointer  ParseSubMenuItem   (const common::Parser::Iterator&);
    ToolMenuStrip::Pointer ParseMenuStrip     (const common::Parser::Iterator&);
    stl::string            GenerateUid        ();

  private:
    WindowSystem&    window_system; //оконная система
    common::ParseLog parse_log;     //протокол разбора
    common::Parser   parser;        //парсер
    size_t           next_uid;      //следующий номер уникального идентификатора
};
