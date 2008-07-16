#include "shared.h"

const size_t SPRITES_COUNT = 200;

typedef stl::vector<Sprite::Pointer> SpriteArray;

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

struct TestScene
{
  SpriteArray          sprites;
  OrthoCamera::Pointer camera;
  Scene                scene;  

  TestScene ()
  {
    sprites.reserve (SPRITES_COUNT);
    
    for (size_t i=0; i<SPRITES_COUNT; i++)
    {
      Sprite::Pointer sprite = Sprite::Create ();      

      sprite->SetName     (common::format ("Sprite%u", i+1).c_str ());
      sprite->SetMaterial ("burst_material");
      
      sprite->SetColor    (math::vec4f (frand (), frand (), frand ()));
      sprite->SetAlpha    (frand ());    
      
      float scale = frand (1, 4);
      
      sprite->Scale       (scale, scale, 1);
      sprite->SetOrientation (frand () * 360.0f, 0, 0, 1);
      sprite->BindToScene (scene); 
      sprite->SetPosition (frand (-10, 10), frand (-10, 10), frand (-10, 10));
      sprite->SetFrame    (rand ());
      
      sprites.push_back (sprite);
    }

    camera = OrthoCamera::Create ();    
    
    camera->BindToScene (scene, NodeBindMode_Capture);
    camera->SetName ("Camera1");                
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
      for (SpriteArray::iterator iter=scene.sprites.begin (), end=scene.sprites.end (); iter!=end; ++iter)
      {
        scene_graph::Sprite& sprite = **iter;
        
        sprite.Rotate (1, 0, 0, 1);
        sprite.SetFrame ((*iter)->Frame () + 1);
      }
        
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
      //инициализаци€ приложени€

    TestApplication test;
    
      //инициализаци€ рендера
      
    SceneRender& render = test.Render ();
    
//    render.SetBackgroundColor (math::vec4f (1, 1, 1, 1));
    
      //создание сцены
      
    TestScene scene;
      
      //создание областей вывода
      
    Viewport vp;
    
    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (scene.camera.get ());
    
    vp.SetArea (0, 0, 100, 100);
    
    render.Attach (vp);

      //установка idle-функции

    test.SetIdleHandler (xtl::bind (&idle, _1, xtl::ref (scene)));

      //запуск приложени€

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
