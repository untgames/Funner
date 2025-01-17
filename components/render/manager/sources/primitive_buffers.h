///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveBuffersImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveBuffersImpl  (const DeviceManagerPtr&);
    ~PrimitiveBuffersImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшем
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCacheState (bool state);
    bool CacheState    ();
    void EnableCache   () { SetCacheState (true); }
    void DisableCache  () { SetCacheState (false); }
    void FlushCache    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание отображений буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelBufferPtr CreateVertexStream (const media::geometry::VertexStream&, MeshBufferUsage usage, bool updatable);
    VertexBufferPtr   CreateVertexBuffer (const media::geometry::VertexBuffer&, MeshBufferUsage usage, bool updatable);
    LowLevelBufferPtr CreateIndexBuffer  (const media::geometry::IndexBuffer&, MeshBufferUsage usage, bool updatable);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);    
    void Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void Update             (const media::geometry::VertexStream& buffer);
    void UpdateVertexStream (media::geometry::object_id_t id, const void* buffer, size_t buffer_size);
    void Update             (const media::geometry::IndexBuffer& buffer);
    void UpdateIndexBuffer  (media::geometry::object_id_t id, const void* buffer, size_t buffer_size);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////        
    void Remove                 (const media::geometry::VertexStream& buffer);
    void Remove                 (const media::geometry::VertexBuffer& buffer);
    void Remove                 (const media::geometry::IndexBuffer& buffer);
    void RemoveAllVertexStreams ();
    void RemoveAllVertexBuffers ();
    void RemoveAllIndexBuffers  ();
    void RemoveAll              ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер пакетирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::BatchingManager& BatchingManager    ();
    bool                      HasBatchingManager ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
