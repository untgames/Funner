#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

namespace
{

/*
    ���������� �������� �����
*/

struct RenderViewVisitor: public xtl::visitor<void, SpriteModel>
{
  typedef xtl::intrusive_ptr<Render> RenderPtr;

  FramePtr  frame;  //���� ���������
  RenderPtr render; //������ 

  RenderViewVisitor (const FramePtr& in_frame, const RenderPtr& in_render) : frame (in_frame), render (in_render) {}

  void visit (SpriteModel& model)
  {
    Renderable* renderable = render->GetRenderable (&model);
    
    if (renderable)
      renderable->Draw (*frame);
  }
};

}

/*
    ����������� / ����������
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
      
      //�������� �����
      
    IRenderer& renderer = *render->Renderer ();
    
    frame = FramePtr (renderer.CreateFrame (), false);

    frame->SetRenderTargets  (renderer.GetColorBuffer (), renderer.GetDepthStencilBuffer ());    
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
    ��������� ������� ������
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
    ��������� ������
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
    ��������� / ������ �������
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
    ���������
*/

void RenderView::Draw ()
{
  if (!camera)
    return;

    //������� �����
    
  frame->Clear ();

    //��������� ������ ���� � ��������

  frame->SetProjection (camera->ProjectionMatrix ());
  frame->SetViewPoint  (camera->WorldPosition ());

    //����� �����

  RenderViewVisitor visitor (frame, render);

  scene->VisitEach (visitor);
  
    //���������� ����� �� ���������
    
  render->Renderer ()->AddFrame (frame.get ());
}

/*
    ������� ������
*/

void RenderView::AddRef ()
{
  addref (this);
}

void RenderView::Release ()
{
  release (this);
}
