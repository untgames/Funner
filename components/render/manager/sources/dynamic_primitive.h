//implementation forwards
class RenderingContext;
class DynamicPrimitiveEntityStorage;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги создания динамического примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
enum DynamicPrimitiveFlag
{
  DynamicPrimitiveFlag_FrameDependent  = 1, //динамический примитив зависит от кадра
  DynamicPrimitiveFlag_EntityDependent = 2, //динамический примитив зависит от сущности

  DynamicPrimitiveFlag_Default = DynamicPrimitiveFlag_EntityDependent
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамический примитив
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitive: public Object, public CacheSource, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~DynamicPrimitive () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateOnPrerender (FrameId frame_id, EntityImpl& entity);
    void UpdateOnRender    (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Группа примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    const manager::RendererPrimitiveGroup* RendererPrimitiveGroup ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Является ли примитив зависимым от кадра / является ли примитив зависимым от EntityDrawParams
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFrameDependent  ();
    bool IsEntityDependent ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    using CacheSource::UpdateCache;
    using CacheSource::ResetCache;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitive (const manager::RendererPrimitiveGroup* group, size_t flags = DynamicPrimitiveFlag_Default);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateOnPrerenderCore (EntityImpl& entity) = 0;
    virtual void UpdateOnRenderCore    (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix) = 0;

  private:
    const manager::RendererPrimitiveGroup* group;
    bool                                   frame_dependent;
    bool                                   entity_dependent;
    FrameId                                cached_frame_id;
    EntityImpl*                            cached_entity;
    FrameImpl*                             cached_frame;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Хранилище динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitiveEntityStorage: public CacheSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitiveEntityStorage  (EntityImpl& entity);
    ~DynamicPrimitiveEntityStorage ();
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddPrimitive        (const DynamicPrimitivePtr&, void* source_tag = 0);
    void RemovePrimitive     (const DynamicPrimitivePtr&);
    void RemoveAllPrimitives ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск примитива по тэгу
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitivePtr FindPrimitive (void* source_tag, bool touch);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кэш объектов, необходимых для построения динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> T*   FindCacheValue       (void* source_tag, bool touch);
    template <class T> void SetCacheValue        (const T& value, void* source_tag = 0);
    template <class T> void RemoveCacheValue     (void* source_tag);
                       void RemoveAllCacheValues ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление автоматической сборкой неиспользуемых динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginUpdate ();
    void EndUpdate   ();

    struct UpdateScope: public xtl::noncopyable
    {
      UpdateScope (DynamicPrimitiveEntityStorage& in_storage) : storage (in_storage) { storage.BeginUpdate (); }
      ~UpdateScope () { storage.EndUpdate (); }

      DynamicPrimitiveEntityStorage& storage;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Группы примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                         RendererPrimitiveGroupsCount ();
    RendererDynamicPrimitiveGroup* RendererPrimitiveGroups      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    using CacheSource::UpdateCache;
    using CacheSource::ResetCache;

  private:
    struct CacheValueBase
    {
      const std::type_info* type;

      CacheValueBase (const std::type_info& in_type) : type (&in_type) {}

      virtual ~CacheValueBase () {}
    };

    template <class T> struct CacheValue: public CacheValueBase
    {
      T value;

      CacheValue (const T& in_value) : CacheValueBase (typeid (T)), value (in_value) {}
    };

  private:
    void UpdateCacheCore ();
    void ResetCacheCore ();

    CacheValueBase* FindCacheValueCore   (const std::type_info&, void*, bool);
    void            AddCacheValueCore    (CacheValueBase*, void*);
    void            RemoveCacheValueCore (const std::type_info&, void*);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прототип динамического примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicPrimitivePrototype: public Object, public CacheSource, public xtl::trackable
{
  public:
    virtual ~DynamicPrimitivePrototype () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание экземпляра
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual DynamicPrimitive* CreateDynamicPrimitiveInstance (EntityImpl& entity, DynamicPrimitiveEntityStorage& dynamic_storage) = 0; //entity should not be retained

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление кешей
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void TouchCacheValues (DynamicPrimitiveEntityStorage& storage) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация
///////////////////////////////////////////////////////////////////////////////////////////////////

inline const RendererPrimitiveGroup* DynamicPrimitive::RendererPrimitiveGroup ()
{
  return group;
}

inline bool DynamicPrimitive::IsFrameDependent ()
{
  return frame_dependent;
}

inline bool DynamicPrimitive::IsEntityDependent ()
{
  return entity_dependent;
}

inline void DynamicPrimitive::UpdateOnPrerender (FrameId frame_id, EntityImpl& entity)
{
  if (frame_id == cached_frame_id)
    return;  

  cached_frame_id = frame_id;
  cached_entity   = 0;
  cached_frame    = 0;

  try
  {
    UpdateOnPrerenderCore (entity);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitive::UpdateOnPrerender");
    throw;
  }
}

inline void DynamicPrimitive::UpdateOnRender (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix)
{
  if (&entity == cached_entity && &frame == cached_frame)
    return;

  cached_entity = &entity;
  cached_frame  = &frame;

  try
  {
    UpdateOnRenderCore (frame, entity, context, mvp_matrix);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitive::UpdateOnRender");
    throw;
  }  
}

template <class T>
inline T* DynamicPrimitiveEntityStorage::FindCacheValue (void* source_tag, bool touch)
{
  CacheValueBase* cache_value = FindCacheValueCore (typeid (T), source_tag, touch);

  if (!cache_value)
    return 0;

  return &static_cast<CacheValue<T>*> (cache_value)->value;
}

template <class T>
inline void DynamicPrimitiveEntityStorage::SetCacheValue (const T& value, void* source_tag)
{
  CacheValueBase* cache_value = FindCacheValueCore (typeid (T), source_tag, true);

  if (cache_value)
  {
    static_cast<CacheValue<T>*> (cache_value)->value = value;
    return;
  }

  stl::auto_ptr<CacheValue<T> > new_value (new CacheValue<T> (value));

  AddCacheValueCore (new_value.get (), source_tag);

  new_value.release ();
}
