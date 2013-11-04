#ifndef RENDER_SCENE_SERVER_RESOURCE_MANAGER_SHARED_HEADER
#define RENDER_SCENE_SERVER_RESOURCE_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <xtl/rect.h>

#include <render/manager/common.h>

#include <render/scene/interchange/types.h>

namespace render
{

namespace scene
{

namespace server
{

/// implementation forwards
struct ResourceLibrary;
class  RenderManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceClient
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceClient  (ResourceLibrary*);
    ResourceClient  (const ResourceClient&);
    ~ResourceClient ();

    ResourceClient& operator = (const ResourceClient&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* name);
    void UnloadResource (const char* name);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceManager  (const RenderManager& render_manager);
    ResourceManager  (const ResourceManager&);
    ~ResourceManager ();

    ResourceManager& operator = (const ResourceManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceClient CreateClient ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
