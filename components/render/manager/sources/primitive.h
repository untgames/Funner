///forward declarations
class PrimitiveBuffersImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание примитива рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitive
{
  MaterialImpl*                    material;    //материал
  render::low_level::IStateBlock*  state_block; //блок состояний примитива
  bool                             indexed;     //является ли данный примитив индексированным или состоящим только из вершин без индексов
  render::low_level::PrimitiveType type;        //тип примитива
  size_t                           first;       //индекс первой вершины/индекса
  size_t                           count;       //количество примитивов
  size_t                           tags_count;  //количество тэгов материала
  const size_t*                    tags;        //тэги материала
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание группы примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitiveGroup
{
  size_t                   primitives_count; //количество примитивов в группе
  const RendererPrimitive* primitives;       //примитивы
};

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
    PrimitiveImpl  (const DeviceManagerPtr&, const MaterialManagerPtr&, const BuffersPtr& buffers);
    ~PrimitiveImpl ();
    
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
///Работа с линиями
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LinesCount       ();
    size_t AddLines         (size_t lines_count, const Line* lines, const MaterialPtr& material);
    void   UpdateLines      (size_t first_lines, size_t lines_count, const Line* lines);
    void   SetLinesMaterial (size_t first_lines, size_t lines_count, const MaterialPtr& material);
    void   RemoveLines      (size_t first_lines, size_t lines_count);
    void   RemoveAllLines   ();
    void   ReserveLines     (size_t lines_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со спрайтами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SpritesCount       ();
    size_t AddSprites         (size_t sprites_count, const Sprite* sprites, const MaterialPtr& material);
    void   UpdateSprites      (size_t first_sprite, size_t sprites_count, const Sprite* sprites);
    void   SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const MaterialPtr& material);
    void   RemoveSprites      (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites   ();
    void   ReserveSprites     (size_t sprites_count);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Группы примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  RendererPrimitiveGroupsCount ();
    RendererPrimitiveGroup* RendererPrimitiveGroups      ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
  private:
    void UpdateCacheCore ();
    void ResetCacheCore  ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
