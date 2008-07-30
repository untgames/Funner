///////////////////////////////////////////////////////////////////////////////////////////////////
///Оконная система на базе Windows Forms
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowSystem: public ICustomWindowSystem, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowSystem  (IApplicationServer*);
    ~WindowSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение команды на стороне оконной системы
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Execute (const char* name, const void* buffer, size_t buffer_size);
    void Execute (const char* command);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetLogHandler (const LogFunction& log);
    const LogFunction& GetLogHandler () { return log_handler; }
    
    void LogMessage (const char* message);    
    void LogPrintf  (const char* format, ...);
    void LogVPrintf (const char* format, va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение сервера приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    IApplicationServer& ApplicationServer () { return *application_server; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Реестры контролов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef ControlRegistry<MenuStripItem>   MenuStripItemRegistry;
    typedef ControlRegistry<MenuStrip>       MenuStripRegistry;
    typedef ControlRegistry<ToolStripButton> ToolStripButtonRegistry;
    typedef ControlRegistry<ToolStrip>       ToolStripRegistry;    

    MenuStripItemRegistry&   MenuStripItems   () { return menu_strip_items; }
    MenuStripRegistry&       MenuStrips       () { return menu_strips; }
    ToolStripButtonRegistry& ToolStripButtons () { return tool_strip_buttons; }
    ToolStripRegistry&       ToolStrips       () { return tool_strips; }

  private:
    void RegisterInvokers  ();
    void LoadConfiguration (const char* file_name_mask);

  private:
    typedef xtl::shared_ptr<script::Environment> ShellEnvironmentPtr;

  private:
    ApplicationServerPtr    application_server; //сервер приложения
    Form::Pointer           main_form;          //главная форма приложения
    ShellEnvironmentPtr     shell_environment;  //окружение скриптовой среды
    script::Shell           shell;              //скриптовый интерпретатор
    MenuStripItemRegistry   menu_strip_items;   //реестр элементов меню
    MenuStripRegistry       menu_strips;        //реестр цепочек меню
    ToolStripButtonRegistry tool_strip_buttons; //реестр кнопок
    ToolStripRegistry       tool_strips;        //реестр цепочек кнопок
    LogFunction             log_handler;        //функция протоколирования
};
