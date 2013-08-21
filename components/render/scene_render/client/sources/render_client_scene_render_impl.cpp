#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

struct SceneRenderImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

SceneRenderImpl::SceneRenderImpl ()
  : impl (new Impl)
{
}

SceneRenderImpl::~SceneRenderImpl ()
{
}

/*
    Обработчики ответов сервера
*/

void SceneRenderImpl::Dummy ()
{
}
