#include "shared.h"

using namespace math;
using namespace render::mid_level::renderer2d;

Primitive2dPtr create_primitive (const Renderer2dPtr& renderer)
{
    //создание списка спрайтов

  Sprite sprites [4];
  
  size_t sprites_count = sizeof (sprites) / sizeof (*sprites);
  
  for (size_t i=0; i<sprites_count; i++)
  {
    Sprite& s = sprites [i];
    
    s.position   = vec3f (float (i), float (i), float (i));
    s.size       = vec2f (1.0f, float (i * i));
    s.color      = vec4f (0.0f, 1.0f, 0.0f, float (i));
    s.tex_offset = vec2f (0.0f, 0.0f);
    s.tex_size   = vec2f (0.5f, 0.5f);
  }    
  
    //создание примитива
  
  Primitive2dPtr primitive (renderer->CreatePrimitive (), false);
  
  primitive->AddSprites (sprites_count, sprites);
  
  return primitive;
}

int main ()
{
  printf ("Results of renderer2d_frame1_test:\n");

  try
  {
    Test test;
    
      //создание базового рендера
    
    RendererPtr basic_renderer (DriverManager::CreateRenderer ("Debug", "Renderer2d"), false);

      //приведение к думерному рендеру

    Renderer2dPtr renderer = xtl::dynamic_pointer_cast<renderer2d::IRenderer> (basic_renderer);
    
      //создание текстуры
    
    Texture2dPtr texture (renderer->CreateTexture (400, 300, media::PixelFormat_RGBA8), false);
    
      //создание примитивов
    
    Primitive2dPtr primitive1 = create_primitive (renderer),
                   primitive2 = create_primitive (renderer);
                   
    primitive1->SetBlendMode (BlendMode_Additive);
    primitive2->SetTexture   (texture.get ());
    
      //создание кадра
      
    Frame2dPtr frame (renderer->CreateFrame (), false);
    
    Viewport viewport;

    viewport.x      = 0;
    viewport.y      = 0;    
    viewport.width  = 100;
    viewport.height = 100;
    
    frame->SetRenderTargets (renderer->GetColorBuffer (0), renderer->GetDepthStencilBuffer (0));
    frame->SetViewport      (viewport);
            
    frame->AddPrimitive (primitive1.get ());
    frame->AddPrimitive (primitive2.get ());
    
      //рисование
      
    renderer->AddFrame (frame.get ());
    renderer->DrawFrames ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
