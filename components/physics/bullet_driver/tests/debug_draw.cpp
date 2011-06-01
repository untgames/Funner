#include "shared.h"

using namespace render::debug;

class MyPrimitiveRender : public PrimitiveRender
{
  public:
    ///Рисование примитивов
    void DrawLinesCore (size_t lines_count, const RenderablePoint* points)
    {
      printf ("Drawing %u lines:\n", lines_count);

      for (size_t i = 0; i < lines_count; i++)
      {
        printf ("  position: ");
        Print  (points [i * 2].position);
        printf (" - ");
        Print  (points [i * 2 + 1].position);
        printf ("; color: ");
        Print  (points [i * 2].color);
        printf (" - ");
        Print  (points [i * 2 + 1].color);
        printf ("\n");
      }
    }

    void DrawTextCore  (const char* text, const math::vec3f& position, const math::vec4f& color)
    {
      printf ("Drawing text '%s' at ", text);
      Print (position);
      printf (" with color ");
      Print (color);
      printf ("\n");
    }

  private:
    void Print (const math::vec3f& v)
    {
      printf ("[%.2f %.2f %.2f]", v.x, v.y, v.z);
    }

    void Print (const math::vec4f& v)
    {
      printf ("[%.2f %.2f %.2f %.2f]", v.x, v.y, v.z, v.w);
    }
};

int main ()
{
  printf ("Results of debug_draw test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr falling_body (scene->CreateRigidBody (sphere_shape.get (), 5), false);

    Transform body_transform;

    body_transform.position.x = 10;

    falling_body->SetWorldTransform (body_transform);

    MyPrimitiveRender render;

    printf ("Drawing initial scene:\n");

    scene->Draw (render);

    scene->PerformSimulation (1.f);

    printf ("Drawing scene after simulation:\n");

    scene->Draw (render);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
