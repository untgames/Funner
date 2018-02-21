#ifndef RENDER_SCENE_INTERCHANGE_SERIALIZER_HEADER
#define RENDER_SCENE_INTERCHANGE_SERIALIZER_HEADER

#include <common/strlib.h>

#include <render/scene/interchange/streams.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы команд (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CommandId
{
  CommandId_FenceRequest,
  CommandId_LoadResource,
  CommandId_UnloadResource,
  CommandId_SetMaxDrawDepth,
  CommandId_CreateTexture,
  CommandId_UpdateTexture,
  CommandId_RemoveTexture,
  CommandId_CloneMaterial,
  CommandId_SetMaterialTexmapImage,
  CommandId_SetMaterialTexmapSampler,
  CommandId_RemoveMaterial,
  CommandId_SetViewportArea,
  CommandId_SetViewportZOrder,
  CommandId_SetViewportActive,
  CommandId_SetViewportName,
  CommandId_SetViewportTechnique,
  CommandId_SetViewportBackground,
  CommandId_SetViewportProperties,
  CommandId_SetViewportScene,
  CommandId_SetViewportCameraWorldMatrix,
  CommandId_SetViewportCameraProjectionMatrix,
  CommandId_SetViewportCameraName,
  CommandId_CreateRenderTarget,
  CommandId_DestroyRenderTarget,
  CommandId_SetRenderTargetActive,
  CommandId_SetRenderTargetScreenArea,
  CommandId_SetRenderTargetBackground,
  CommandId_AttachViewportToRenderTarget,
  CommandId_DetachViewportFromRenderTarget,
  CommandId_UpdateRenderTarget,
  CommandId_UpdatePropertyMap,
  CommandId_RemovePropertyMap,
  CommandId_RemovePropertyLayout,
  CommandId_CreateScene,
  CommandId_DestroyScene,
  CommandId_SetSceneName,
  CommandId_SetSceneNodes,
  CommandId_CreateNode,
  CommandId_DestroyNode,
  CommandId_SetNodeName,
  CommandId_SetNodeWorldMatrix,
  CommandId_SetNodeScene,
  CommandId_SetEntityBounds,
  CommandId_SetEntityVisibility,
  CommandId_SetVisualModelScissor,
  CommandId_SetVisualModelDynamicShaderProperties,
  CommandId_SetVisualModelStaticShaderProperties,
  CommandId_UpdateDynamicMesh,
  CommandId_SetStaticMeshName,
  CommandId_SetLightParams,
  CommandId_SetPageCurlParams,
  CommandId_SetParticleEmitterSystemId,
  CommandId_UpdateParticleEmitterSystem,
  CommandId_ReserveSpriteLists,
  CommandId_CreateSpriteList,
  CommandId_RemoveSpriteList,
  CommandId_SetSpriteListMaterial,
  CommandId_SetSpriteListBuffer,
  CommandId_SetSpriteListDescs,
  CommandId_SetLineListParams,
  CommandId_SetLineListMaterial,
  CommandId_SetLineListBuffer,
  CommandId_SetLineListDescs,
  CommandId_FenceResponse,
  CommandId_FirstUserDefined = 10000,
};

