#include "shared.h"

using namespace render::scene::interchange;

/*
    Описание реализации сервера рендеринга сцены
*/

struct SceneRenderServer::Impl
{
};

/*
    Конструктор / деструктор
*/

SceneRenderServer::SceneRenderServer (const char* name)
{
}

SceneRenderServer::~SceneRenderServer ()
{
}

/*
    Присоединение окон
*/

void SceneRenderServer::AttachWindow (const char* name, syslib::Window& window, const char* init_string)
{
}

void SceneRenderServer::DetachWindow (const char* name)
{
}

void SceneRenderServer::DetachAllWindows ()
{
}
