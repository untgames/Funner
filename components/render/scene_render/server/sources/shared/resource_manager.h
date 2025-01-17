#ifndef RENDER_SCENE_SERVER_RESOURCE_MANAGER_SHARED_HEADER
#define RENDER_SCENE_SERVER_RESOURCE_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <xtl/rect.h>

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
///Клиент менеджера ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceClient
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceClient  (ResourceLibrary*);
    ResourceClient  (const ResourceClient&);
    ~ResourceClient ();

    ResourceClient& operator = (const ResourceClient&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* name);
    void UnloadResource (const char* name);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    ResourceManager  (const RenderManager& render_manager);
    ResourceManager  (const ResourceManager&);
    ~ResourceManager ();

    ResourceManager& operator = (const ResourceManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание клиента
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
