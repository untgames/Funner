///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConnectionState: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConnectionState  (ServerImpl&);
    ~ConnectionState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapSynchronizer CreateSynchronizer  (const common::PropertyMap&);
    common::PropertyMap     GetClientProperties (uint64 id); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Команды клиента
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource (const char*);
    void UnloadResource (const char*);

    void CreateViewport (uint32 id);
    void SetViewportArea (uint32 id, int8 left, int8 top, int8 width, int8 height);
    void SetViewportZOrder (uint32 id, int8 zorder);
    void SetViewportActive (uint32 id, bool8 is_active);
    void SetViewportName (uint32 id, const char* name);
    void SetViewportTechnique (uint32 id, const char* name);
    void SetViewportBackground (uint32 id, bool8 state, const math::vec4f& color);
    void SetViewportProperties (uint32 id, uint64 properties_id);
    void DestroyViewport (uint32 id);

    void CreateRenderTarget (uint32 id, const char* name);
    void DestroyRenderTarget (uint32 id);
    void SetRenderTargetActive (uint32 id, bool8 active);
    void SetRenderTargetScreenArea (uint32 id, int8 left, int8 top, int8 width, int8 height);
    void SetRenderTargetBackground (uint32 id, bool8 state, const math::vec4f& color);
    void AttachViewportToRenderTarget (uint32 render_target_id, uint32 viewport_id);
    void DetachViewportFromRenderTarget (uint32 render_target_id, uint32 viewport_id);
    void UpdateRenderTarget (uint32 id);

    void UpdatePropertyMap (interchange::InputStream&);
    void RemovePropertyMap (uint64 id);
    void RemovePropertyLayout (uint64 id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренние команды
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnWindowAttached        (size_t window_id, const char* name, const char* init_string, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom);
    void OnWindowDetached        (size_t window_id);
    void OnWindowSizeChanged     (size_t window_id, size_t width, size_t height);
    void OnWindowViewportChanged (size_t window_id, size_t left, size_t top, size_t right, size_t bottom);
    void OnWindowHandleChanged   (size_t window_id, void* handle);
    void OnWindowPaint           (size_t window_id);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
