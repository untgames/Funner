#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор / деструктор
*/

Render::Render (mid_level::IRenderer* in_renderer)
{
  static const char* METHOD_NAME = "render::render2d::Render::Render";

  if (!in_renderer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer");

  IRenderer* casted_renderer = dynamic_cast<IRenderer*> (in_renderer);

  if (!casted_renderer)
    throw xtl::make_argument_exception (METHOD_NAME, "renderer", typeid (in_renderer).name (),
      "Renderer type is incompatible with render::renderer2d::IRenderer");

  renderer = casted_renderer;
}

Render::~Render ()
{
}

/*
    Создание областей вывода
*/

IRenderView* Render::CreateRenderView (scene_graph::Scene* scene)
{
  return new RenderView (scene, this);
}

/*
    Установка цвета очистка буфера кадра
*/

void Render::SetBackgroundColor (const math::vec4f& color)
{
  clear_color = color;
}

void Render::GetBackgroundColor (math::vec4f& color)
{
  color = clear_color;
}

/*
    Установка функции отладочного протоколирования
*/

void Render::SetLogHandler (const LogFunction& log)
{
  log_handler = log;
}

const Render::LogFunction& Render::GetLogHandler ()
{
  return log_handler;
}

/*
    Подсчёт ссылок
*/

void Render::AddRef ()
{
  addref (this);
}

void Render::Release ()
{
  release (this);
}

/*
    Работа с кэшем
*/

Renderable* Render::GetRenderable (scene_graph::Sprite* entity)
{
    //попытка найти объект в кэше

  RenderableMap::iterator iter = renderables_cache.find (entity);

  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //создание нового спрайта

  RenderablePtr renderable (new RenderableSprite (entity, *renderer), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

void Render::InsertRenderable (scene_graph::Entity* entity, const RenderablePtr& renderable)
{  
  renderables_cache.insert_pair (entity, RenderableHolder (renderable,
    entity->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Render::RemoveRenderable, this, entity))));
}

void Render::RemoveRenderable (scene_graph::Entity* entity)
{
  renderables_cache.erase (entity);
}

/*
    Добавление кадра на отрисовку    
*/

void Render::AddFrame (IFrame* frame)
{
  renderer->AddFrame (frame);
}
