///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialManager  (const DeviceManagerPtr&, const TextureManagerPtr&, const ProgramManagerPtr&, const EffectManagerPtr&);
    ~MaterialManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialPtr CreateMaterial ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: является ли ресурс библиотекой материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsMaterialLibraryResource (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadMaterialLibrary   (const char* name);
    void LoadMaterialLibrary   (const media::rfx::MaterialLibrary&);
    void UnloadMaterialLibrary (const char* name);
    void UnloadMaterialLibrary (const media::rfx::MaterialLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация совместно используемых материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShareMaterial   (const char* name, const MaterialPtr& material);
    void UnshareMaterial (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение прокси
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialProxy GetMaterialProxy (const char* name); //создание прокси в случае отсутствия
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск загруженного материала
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialPtr FindMaterial (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка материала по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetDefaultMaterial (const MaterialPtr& material);
    MaterialPtr DefaultMaterial    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ссылка на менеджер устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::DeviceManager& DeviceManager ();
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
