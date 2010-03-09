///////////////////////////////////////////////////////////////////////////////////////////////////
///Меш
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveImpl  ();
    ~PrimitiveImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с мешами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     ();
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
    void   UpdateMesh      (size_t mesh_index, const media::geometry::Mesh& mesh);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с динамическими буферами меша
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddMeshBuffer        (const media::geometry::VertexStream& buffer, MeshBufferUsage usage);
    void AddMeshBuffer        (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage);
    void AddMeshBuffer        (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage);
    void UpdateMeshBuffer     (const media::geometry::VertexStream& buffer);
    void UpdateMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void UpdateMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexStream& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveAllMeshBuffers ();
    
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
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
