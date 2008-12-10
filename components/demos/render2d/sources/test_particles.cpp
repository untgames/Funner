#include "shared.h"

const size_t PARTICLES_COUNT = 100;
char* PARTICLES_FILE_NAME = "media/explode.wxf";

typedef stl::vector<math::vec3f> ParticleArray;

struct ParticleFrame
{
  ParticleArray particles;

  void Load (common::Parser::Iterator iter)
  {
    particles.reserve (PARTICLES_COUNT);

    read (*iter, "", stl::back_inserter (particles), iter->AttributesCount ()/3);

    for (ParticleArray::iterator pos_iter=particles.begin (); pos_iter!=particles.end (); ++pos_iter)
      stl::swap (pos_iter->y, pos_iter->z);
  }
};

typedef xtl::shared_ptr<ParticleFrame>  ParticleFramePtr;
typedef stl::vector<ParticleFramePtr>   ParticleFrameArray;

struct ParticleSystem
{
  ParticleFrameArray frames;

  void Load (const char* file_name)
  {
    common::Parser parser (file_name, "wxf");
    common::ParseLog log = parser.Log ();

    for (common::Parser::NamesakeIterator iter=parser.Root ().First ("positions"); iter; ++iter)
    {
      ParticleFramePtr frame (new ParticleFrame);

      frame->Load (iter);

      frames.push_back (frame);
    }

    for (size_t i=0; i<log.MessagesCount (); i++)
      printf ("%s\n", log.Message (i));
  }
};

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

struct TestScene
{
  SpriteList::Pointer  sprite_list;
  OrthoCamera::Pointer camera;
  Scene                scene;
  ParticleSystem       particle_system;
  size_t               current_particle_frame;

  TestScene () : current_particle_frame (0)
  {
    particle_system.Load (PARTICLES_FILE_NAME);

    sprite_list = SpriteList::Create ();

    sprite_list->SetName     ("SpriteList");
//    sprite_list->SetMaterial ("font_material");
    sprite_list->SetMaterial ("pistol_material");
    sprite_list->Reserve     (PARTICLES_COUNT);
    sprite_list->BindToScene (scene);

    for (size_t i=0; i<PARTICLES_COUNT; i++)
    {
      SpriteModel::SpriteDesc sprite;

      float scale = frand (2, 8);

      sprite.position = math::vec3f (frand (-10, 10), frand (-10, 10), frand (-10, 10));
      sprite.size     = scale;
      sprite.color    = math::vec4f (1, 1, 1, 1);

      sprite.frame    = rand ();

      sprite_list->Insert (sprite);
    }

    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, -50);
    camera->SetLeft     (-100);
    camera->SetRight    (100);
    camera->SetTop      (100);
    camera->SetBottom   (-100);
    camera->SetZNear    (-100);
    camera->SetZFar     (100);
  }
};

void idle (TestApplication& app, TestScene& scene)
{
  try
  {
    static clock_t last_frame_update = 0,
                   last_pos_update = 0;

    if (clock () - last_pos_update >= CLK_TCK / 25)
    {
      if (scene.particle_system.frames.empty ())
        return;

      SpriteModel::SpriteDesc* sprite = scene.sprite_list->Sprites ();
      ParticleFrame& particle_frame   = *scene.particle_system.frames [scene.current_particle_frame];

      for (ParticleArray::iterator  pos_iter = particle_frame.particles.begin (); pos_iter!=particle_frame.particles.end (); ++pos_iter, ++sprite)
      {
        sprite->position = *pos_iter;
      }

      scene.sprite_list->Invalidate ();

      scene.current_particle_frame++;

      if (scene.current_particle_frame >= scene.particle_system.frames.size ())
        scene.current_particle_frame = 0;

      last_pos_update = clock ();
    }

    if (clock () - last_frame_update >= CLK_TCK / 20)
    {
      SpriteModel::SpriteDesc* sprite         = scene.sprite_list->Sprites ();
      size_t                   sprites_count  = scene.sprite_list->SpritesCount ();

      for (size_t i=0; i<sprites_count; i++, sprite++)
        sprite->frame++;

      scene.sprite_list->Invalidate ();

      last_frame_update = clock ();
    }

    app.PostRedraw ();
  }
  catch (std::exception& exception)
  {
    printf ("exception at idle: %s\n", exception.what ());
  }
}

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main (int argc, char* argv [])
{
  if (argc >= 2)
  {
    PARTICLES_FILE_NAME = argv [1];
  }

  printf ("Results of test_particles:\n");

  common::LogFilter filter ("*", &log_print);

  try
  {
      //инициализация приложения

    TestApplication test;

      //создание сцены

    TestScene scene;

      //создание областей вывода

    Viewport vp;
    Screen screen;

    screen.SetBackgroundColor (1, 0, 0, 0);

    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (scene.camera.get ());
    vp.SetBackgroundColor (0, 0, 0, 0);
    vp.EnableBackground ();

    vp.SetArea (10, 10, 80, 80);

    screen.Attach (vp);

    RenderTarget& render_target = test.RenderTarget ();

    render_target.SetScreen (&screen);

      //установка idle-функции

    test.SetIdleHandler (xtl::bind (&idle, _1, xtl::ref (scene)));

      //запуск приложения

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 0;
  }
}
