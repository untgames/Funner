#include "shared.h"

int main ()
{
  printf ("Results of renderer2d_1_test:\n");
  
  try
  {
    Test test;
    
      //создание базового рендера
    
    RendererPtr basic_renderer (DriverManager::CreateRenderer ("debug", "renderer2d"), false);
    
      //приведение к думерному рендеру
    
    Renderer2dPtr renderer = xtl::dynamic_pointer_cast<renderer2d::IRenderer> (basic_renderer);
    
      //создание текстуры
    
    Texture2dPtr texture (renderer->CreateTexture (400, 300, media::PixelFormat_RGBA8));
    
      //вывод параметров текстуры
      
    printf ("check texture size: %ux%u\n", texture->GetWidth (), texture->GetHeight ());
    
      //проверка получения образа текстуры
    
    media::Image image;
    
    texture->CaptureImage (image);
    
    printf ("check image after capture: %ux%ux%u (format=%u)\n", image.Width (), image.Height (), image.Depth (), image.Format ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
