#include <cstdio>

#include <render/debug_render.h>

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
  printf ("result of debug_render test:\n");

  MyPrimitiveRender render;

  {
    printf ("Drawing single line:\n");

    RenderablePoint from, to;

    from.position = math::vec3f (0.1f, 0.2f, 0.3f);
    from.color    = math::vec4f (0.1f, 0.2f, 0.3f, 0.4f);
    to.position   = math::vec3f (0.5f, 0.6f, 0.7f);
    to.color      = math::vec4f (0.5f, 0.6f, 0.7f, 0.8f);

    render.DrawLine (from, to);
  }

  {
    printf ("Drawing two lines:\n");

    RenderablePoint vertices [4];

    for (size_t i = 0; i < 4; i++)
    {
      vertices [i].position = math::vec3f (i * 0.1f, i * 0.2f, i * 0.3f);
      vertices [i].color    = math::vec4f (i * 0.1f, i * 0.2f, i * 0.3f, i * 0.4f);
    }

    render.DrawLines (2, vertices);
  }

  {
    printf ("Drawing triangle:\n");

    RenderablePoint vertices [3];

    for (size_t i = 0; i < 3; i++)
    {
      vertices [i].position = math::vec3f (i * 0.1f, i * 0.2f, i * 0.3f);
      vertices [i].color    = math::vec4f (i * 0.1f, i * 0.2f, i * 0.3f, i * 0.4f);
    }

    render.DrawTriangle (vertices);
  }

  {
    printf ("Drawing text:\n");

    render.DrawText ("some text", math::vec3f (0.1f, 0.2f, 0.3f), math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  {
    printf ("Drawing wire sphere:\n");

    render.DrawWireSphere (math::vec3f (0.1f, 0.2f, 0.3f), 2.f, math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  {
    printf ("Drawing wire box:\n");

    render.DrawWireBox (math::vec3f (0.1f, 0.2f, 0.3f), math::vec3f (0.7f, 0.8f, 0.9f), math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  {
    printf ("Drawing wire cone:\n");

    render.DrawWireCone (math::vec3f (0.f, 0.f, 0.f), 2.f, math::vec3f (0.f, 1.f, 0.f), 2.f, math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  {
    printf ("Drawing wire truncated cone:\n");

    render.DrawWireTruncatedCone (math::vec3f (0.f, 0.f, 0.f), 2.f, math::vec3f (0.f, 1.f, 0.f), 2.f, 1.8f, math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  {
    printf ("Drawing wire cylinder:\n");

    render.DrawWireCylinder (math::vec3f (0.f, 0.f, 0.f), 2.f, math::vec3f (0.f, 1.f, 0.f), 2.f, math::vec4f (0.1f, 0.2f, 0.3f, 0.4f));
  }

  return 0;
}
