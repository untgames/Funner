///////////////////////////////////////////////////////////////////////////////////////////////////
///Парсер конфигураций
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationParser
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConfigurationParser  (const char* file_name, WindowSystem& window_system, PluginManager& plugin_manager);
    ~ConfigurationParser ();

  private:
    void                     ParseConfiguration   (System::Xml::XmlNode^);
    MenuStripItem::Pointer   ParseMenuStripItem   (System::Xml::XmlNode^);
    MenuStrip::Pointer       ParseMenuStrip       (System::Xml::XmlNode^);
    ToolStripButton::Pointer ParseToolStripButton (System::Xml::XmlNode^);
    ToolStrip::Pointer       ParseToolStrip       (System::Xml::XmlNode^);
    stl::string              GenerateUid          ();

  private:
    WindowSystem&            window_system;     //оконная система
    PluginManager&           plugin_manager;    //менеджер плагинов
    size_t                   next_uid;          //следующий номер уникального идентификатора
    common::Log              log;               //лог
};
