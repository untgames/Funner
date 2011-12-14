#include "shared.h"

/*
    Константы
*/

const char* RESULTS_DIR     = "screenshots";                 //имя папки результатов
const char* SCREENSHOT_NAME = "screenshots/screenshot1.tga"; //имя файла, в который будет сохранён образ экрана
const char* TEXTURE_NAME    = "data/pic1.pvr";               //имя текстуры

/*
    Данные теста
*/

class Test: public BasicTest
{
  public:
///Конструктор
    Test () : BasicTest (L"Window driver test #1")
    {
        //инициализация параметров теста

      sprite_angle    = 0;
      sprite_rotation = 0;
      screenshot_made = false;

        //создание ресурсов

      TexturePtr texture = LoadTexture (TEXTURE_NAME);

      primitive1 = PrimitivePtr (Renderer ()->CreatePrimitive (), false);
      primitive2 = PrimitivePtr (Renderer ()->CreatePrimitive (), false);

      Sprite sprite1 = {vec3f (0.f, 0.f, 0.1f), vec2f (1.f, 1.f), vec4f (1.f, 1.f, 1.f, 1.0f), vec2f (0.f, 0.f), vec2f (1.f, 1.f)};
      Sprite sprite2 = {vec3f (0.f, 0.f, 0.2f), vec2f (1.f, 1.f), vec4f (1.f, 1.f, 1.f, 1.0f), vec2f (0.f, 0.f), vec2f (1.f, 1.f)};

      primitive1->SetTexture (texture.get ());
      primitive2->SetTexture (texture.get ());

      primitive1->SetBlendMode (BlendMode_Translucent);
      primitive2->SetShaderMode (ShaderMode_AlphaClamp);

      printf ("Primitive1 sprites count is %u\n", primitive1->GetSpritesCount ());

      primitive1->AddSprites (1, &sprite1);
      primitive2->AddSprites (1, &sprite2);

      printf ("Primitive1 sprites count is %u\n", primitive1->GetSpritesCount ());

      // building frame

      printf ("Frame primitives count is %u\n", Frame ()->PrimitivesCount ());

      Frame ()->AddPrimitive (primitive1.get ());
      Frame ()->AddPrimitive (primitive2.get ());

      printf ("Frame primitives count is %u\n", Frame ()->PrimitivesCount ());
    }

  private:
///Обработчик главного цикла приложения
    void OnIdle ()
    {
      static const float PI = 3.1415926f;

      mat4f tm1 = rotate (radian (sprite_rotation), vec3f (0, 0, 1)) * translate (vec3f (cos (sprite_angle) * 10 + 5, sin (sprite_angle) * 10 + 5, 0.1f)) * scale (vec3f (40, 40, 1)) * rotate (-radian (sprite_rotation), vec3f (0, 0, 1));
      mat4f tm2 = translate (vec3f (cos (sprite_angle + PI) * 10 + 5, sin (sprite_angle + PI) * 10 + 5, 0.2f)) * rotate (radian (sprite_rotation), vec3f (0, 0, 1)) * scale (vec3f (40, 40, 1)) * rotate (-radian (sprite_rotation), vec3f (0, 0, 1));

      primitive1->SetTransform (tm1);
      primitive2->SetTransform (tm2);

      primitive2->SetAlphaReference (0.5f * (1.0f + cos(sprite_angle)));

      sprite_angle    += 0.0005f;
      sprite_rotation += 0.0005f;

      if (!screenshot_made)
      {
        media::Image screenshot;

        Renderer ()->GetFrameBuffer (0)->GetColorBuffer ()->CaptureImage (screenshot);

        screenshot.Save (SCREENSHOT_NAME);

        screenshot_made = true;
      }
    }

  private:
    float        sprite_angle;
    float        sprite_rotation;
    bool         screenshot_made;
    PrimitivePtr primitive1;
    PrimitivePtr primitive2;
};

int main ()
{
  printf ("Results of low_level_driver1_test:\n");

  try
  {
    if (!common::FileSystem::IsDir (RESULTS_DIR))
      common::FileSystem::Mkdir (RESULTS_DIR);

    Test test;

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
