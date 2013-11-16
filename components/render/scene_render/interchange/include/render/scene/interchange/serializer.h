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
  CommandId_LoadResource,
  CommandId_UnloadResource,
  CommandId_SetMaxDrawDepth,
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
  CommandId_CreateNode,
  CommandId_DestroyNode,
  CommandId_SetNodeName,
  CommandId_SetNodeWorldMatrix,
  CommandId_SetEntityBounds,
  CommandId_SetVisualModelScissor,
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
    void LoadResource(const char* name);
    void UnloadResource(const char* name);
    void SetMaxDrawDepth(uint32 depth);
    void SetViewportArea(object_id_t id, int32 left, int32 top, int32 width, int32 height);
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
    void CreateNode(object_id_t id, NodeType type);
    void DestroyNode(object_id_t id);
    void SetNodeName(object_id_t id, const char* name);
    void SetNodeWorldMatrix(object_id_t id, const math::mat4f& tm);
    void SetEntityBounds(object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box);
    void SetVisualModelScissor(object_id_t id, object_id_t scissor_id);

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicDeserializer
{
  public:
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
