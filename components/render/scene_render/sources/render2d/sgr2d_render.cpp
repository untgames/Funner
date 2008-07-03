#include "shared.h"

using namespace render;
using namespace render::render2d;

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
  throw xtl::make_not_implemented_exception ("render::render2d::Render::CreateRenderView");
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

#if 0

/*
    Получение примитива соответствующего объекту сцены / удаление примитива из кэша
*/

Render::Primitive* Render::FindPrimitive (scene_graph::Entity* entity)
{
  PrimitiveMap::iterator iter = primitives_cache.find (entity);
  
  if (iter != primitives_cache.end ())
    return iter->second.get ();
    
  return 0;
}

void Render::InsertPrimitive (scene_graph::Sprite* sprite)
{
  
}

void Render::RemovePrimitive (scene_graph::Entity* entity)
{
  primitives_cache.erase (entity);
}

#endif
