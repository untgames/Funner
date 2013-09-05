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
  CommandId_CreateViewport,
  CommandId_SetViewportArea,
  CommandId_SetViewportZOrder,
  CommandId_SetViewportActive,
  CommandId_SetViewportName,
  CommandId_SetViewportTechnique,
  CommandId_SetViewportBackground,
  CommandId_DestroyViewport,
  CommandId_CreateRenderTarget,
  CommandId_DestroyRenderTarget,
  CommandId_SetRenderTargetActive,
  CommandId_SetRenderTargetScreenArea,
  CommandId_SetRenderTargetBackground,
  CommandId_AttachViewportToRenderTarget,
  CommandId_DetachViewportFromRenderTarget,
  CommandId_UpdateRenderTarget,
  CommandId_Dummy,
  CommandId_PropertyLayout,
  CommandId_PropertyMap,
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
    void CreateViewport(uint32 id);
    void SetViewportArea(uint32 id, int8 left, int8 top, int8 width, int8 height);
    void SetViewportZOrder(uint32 id, int8 zorder);
    void SetViewportActive(uint32 id, bool8 is_active);
    void SetViewportName(uint32 id, const char* name);
    void SetViewportTechnique(uint32 id, const char* name);
    void SetViewportBackground(uint32 id, bool8 state, const math::vec4f& color);
    void DestroyViewport(uint32 id);
    void CreateRenderTarget(uint32 id, const char* name);
    void DestroyRenderTarget(uint32 id);
    void SetRenderTargetActive(uint32 id, bool8 active);
    void SetRenderTargetScreenArea(uint32 id, int8 left, int8 top, int8 width, int8 height);
    void SetRenderTargetBackground(uint32 id, bool8 state, const math::vec4f& color);
    void AttachViewportToRenderTarget(uint32 render_target_id, uint32 viewport_id);
    void DetachViewportFromRenderTarget(uint32 render_target_id, uint32 viewport_id);
    void UpdateRenderTarget(uint32 id);

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
    virtual bool DeserializeUnknownCommand (CommandId id) { return false; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClientToServerDeserializer: public BasicDeserializer, public InputStream
{
  public:
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

    void Dummy();

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от сервера к клиенту
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerToClientDeserializer: public BasicDeserializer, public InputStream
{
  public:
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
