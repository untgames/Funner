#include "shared.h"

const size_t SPRITES_COUNT = 200;

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

struct TestScene
{
  SpriteList::Pointer  sprite_list;
  OrthoCamera::Pointer camera;
  Scene                scene;

  TestScene ()
  {
    sprite_list = SpriteList::Create ();    
    
    sprite_list->SetName     ("SpriteList");
    sprite_list->SetMaterial ("burst_material");
    sprite_list->Reserve     (SPRITES_COUNT);
    sprite_list->BindToScene (scene);

    for (size_t i=0; i<SPRITES_COUNT; i++)
    {
      SpriteModel::SpriteDesc sprite;

      float scale = frand (1, 4);      

      sprite.position = math::vec3f (frand (-10, 10), frand (-10, 10), frand (-10, 10));
      sprite.size     = scale;
      sprite.color    = math::vec4f (frand (), frand (), frand (), frand ());
      sprite.frame    = rand ();  

      sprite_list->Insert (sprite);
    }

    camera = OrthoCamera::Create ();    
    
    camera->BindToScene (scene, NodeBindMode_Capture);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, -3);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (-10);
    camera->SetZFar     (10);    
  }
};

void idle (TestApplication& app, TestScene& scene)
{
  try
  {
    static clock_t last_update = 0;

    if (clock () - last_update >= CLK_TCK / 25)
    {    
      SpriteModel::SpriteDesc* sprite        = scene.sprite_list->Sprites ();
      size_t                   sprites_count = scene.sprite_list->SpritesCount ();

      for (size_t i=0; i<sprites_count; i++, sprite++)
        sprite->frame++;

      scene.sprite_list->Invalidate ();

      last_update = clock ();
    }    

    app.PostRedraw ();
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
      //инициализация приложения

    TestApplication test;
    
      //создание сцены
      
    TestScene scene;
      
      //создание областей вывода
      
    Viewport vp;
    Desktop  desktop;

    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (scene.camera.get ());

    vp.SetArea (0, 0, 100, 100);

    desktop.Attach (vp);

    RenderTarget& render_target = test.RenderTarget ();
    
    render_target.SetDesktop (&desktop);

      //установка idle-функции

    test.SetIdleHandler (xtl::bind (&idle, _1, xtl::ref (scene)));

      //запуск приложения

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
