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
  RenderablePoint begin;
  RenderablePoint end;
};

struct RenderableTriangle
{
  RenderablePoint point [3];
};

struct RenderableText
{
  math::vec3f position;
  const char* text;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDebugRenderable : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
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
