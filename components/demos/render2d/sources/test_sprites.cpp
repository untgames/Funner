#include "shared.h"

const size_t SPRITES_COUNT = 200;

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

struct Test
{
  TestApplication      application;
  SpriteList::Pointer  sprite_list;
  Sprite::Pointer      sprite;
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;
 
  Test ()
  {
      //создание сцены
    
    sprite_list = SpriteList::Create ();    
    sprite      = Sprite::Create ();
    
    sprite_list->SetMaterial ("burst_material");
    sprite_list->Reserve     (SPRITES_COUNT);
    sprite_list->BindToScene (scene);
    
    sprite->SetMaterial ("dynamic_material");
    sprite->SetColor    (1, 1, 1, 0.95f);
    sprite->SetPosition (0, 0, -15);
    sprite->SetScale    (15, 15, 1);
    sprite->BindToScene (scene);

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
    camera->SetZNear    (-20);
    camera->SetZFar     (20);    
    
      //создание областей вывода
      
    Viewport vp;
    
    screen.SetBackgroundColor (1, 0, 0, 0);
    screen.SetBackgroundState (false);

    vp.SetName            ("Viewport1");
    vp.SetRenderPath      ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (0, 0, 0, 0);
    vp.EnableBackground   ();

    vp.SetArea (5, 5, 90, 90);
//    vp.SetArea (0, 0, 100, 100);

    screen.Attach (vp);    

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();
    
    render_target.SetScreen (&screen);    
    
      //настройка запросов рендеринга
      
    application.Render ().SetMaxDrawDepth (3);
      
    application.Render ().RegisterQueryHandler ("test_query", xtl::bind (&Test::SetupDynamicRenderTarget, this, _1, _2));
    
      //установка idle-функции

    application.SetIdleHandler (xtl::bind (&Test::Idle, this));    
  }
  
    //настройка динамического целевого буфера рендеринга
  void SetupDynamicRenderTarget (RenderTarget& render_target, const char*)
  {
    render_target.SetScreen (&screen);
  }
  
    //обработчик главного цикла приложения
  void Idle ()
  {
    try
    {
      static clock_t last_update = 0;

      if (clock () - last_update >= CLK_TCK / 25)
      {    
        SpriteModel::SpriteDesc* sprite        = sprite_list->Sprites ();
        size_t                   sprites_count = sprite_list->SpritesCount ();

        for (size_t i=0; i<sprites_count; i++, sprite++)
          sprite->frame++;

        sprite_list->Invalidate ();
        
        this->sprite->Rotate (1, 0, 0, 1);

        last_update = clock ();
      }    

      application.PostRedraw ();
    }
    catch (std::exception& exception)
    {
      printf ("exception at idle: %s\n", exception.what ());
    }    
  }
};

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of test_sprites:\n");

  try
  {    
      //настройка протоколированиЯ

    common::LogFilter filter ("*", &log_print);
    
      //запуск теста
    
    Test test;    

    return test.application.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
