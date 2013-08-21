#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <render/scene_render_server.h>

#include <render/scene/interchange/connection_manager.h>
#include <render/scene/interchange/context.h>
#include <render/scene/interchange/render_thread.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Объект принимающий входящие подключения рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConnectionAcceptor
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConnectionAcceptor  (const char* name);
    ~ConnectionAcceptor ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}
