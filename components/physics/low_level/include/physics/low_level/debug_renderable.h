#ifndef PHYSICS_LOW_LEVEL_DEBUG_RENDERABLE_HEADER
#define PHYSICS_LOW_LEVEL_DEBUG_RENDERABLE_HEADER

#include <math/vector.h>

namespace physics
{

namespace low_level
{

struct RenderablePoint
{
  math::vec3f position;
  math::vec3f color;
};

struct RenderableLine
{
  math::vec3f begin;
  math::vec3f end;
  math::vec3f begin_color;
  math::vec3f end_color;
};

struct RenderableTriangle
{
  math::vec3f point1;
  math::vec3f point2;
  math::vec3f point3;
  math::vec3f point1_color;
  math::vec3f point2_color;
  math::vec3f point3_color;
};

struct RenderableText
{
  math::vec3f position;
  const char* text;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные для отладочной отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDebugRenderable : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t              PointsCount    () = 0;
    virtual size_t              LinesCount     () = 0;
    virtual size_t              TrianglesCount () = 0;
    virtual size_t              TextsCount     () = 0;
    virtual RenderablePoint*    Points         () = 0;
    virtual RenderableLine*     Lines          () = 0;
    virtual RenderableTriangle* Triangles      () = 0;
    virtual RenderableText*     Texts          () = 0;
};

}

}

#endif
