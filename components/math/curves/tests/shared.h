#ifndef MATH_CURVES_TESTS_SHARED_HEADER
#define MATH_CURVES_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/vector>

#include <math/angle.h>
#include <math/vector.h>
#include <math/quat.h>

#include <math/basic_spline.h>

using namespace math;

void dump (float x)
{
  static const float EPS = 0.001f;

  if (fabs (x) < EPS)
    x = 0.0f;

  printf ("%.3f", x);
}

void dump (bool x)
{
  printf (x ? "true" : "false");
}

template <class T>
void dump (const angle<T>& ang)
{
  dump (degree (ang));
  printf (" deg");
}

template <class T, unsigned int Size>
void dump (const vector<T, Size>& v)
{
  printf ("[");

  for (unsigned int i=0; i<Size; i++)
  {
    if (i)
      printf (", ");
  
    dump (v [i]);
  }
  
  printf ("]");
}

template <class Time, class Spline>
void test_spline (const char* message, Time begin, Time end, const Spline spline)
{
  static const int N = 10;
  
  Time dt = (end - begin) / N;
  
  printf ("%s (from ", message);
  dump (begin);
  printf (" to ");
  dump (end);
  printf ("):\n");
  
  for (Time t=begin; t<end+dt; t+=dt)
  {
    typename Spline::value_type value = spline (t);
    
    printf ("  f(");
    dump (t);
    printf (")=");
    dump (value);
    printf ("\n");
  }
}

template <class T> struct Point
{
  float time;
  T     value;
      
  Point (float in_time, const T& in_value)
    : time (in_time)
    , value (in_value)
  {
  }
};

void get_bounds (const stl::vector<Point<float> >& points, float& min_x, float& min_y, float& max_x, float& max_y)
{
  min_x = points.front ().time;
  max_x = min_x;
  min_y = points.front ().value;
  max_y = min_y;

  for (stl::vector<Point<float> >::const_iterator iter=points.begin (); iter!=points.end (); ++iter)
  {
    const Point<float>& point = *iter;
    
    if (point.time < min_x) min_x = point.time;
    if (point.time > max_x) max_x = point.time;
    
    if (point.value < min_y) min_y = point.value;
    if (point.value > max_y) max_y = point.value;
  }
}

void get_bounds (const stl::vector<Point<vec2f> >& points, float& min_x, float& min_y, float& max_x, float& max_y)
{
  min_x = points.front ().value.x;
  max_x = min_x;
  min_y = points.front ().value.y;
  max_y = min_y;
  
  for (stl::vector<Point<vec2f> >::const_iterator iter=points.begin (); iter!=points.end (); ++iter)
  {
    const Point<vec2f>& point = *iter;
    
    if (point.value.x < min_x) min_x = point.value.x;
    if (point.value.x > max_x) max_x = point.value.x;
    
    if (point.value.y < min_y) min_y = point.value.y;
    if (point.value.y > max_y) max_y = point.value.y;
  }
}

void get_values (const Point<float>& point, float& x, float& y)
{
  x = point.time;
  y = point.value;
}

void get_values (const Point<vec2f>& point, float& x, float& y)
{
  x = point.value.x;
  y = point.value.y;
}

template <class Point>
void plot (const stl::vector<Point>& points, size_t width, size_t height)
{
  stl::vector<char> field ((width + 1) * height, ' ');
  
  if (!points.empty ())
  {
    float min_x = 0.0f, min_y = 0.0f, max_x = 0.0f, max_y = 0.0f;

    get_bounds (points, min_x, min_y, max_x, max_y);
    
    if (min_x == max_x)
    {
      min_x = min_x - 100.f;
      max_x = max_x + 100.f;
    }
    
    if (min_y == max_y)
    {
      min_y = min_y - 100.f;
      max_y = max_y + 100.f;
    }
    
    printf ("bounds: (%.3f-%.3f)-(%.3f-%.3f)\n", min_x, min_y, max_x, max_y);
    
    for (stl::vector<Point>::const_iterator iter=points.begin (); iter!=points.end (); ++iter)
    {
      const Point& point = *iter;
      
      float x, y;
      
      get_values (point, x, y);
      
      unsigned int row    = (unsigned int)((y - min_y) / (max_y - min_y) * height),
                   column = (unsigned int)((x - min_x) / (max_x - min_x) * width);
                   
      if (row >= height || column >= width)
        continue;
        
      field [row * (width + 1) + column] = '*';
    }
  }
  
  for (size_t i=0; i<height; i++)
  {
    char* row = &field [(height - 1 - i) * (width + 1)];
  
    row [width] = '\0';
    
    printf ("%s\n", row);
  }
}

template <class Spline>
void plot_spline (const char* message, const Spline& spline, size_t width=100, size_t height=100)
{
  typedef stl::vector<Point<typename Spline::value_type> > point_array;
  
  static const int N = 100;
  
  point_array points;
  
  points.reserve (N+1);
  
  if (spline.min_time () != spline.max_time ())
  {
    float dt = (spline.max_time () - spline.min_time ()) / N;
    
    for (float t=spline.min_time (); t<spline.max_time ()+dt; t+=dt)
    {
      points.push_back (typename point_array::value_type (t, spline (t)));
    }
  }

  printf ("%s\n", message); 
  plot (points, width, height);
}

template <class T>
void dump (const char* message, const T& x)
{
  printf ("%s: ", message);
  dump (x);
  printf ("\n");
}

#endif
