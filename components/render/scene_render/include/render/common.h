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
  int    left, top;
  size_t width, height;
  
  Rect ();
  Rect (int left, int top, size_t width, size_t height);
};

#include <render/detail/common.inl>

}

#endif
