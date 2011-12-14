/*
    Прямоугольная область
*/

inline Rect::Rect ()
{
  left = top = 0;
  width = height = 0;
}

inline Rect::Rect (int in_left, int in_top, size_t in_width, size_t in_height)
  : left (in_left), top (in_top), width (in_width), height (in_height)
  {}
