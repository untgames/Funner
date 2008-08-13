#include "shared.h"

using namespace math;
using namespace render::mid_level::renderer2d;

int main ()
{
  printf ("Results of renderer2d_frame2_test:\n");

  try
  {
    Test test;
    
      //создание базового рендера
    
    RendererPtr basic_renderer (DriverManager::CreateRenderer ("Debug", "Renderer2d"), false);

      //приведение к думерному рендеру

    Renderer2dPtr renderer = xtl::dynamic_pointer_cast<renderer2d::IRenderer> (basic_renderer);
    
      //создание кадров

    Frame2dPtr frame1 (renderer->CreateFrame (), false),
               frame2 (renderer->CreateFrame (), false);

    Viewport viewport;

    viewport.x      = 0;
    viewport.y      = 0;    
    viewport.width  = 100;
    viewport.height = 100;
    
    frame1->SetRenderTargets (renderer->GetColorBuffer (0), renderer->GetDepthStencilBuffer (0));
    frame1->SetViewport      (viewport);
    
      //рисование
      
    renderer->AddFrame         (frame1.get ());
    renderer->SetFramePosition (0);
    renderer->AddFrame         (frame2.get ());
    renderer->DrawFrames       ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
