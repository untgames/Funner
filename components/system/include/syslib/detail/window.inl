/*
    Color
*/

inline Color::Color ()
  : red (0), green (0), blue (0)
  {}
  
inline Color::Color (unsigned char in_red, unsigned char in_green, unsigned char in_blue)
  : red (in_red), green (in_green), blue (in_blue)
  {}

/*
    Point
*/

inline Point::Point ()
  : x (0), y (0)
  {}

inline Point::Point (size_t in_x, size_t in_y)
  : x (in_x), y (in_y)
  {}
