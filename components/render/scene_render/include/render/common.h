#ifndef RENDER_SCENE_RENDER_COMMON_HEADER
#define RENDER_SCENE_RENDER_COMMON_HEADER

#include <cstddef>

namespace render
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прямоугольная область
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Rect
{
  int    x, y;
  size_t width, height;
  
  Rect ();
  Rect (int x, int y, size_t width, size_t height);
};

}

#endif
