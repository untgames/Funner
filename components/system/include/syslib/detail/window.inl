/*
    Point
*/

inline Point::Point ()
  : x (0), y (0)
  {}

inline Point::Point (size_t in_x, size_t in_y)
  : x (in_x), y (in_y)
  {}

/*
    Rect
*/  

inline Rect::Rect ()
{
  left = top = right = bottom = 0;
}

inline Rect::Rect (size_t in_left, size_t in_top, size_t in_right, size_t in_bottom)
{
  left   = in_left;
  top    = in_top;
  right  = in_right;
  bottom = in_bottom;
}
