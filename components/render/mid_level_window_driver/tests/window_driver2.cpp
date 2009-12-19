#include "shared.h"

/*
     онстанты
*/

const char*  TEXTURE_NAME  = "data/house.tif"; //им€ текстуры спрайтов
const size_t SPRITES_COUNT = 500;              //количество спрайтов

/*
    ƒанные теста
*/

class Test: public BasicTest
{
  public:
/// онструктор
    Test () : BasicTest (L"Window driver test #2")
    {
        //инициализаци€ параметров теста

      rotation_angle = 0.0f;

        //создание ресурсов

      primitive = PrimitivePtr (Renderer ()->CreatePrimitive (), false);

      primitive->SetTexture (LoadTexture (TEXTURE_NAME).get ());

      for (size_t i = 0; i < SPRITES_COUNT; i++)
      {
        Sprite sprite;

        sprite.position   = vec3f (frand (-100, 100), frand (-100, 100), frand (-100, 100));
        sprite.size       = vec2f (frand (1, 20), frand (1, 20));
        sprite.color      = vec4f (frand (), frand (), frand (), frand ());
        sprite.tex_offset = vec2f (0.f, 0.f);
        sprite.tex_size   = vec2f (1.f, 1.f);

        primitive->AddSprites (1, &sprite);
      }
      
      Viewport scissor = {100, 100, 600, 600};
      
      primitive->SetScissor (scissor);
      primitive->SetScissorState (true);

      // building frame

      Frame ()->AddPrimitive (primitive.get ());
    }

  private:
///ќбработчик главного цикла приложени€
    void OnIdle ()
    {
      primitive->SetTransform (rotate (radian (rotation_angle), vec3f (0, 0, 1)));

      rotation_angle += 0.05f;      
    }

  private:
    float        rotation_angle; //угол поворота спрайтов
    PrimitivePtr primitive;      //примитив, содержащий спрайты
};

int main ()
{
  printf ("Results of low_level_driver2_test:\n");

  try
  {
    return Test ().Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
