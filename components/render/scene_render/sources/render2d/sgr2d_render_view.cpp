#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

namespace
{

/*
    Посетитель объектов сцены
*/

struct RenderViewVisitor: public xtl::visitor<void, SpriteModel, TextLine>
{
  typedef xtl::intrusive_ptr<Render> RenderPtr;

  FramePtr  frame;  //кадр отрисовки
  RenderPtr render; //рендер 

  RenderViewVisitor (const FramePtr& in_frame, const RenderPtr& in_render) : frame (in_frame), render (in_render) {}

  void visit (SpriteModel& model)
  {
    Renderable* renderable = render->GetRenderable (&model);
    
    if (renderable)
      renderable->Draw (*frame);
  }

  void visit (TextLine& text_line)
  {
    Renderable* renderable = render->GetRenderable (&text_line);
    
    if (renderable)
      renderable->Draw (*frame);
  }
};

}

/*
    Конструктор / деструктор
*/

RenderView::RenderView (Scene* in_scene, Render* in_render)
  : render (in_render),
    scene (in_scene),
    camera (0)
{
  try
  {
    if (!scene)
      throw xtl::make_null_argument_exception ("", "scene");
      
    if (!render)
      throw xtl::make_null_argument_exception ("", "render");
      
      //создание кадра
      
    IRenderer& renderer = *render->Renderer ();
    
    frame = FramePtr (renderer.CreateFrame (), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::RenderView");
    throw;
  }    
}

RenderView::~RenderView ()
{
}

/*
    Целевые буферы рендеринга
*/

void RenderView::SetRenderTargets (mid_level::IRenderTarget* render_target, mid_level::IRenderTarget* depth_stencil_target)
{
  frame->SetRenderTargets (render_target, depth_stencil_target);
}

mid_level::IRenderTarget* RenderView::GetRenderTarget ()
{
  return frame->GetRenderTarget ();  
}

mid_level::IRenderTarget* RenderView::GetDepthStencilTarget ()
{
  return frame->GetDepthStencilTarget ();
}

/*
    Установка области вывода
*/

void RenderView::SetViewport (const Rect& rect)
{
  try
  {
    render::mid_level::Viewport viewport;
    
    viewport.x      = rect.left;
    viewport.y      = rect.top;
    viewport.width  = rect.width;
    viewport.height = rect.height;
    
    frame->SetViewport (viewport);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::SetViewport");
    throw;
  }
}

void RenderView::GetViewport (Rect& out_rect)
{
  try
  {
    render::mid_level::Viewport viewport;
    
    frame->GetViewport (viewport);
    
    out_rect.left   = viewport.x;
    out_rect.top    = viewport.y;
    out_rect.width  = viewport.width;
    out_rect.height = viewport.height;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::RenderView::GetViewport");
    throw;
  }
}

/*
    Установка камеры
*/

void RenderView::SetCamera (scene_graph::Camera* in_camera)
{
  camera = in_camera;
}

scene_graph::Camera* RenderView::GetCamera ()
{
  return camera;
}

/*
    Установка / чтение свойств
*/

void RenderView::SetProperty (const char*, const char*)
{
}

void RenderView::GetProperty (const char*, size_t buffer_size, char* value_buffer)
{
  if (buffer_size && value_buffer)
    *value_buffer = 0;
}

/*
    Рисование
*/

void RenderView::Draw ()
{
  if (!camera)
    return;

    //очистка кадра

  frame->Clear ();

    //установка матриц вида и проекции

  frame->SetProjection (camera->ProjectionMatrix ());
  frame->SetViewPoint  (camera->WorldPosition ());

    //обход сцены

  RenderViewVisitor visitor (frame, render);

  scene->VisitEach (visitor);

    //добавление кадра на отрисовку

  render->Renderer ()->AddFrame (frame.get ());
}

/*
    Подсчёт ссылок
*/

void RenderView::AddRef ()
{
  addref (this);
}

void RenderView::Release ()
{
  release (this);
}
