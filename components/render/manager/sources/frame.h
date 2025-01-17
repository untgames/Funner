///////////////////////////////////////////////////////////////////////////////////////////////////
///Визуализируемый кадр
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameImpl  (const DeviceManagerPtr&, const EffectManagerPtr&, const PropertyCachePtr&);
    ~FrameImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Карта целей рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetRenderTargets (RenderTargetMapImpl&);
    RenderTargetMapImpl& RenderTargets    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление отсечением
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetScissorState (bool state);
    bool ScissorState    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги очистки кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetClearFlags (size_t clear_flags);
    size_t ClearFlags    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры очистки буфера цвета
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetClearColor (const math::vec4f& color);
    const math::vec4f& ClearColor    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры очистки буфера попиксельного отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetClearDepthValue   (float depth_value);
    void          SetClearStencilIndex (unsigned char stencil_index);
    float         ClearDepthValue      ();
    unsigned char ClearStencilIndex    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Локальные текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLocalTexture        (const char* name, const TexturePtr& texture);
    void RemoveLocalTexture     (const char* name);
    void RemoveAllLocalTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение локальной текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    TexturePtr FindLocalTexture (const char* name);
    TexturePtr LocalTexture     (const char* name);
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка эффекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetEffect (const char* name);
    const char* Effect    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рендер эффекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectRendererPtr EffectRenderer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Свойства рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение буфера свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    const LowLevelBufferPtr& DevicePropertyBuffer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Макро-определения шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                                 SetShaderOptions   (const common::PropertyMap&);
    const common::PropertyMap&           ShaderOptions      ();
    render::manager::ShaderOptionsCache& ShaderOptionsCache ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t EntitiesCount     ();
    void   AddEntity         (const EntityPtr& entity, void* user_data);
    void   RemoveAllEntities ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление пре-рендеринга и пост-рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FramesCount     ();
    void   AddFrame        (const FramePtr& frame);
    void   RemoveAllFrames ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Автоматическая очистка кадра после отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAutoCleanup       (bool state);
    bool IsAutoCleanupEnabled ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пользовательский обработчик отрисовки объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Frame::EntityDrawFunction EntityDrawFunction;    

    void                      SetEntityDrawHandler (const EntityDrawFunction& handler);
    const EntityDrawFunction& EntityDrawHandler    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка свойств пары frame-entity
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetEntityDependentProperties (const common::PropertyMap&);
    const common::PropertyMap& EntityDependentProperties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка матрицы vipew * projection
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetViewProjectionMatrix (const math::mat4f&);
    const math::mat4f& ViewProjectionMatrix    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Prerender (EntityDrawFunction handler); //без ссылки: на случай попытки обновления обработчика во время обхода
    void Draw      (RenderingContext* previous = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
  private:
    void Cleanup ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
