#ifndef RENDER_DEBUG_RENDER_HEADER
#define RENDER_DEBUG_RENDER_HEADER

#include <cstddef>

#include <math/vector.h>

namespace render
{

namespace debug
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Визуализируемая точка
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderablePoint
{
  math::vec3f position;
  math::vec4f color;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рендер вспомогательной геометрии
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveRender
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawLine              (const RenderablePoint& from, const RenderablePoint& to);
    void DrawLines             (size_t lines_count, const RenderablePoint* points);
    void DrawTriangle          (const RenderablePoint points [3]);
    void DrawText              (const char* text, const math::vec3f& position, const math::vec4f& color);
    void DrawWireSphere        (const math::vec3f& position, float radius, const math::vec4f& color);
    void DrawWireBox           (const math::vec3f& min, const math::vec3f& max, const math::vec4f& color);
    void DrawWireCone          (const math::vec3f& position, float radius, const math::vec3f& normal, float height, const math::vec4f& color);
    void DrawWireTruncatedCone (const math::vec3f& position, float radius, const math::vec3f& normal, float height, float truncate_height, const math::vec4f& color);
    void DrawWireCylinder      (const math::vec3f& position, float radius, const math::vec3f& normal, float height, const math::vec4f& color);

  protected:
    virtual ~PrimitiveRender () {}

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Рисование примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawLinesCore (size_t lines_count, const RenderablePoint* points) = 0;
    virtual void DrawTextCore  (const char* text, const math::vec3f& position, const math::vec4f& color) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализации по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawCircle (const math::vec3f& position, const math::vec3f& normal, float radius, const math::vec4f color);
};

}

}

#endif
