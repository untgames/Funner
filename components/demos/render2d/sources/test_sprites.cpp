#include "shared.h"

void idle (TestApplication& app)
{
  try
  {
    SceneRender& render = app.Render ();        

    render.Draw ();
  }
  catch (std::exception& exception)
  {
    printf ("exception at idle: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of test_sprites:\n");

  try
  {    
      //инициализаци€ приложени€

    TestApplication test;
    
      //инициализаци€ рендера
      
    SceneRender& render = test.Render ();
    
    render.SetBackgroundColor (math::vec4f (1, 1, 1, 1));
    
      //создание сцены
      
    Sprite::Pointer      sprite = Sprite::Create ();
    OrthoCamera::Pointer camera = OrthoCamera::Create ();

    Scene scene;

    sprite->BindToScene (scene, NodeBindMode_Capture);
    camera->BindToScene (scene, NodeBindMode_Capture);

    sprite->SetName ("Sprite1");
    camera->SetName ("Camera1");
    sprite->SetMaterial ("sprite_material");
    sprite->SetColor (math::vec4f (1.0f, 1.0f, 1.0f));
    sprite->SetAlpha (0.5f);
    
    sprite->Scale (20, 20, 1);
    
    camera->SetPosition (0, 0, -3);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (-10);
    camera->SetZFar     (10);

      //создание областей вывода
      
    Viewport vp;
    
    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (camera.get ());
    
    vp.SetArea (0, 0, 100, 100);
    
    render.Attach (vp);

      //установка idle-функции

//    test.SetIdleHandler (&idle);

      //запуск приложени€

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
