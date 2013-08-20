#ifndef RENDER_SCENE_INTERCHANGE_RENDER_THREAD_HEADER
#define RENDER_SCENE_INTERCHANGE_RENDER_THREAD_HEADER

#include <cstddef>

#include <render/scene/interchange/connection.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Нить рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderThread
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderThread  (const char* thread_connection_name, size_t render_queue_size, const char* target_connection_name, const char* target_connection_init_string = "");
    RenderThread  (const char* thread_connection_name, size_t render_queue_size, IConnection* connection);
    RenderThread  (const RenderThread&);
    ~RenderThread ();

    RenderThread& operator = (const RenderThread&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ConnectionName () const;

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