///имена команд
stl::string get_command_name (CommandId command_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClientToServerSerializer: public OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FenceRequest(object_id_t tag);
    void LoadResource(const char* name);
    void UnloadResource(const char* name);
    void SetMaxDrawDepth(uint32 depth);
    void CreateTexture(const char* texture_name, media::Image image, TextureDimension dimension, bool8 create_mipmaps);
    void UpdateTexture(const char* texture_name, media::Image image);
    void RemoveTexture(const char* texture_name);
    void CloneMaterial(const char* material_name, const char* prototype_name);
    void SetMaterialTexmapImage(const char* material_name, const char* semantic, const char* image_name);
    void SetMaterialTexmapSampler(const char* material_name, const char* semantic, const char* sampler);
    void RemoveMaterial(const char* material_name);
    void SetViewportArea(object_id_t id, int32 left, int32 top, int32 width, int32 height, float32 min_depth, float32 max_depth);
    void SetViewportZOrder(object_id_t id, int32 zorder);
    void SetViewportActive(object_id_t id, bool8 is_active);
    void SetViewportName(object_id_t id, const char* name);
    void SetViewportTechnique(object_id_t id, const char* name);
    void SetViewportBackground(object_id_t id, bool8 state, const math::vec4f& color);
    void SetViewportProperties(object_id_t id, uint64 properties_id);
    void SetViewportScene(object_id_t id, object_id_t scene_id);
    void SetViewportCameraWorldMatrix(object_id_t id, const math::mat4f& tm);
    void SetViewportCameraProjectionMatrix(object_id_t id, const math::mat4f& tm);
    void SetViewportCameraName(object_id_t id, const char* name);
    void CreateRenderTarget(object_id_t id, const char* name, const char* init_string);
    void DestroyRenderTarget(object_id_t id);
    void SetRenderTargetActive(object_id_t id, bool8 active);
    void SetRenderTargetScreenArea(object_id_t id, int32 left, int32 top, int32 width, int32 height);
    void SetRenderTargetBackground(object_id_t id, bool8 state, const math::vec4f& color);
    void AttachViewportToRenderTarget(object_id_t id, object_id_t viewport_id);
    void DetachViewportFromRenderTarget(object_id_t id, object_id_t viewport_id);
    void UpdateRenderTarget(object_id_t id);
    OutputStream& UpdatePropertyMap();
    void RemovePropertyMap(object_id_t id);
    void RemovePropertyLayout(object_id_t id);
    void CreateScene(object_id_t id);
    void DestroyScene(object_id_t id);
    void SetSceneName(object_id_t id, const char* name);
    void SetSceneNodes(object_id_t id, RawArray<object_id_t> nodes);
    void CreateNode(object_id_t id, NodeType type);
    void DestroyNode(object_id_t id);
    void SetNodeName(object_id_t id, const char* name);
    void SetNodeWorldMatrix(object_id_t id, const math::mat4f& tm);
    void SetNodeScene(object_id_t id, object_id_t scene_id);
    void SetEntityBounds(object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box);
    void SetEntityVisibility(object_id_t id, bool state);
    void SetVisualModelScissor(object_id_t id, object_id_t scissor_id);
    void SetVisualModelDynamicShaderProperties(object_id_t id, object_id_t properties_id);
    void SetVisualModelStaticShaderProperties(object_id_t id, object_id_t properties_id);
    void UpdateDynamicMesh(object_id_t id, media::geometry::Mesh mesh);
    void SetStaticMeshName(object_id_t id, const char* mesh_name);
    void SetLightParams(object_id_t id, const LightParams& params);
    void SetPageCurlParams(object_id_t id, const PageCurlParams& params, const char* front_left_material, const char* front_right_material, const char* back_left_material, const char* back_right_material);
    void SetParticleEmitterSystemId(object_id_t id, const char* system_id, SpriteMode sprite_mode);
    void UpdateParticleEmitterSystem(object_id_t id, uint32 new_time, uint64 properties_id);
    void ReserveSpriteLists(object_id_t id, uint32 list_subids_count);
    void CreateSpriteList(object_id_t id, uint32 list_subid, SpriteMode mode, PrimitiveUsage usage, const math::vec3f& up, const char* batch);
    void RemoveSpriteList(object_id_t id, uint32 list_subid);
    void SetSpriteListMaterial(object_id_t id, uint32 list_subid, const char* material);
    void SetSpriteListBuffer(object_id_t id, uint32 list_subid, uint32 count, uint32 reserve_count);
    void SetSpriteListDescs(object_id_t id, uint32 list_subid, uint32 first, RawArray<SpriteDesc> descs);
    void SetLineListParams(object_id_t id, PrimitiveUsage usage, const char* batch);
    void SetLineListMaterial(object_id_t id, const char* material);
    void SetLineListBuffer(object_id_t id, uint32 count, uint32 reserve_count);
    void SetLineListDescs(object_id_t id, uint32 first, RawArray<LineDesc> descs);

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicDeserializer
{
  public:
    typedef interchange::CommandId CommandId;

    virtual ~BasicDeserializer () {}

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик неизвестной команды
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool DeserializeUnknownCommand (CommandId id, InputStream& stream) { return false; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClientToServerDeserializer: public BasicDeserializer, protected InputStream
{
  public:
    using InputStream::Available;
    using InputStream::Position;
    using InputStream::Skip;
    using InputStream::SetPosition;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчер десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Dispatcher> bool Deserialize (CommandId id, Dispatcher& dispatcher);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализатор команд от сервера к клиенту
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerToClientSerializer: public OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////

    void FenceResponse(object_id_t tag);
    OutputStream& UpdatePropertyMap();
    void RemovePropertyMap(object_id_t id);
    void RemovePropertyLayout(object_id_t id);

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от сервера к клиенту
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerToClientDeserializer: public BasicDeserializer, protected InputStream
{
  public:
    using InputStream::Available;
    using InputStream::Position;
    using InputStream::Skip;
    using InputStream::SetPosition;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчер десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Dispatcher> bool Deserialize (CommandId id, Dispatcher& dispatcher);
};

#include <render/scene/interchange/detail/serializer.inl>

}

}

}

#endif
