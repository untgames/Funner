#include "shared.h"

using namespace render;

/*
    Конструктор
*/

RenderContext::RenderContext
 (const Viewport&       in_viewport,
  mid_level::IRenderer& in_renderer,
  const Rect&           in_window_rect,
  bool                  in_need_clear,
  const math::vec4f&    in_clear_color)
    : viewport (&in_viewport),
      renderer (&in_renderer),
      need_clear (in_need_clear),
      clear_color (&in_clear_color),
      window_rect (in_window_rect)
{
  const Rect& viewport_rect = viewport->Area ();

  window_rect.left = viewport_rect.left - window_rect.left;
  window_rect.top  = viewport_rect.top  - window_rect.top;
}

/*
    Система рендеринга
*/

mid_level::IRenderer& RenderContext::Renderer ()
{
  return *renderer;
}

/*
    Имя пути рендеринга
*/

const char* RenderContext::RenderPath ()
{
  return viewport->RenderPath ();
}

/*
    Камера
*/

const scene_graph::Camera* RenderContext::Camera ()
{
  return viewport->Camera ();
}

/*
    Область вывода
*/

Rect RenderContext::TransformViewport (const Rect& client_rect)
{
  float kx = float (client_rect.width) / window_rect.width,
        ky = float (client_rect.height) / window_rect.height;
        
  const Rect& viewport_rect = viewport->Area ();

  return Rect (int (client_rect.left + window_rect.left * kx), int (client_rect.top + window_rect.top * ky),
               size_t (viewport_rect.width * kx), size_t (viewport_rect.height * ky));
}

/*
    Параметры очистки области вывода
*/

//нужно ли очищать область вывода
bool RenderContext::NeedClearViewport ()
{
  return need_clear;
}

//цвет очистки
const math::vec4f& RenderContext::ClearColor ()
{
  return *clear_color;
}

/*
    Чтение свойств
*/

const char* RenderContext::GetProperty (const char* name)
{
  return viewport->GetProperty (name);
}

bool RenderContext::HasProperty (const char* name)
{
  return viewport->HasProperty (name);
}
