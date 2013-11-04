#ifndef RENDER_SCENE_SERVER_SHARED_HEADER
#define RENDER_SCENE_SERVER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <xtl/trackable.h>

#include <common/log.h>

#include <render/manager.h>

namespace render
{

namespace scene
{

namespace server
{

//forward declarations
class ResourceManager;
class SceneManager;
class ScreenManager;
class WindowManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerImpl: public xtl::noncopyable, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ServerImpl  (const char* name);
    ~ServerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    common::Log& Log ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::RenderManager&  RenderManager   ();
    server::ScreenManager&   ScreenManager   ();
    server::WindowManager&   WindowManager   ();
    server::SceneManager&    SceneManager    ();
    server::ResourceManager& ResourceManager ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
