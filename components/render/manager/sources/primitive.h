///forward declarations
class PrimitiveBuffersImpl;
class SimplePrimitiveListImplBase;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Меш
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveImpl: public Object, public CacheSource
{
  public:
    typedef xtl::intrusive_ptr<PrimitiveBuffersImpl> BuffersPtr;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveImpl  (const DeviceManagerPtr&, const MaterialManagerPtr&, const BuffersPtr& buffers, const char* name = "");
    ~PrimitiveImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    ();
    void        SetName (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буферы примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
    const BuffersPtr& Buffers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с мешами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     ();
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со спрайтами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t        SpriteListsCount        ();
    SpriteListPtr AddStandaloneSpriteList (SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
    SpriteListPtr AddBatchingSpriteList   (SpriteMode mode, const math::vec3f& up);
    void          RemoveSpriteList        (const SpriteListPtr&);
    void          RemoveAllSpriteLists    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с линиями
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      LineListsCount        ();
    LineListPtr AddStandaloneLineList (MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
    LineListPtr AddBatchingLineList   ();
    void        RemoveLineList        (const LineListPtr&);
    void        RemoveAllLineLists    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Группы примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  RendererPrimitiveGroupsCount ();
    RendererPrimitiveGroup* RendererPrimitiveGroups      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FillDynamicPrimitiveStorage (DynamicPrimitiveEntityStorage&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    using CacheSource::UpdateCache;
    using CacheSource::ResetCache;

  private:
    void AddSimplePrimitiveList        (SimplePrimitiveListImplBase*, int type);
    void RemoveSimplePrimitiveList     (SimplePrimitiveListImplBase*);
    void RemoveAllSimplePrimitiveLists (int type);
      
  private:
    void UpdateCacheCore ();
    void ResetCacheCore ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
