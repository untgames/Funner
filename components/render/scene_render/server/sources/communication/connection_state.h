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
///Установка контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetContext (Context*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapSynchronizer CreateSynchronizer    (const common::PropertyMap&);
    common::PropertyMap     GetClientProperties   (object_id_t id); 
    void                    SynchronizeProperties ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Команды клиента
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource (const char*);
    void UnloadResource (const char*);

    void SetMaxDrawDepth (uint32 depth);

    void CreateTexture(const char* texture_name, const media::Image& image, interchange::TextureDimension dimension, bool8 create_mips);
    void UpdateTexture(const char* texture_name, const media::Image& image);
    void RemoveTexture(const char* texture_name);

    void CloneMaterial(const char* material_name, const char* prototype_name);
    void SetMaterialTexmapImage(const char* material_name, const char* semantic, const char* image_name);
    void SetMaterialTexmapSampler(const char* material_name, const char* semantic, const char* sampler);
    void RemoveMaterial(const char* material_name);

    void SetViewportArea (object_id_t id, int32 left, int32 top, int32 width, int32 height, float32 min_depth, float32 max_depth);
    void SetViewportZOrder (object_id_t id, int32 zorder);
    void SetViewportActive (object_id_t id, bool8 is_active);
    void SetViewportName (object_id_t id, const char* name);
    void SetViewportTechnique (object_id_t id, const char* name);
    void SetViewportBackground (object_id_t id, bool8 state, const math::vec4f& color);
    void SetViewportProperties (object_id_t id, object_id_t properties_id);
    void SetViewportScene(object_id_t id, object_id_t scene_id);
    void SetViewportCameraWorldMatrix(object_id_t id, const math::mat4f& tm);
    void SetViewportCameraProjectionMatrix(object_id_t id, const math::mat4f& tm);
    void SetViewportCameraName(object_id_t id, const char* name);

    void CreateRenderTarget (object_id_t id, const char* name, const char* init_string);
    void DestroyRenderTarget (object_id_t id);
    void SetRenderTargetActive (object_id_t id, bool8 active);
    void SetRenderTargetScreenArea (object_id_t id, int32 left, int32 top, int32 width, int32 height);
    void SetRenderTargetBackground (object_id_t id, bool8 state, const math::vec4f& color);
    void AttachViewportToRenderTarget (object_id_t render_target_id, object_id_t viewport_id);
    void DetachViewportFromRenderTarget (object_id_t render_target_id, object_id_t viewport_id);
    void UpdateRenderTarget (object_id_t id);

    void UpdatePropertyMap (interchange::InputStream&);
    void RemovePropertyMap (object_id_t id);
    void RemovePropertyLayout (object_id_t id);

    void CreateScene (object_id_t id);
    void DestroyScene (object_id_t id);
    void SetSceneName (object_id_t id, const char* name);
    void SetSceneNodes (object_id_t id, const interchange::RawArray<object_id_t>& nodes);

    void CreateNode (object_id_t id, interchange::NodeType type);
    void DestroyNode (object_id_t id);
    void SetNodeName (object_id_t id, const char* name);
    void SetNodeWorldMatrix (object_id_t id, const math::mat4f& tm);
    void SetNodeScene (object_id_t id, object_id_t scene_id);

    void SetEntityBounds (object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box);
    void SetEntityVisibility (object_id_t id, bool is_visible);

    void SetVisualModelScissor (object_id_t id, object_id_t scissor_id);
    void SetVisualModelDynamicShaderProperties (object_id_t id, object_id_t properties_id);
    void SetVisualModelStaticShaderProperties (object_id_t id, object_id_t properties_id);

    void SetStaticMeshName (object_id_t id, const char* name);

    void SetLightParams (object_id_t id, const interchange::LightParams& params);

    void SetPageCurlParams(object_id_t id, const interchange::PageCurlParams& params, const char* front_left_material, const char* front_right_material, const char* back_left_material, const char* back_right_material);

    void ReserveSpriteLists (object_id_t id, uint32 list_subids_count);
    void CreateSpriteList (object_id_t id, uint32 list_subid, interchange::SpriteMode mode, interchange::PrimitiveUsage usage, const math::vec3f& up, const char* batch);
    void RemoveSpriteList (object_id_t id, uint32 list_subid);
    void SetSpriteListMaterial (object_id_t id, uint32 list_subid, const char* material);
    void SetSpriteListBuffer (object_id_t id, uint32 list_subid, uint32 count, uint32 reserve_count);
    void SetSpriteListDescs (object_id_t id, uint32 list_subid, uint32 first, interchange::RawArray<interchange::SpriteDesc> descs);

    void SetLineListParams (object_id_t id, interchange::PrimitiveUsage usage, const char* batch);
    void SetLineListMaterial (object_id_t id, const char* material);
    void SetLineListBuffer (object_id_t id, uint32 count, uint32 reserve_count);
    void SetLineListDescs (object_id_t id, uint32 first, interchange::RawArray<interchange::LineDesc> descs);

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
