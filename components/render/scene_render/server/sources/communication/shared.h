#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <common/property_map.h>
#include <common/strlib.h>

#include <syslib/semaphore.h>
#include <syslib/thread.h>

#include <render/scene_render_server.h>

#include <render/scene/interchange/connection_manager.h>
#include <render/scene/interchange/context.h>
#include <render/scene/interchange/property_synchronizer.h>
#include <render/scene/interchange/render_thread.h>
#include <render/scene/interchange/serializer.h>

#include <shared/server.h>

namespace render
{

namespace scene
{

namespace server
{

using interchange::uint64;
using interchange::int64;
using interchange::uint32;
using interchange::int32;
using interchange::uint16;
using interchange::int16;
using interchange::uint8;
using interchange::int8;
using interchange::bool8;
using interchange::float32;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренние команды сервера
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InternalCommandId
{
  InternalCommandId_OnWindowAttached = interchange::CommandId_FirstUserDefined,
  InternalCommandId_OnWindowDetached,
  InternalCommandId_OnWindowSizeChanged,
  InternalCommandId_OnWindowViewportChanged,
  InternalCommandId_OnWindowHandleChanged,
  InternalCommandId_OnWindowPaint,
};

typedef interchange::Context<interchange::ServerToClientSerializer, interchange::ClientToServerDeserializer> Context;
typedef interchange::PropertyMapAutoWriter::Synchronizer                                                     PropertyMapSynchronizer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Объект принимающий входящие подключения рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConnectionAcceptor
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConnectionAcceptor  (const char* name, ServerThreadingModel model);
    ~ConnectionAcceptor ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание незавершенных команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool WaitQueuedCommands (size_t timeout_ms);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Соединение с клиентом
///////////////////////////////////////////////////////////////////////////////////////////////////
class Connection: public interchange::IConnection, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Connection  (ServerImpl& server, const char* init_string); //must be thread neutral
    ~Connection ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка входного потока данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessCommands (const interchange::CommandBuffer& commands);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение события оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчет ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исходящее соединение от сервера к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerLoopbackConnection: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ServerLoopbackConnection  (const char* name);
    ~ServerLoopbackConnection ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сообщения серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnWindowAttached        (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom);
    void OnWindowDetached        (size_t id);
    void OnWindowSizeChanged     (size_t id, size_t width, size_t height);
    void OnWindowViewportChanged (size_t id, size_t left, size_t top, size_t right, size_t bottom);
    void OnWindowHandleChanged   (size_t id, void* handle);
    void OnWindowPaint           (size_t id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка входящей команды
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool ProcessIncomingCommand (InternalCommandId id, interchange::InputStream&, ConnectionState& state);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренний менеджер окон
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClientWindowManager: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClientWindowManager  (ServerLoopbackConnection&);
    ~ClientWindowManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение окон
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachWindow     (const char* name, syslib::Window& window, const common::PropertyMap& properties);
    void DetachWindow     (const char* name);
    void DetachAllWindows ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}
