///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий менеджера эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEffectManagerListener
{
  public:
    virtual void OnConfigurationChanged () {}
    
  protected:
    virtual ~IEffectManagerListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
class EffectManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectManager  (const DeviceManagerPtr&, const TextureManagerPtr&, const ProgramManagerPtr&, IEffectManagerListener* listener = 0);
    ~EffectManager ();    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: является ли ресурс библиотекой эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsEffectLibraryResource (const char* name);
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadEffectLibrary   (const char* name);
    void UnloadEffectLibrary (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение прокси
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectProxy GetEffectProxy (const char* name); //создание прокси в случае отсутствия
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск загруженного эффекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectPtr FindEffect (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение прокси
///////////////////////////////////////////////////////////////////////////////////////////////////
    BlendStateProxy GetBlendStateProxy (const char* name); //создание прокси в случае отсутствия
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка эффекта по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    void      SetDefaultEffect (const EffectPtr& effect);
    EffectPtr DefaultEffect    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Применение настроек / получение конфигурации ренедринга эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              ChangeSettings (const common::PropertyMap& settings);
    common::ParseNode Configuration  ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
