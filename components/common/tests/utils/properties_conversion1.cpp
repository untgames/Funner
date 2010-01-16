#include <cstdio>
#include <exception>

#include <stl/string>

#include <math/vector.h>
#include <math/matrix.h>

#include <common/property_map.h>

using namespace common;

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void test (PropertyMap& properties, const char* name)
{
  TEST
  (
    printf ("GetString('%s'): ", name);
    printf ("%s\n", properties.GetString (name));
  )
  
  TEST
  (
    printf ("GetInteger('%s'): ", name);
    printf ("%d\n", properties.GetInteger (name));
  )
  
  TEST
  (
    printf ("GetFloat('%s'): ", name);
    printf ("%.3f\n", properties.GetFloat (name));
  )
  
  TEST
  (
    printf ("GetVector('%s'): ", name);
    
    const math::vec4f& v = properties.GetVector (name);
    
    printf ("%.3f %.3f %.3f %.3f\n", v.x, v.y, v.z, v.w);
  )
  
  TEST
  (
    printf ("GetMatrix('%s'): ", name);

    const math::mat4f& m = properties.GetMatrix (name);

    printf ("[%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f]\n", 
      m [0][0], m [0][1], m [0][2], m [0][3], m [1][0], m [1][1], m [1][2], m [1][3], m [2][0], m [2][1], m [2][2], m [2][3], m [3][0], m [3][1], m [3][2], m [3][3]);
  )
  
  TEST
  (
    stl::string result;
  
    printf ("GetProperty('%s', stl::string&): ", name);
    
    properties.GetProperty (name, result);
    
    printf ("%s\n", result.c_str ());
  )
  
  TEST
  (
    int result = -1;
  
    printf ("GetProperty('%s', int&): ", name);
    
    properties.GetProperty (name, result);
    
    printf ("%d\n", result);
  )
  
  TEST
  (
    float result = -1.0f;
  
    printf ("GetProperty('%s', float&): ", name);
    
    properties.GetProperty (name, result);
    
    printf ("%.3f\n", result);
  )
  
  TEST
  (
    math::vec4f result = -1.0f;
  
    printf ("GetProperty('%s', vec4f&): ", name);
    
    properties.GetProperty (name, result);
    
    printf ("%.3f %.3f %.3f %.3f\n", result.x, result.y, result.z, result.w);
  )
  
  TEST
  (
    math::mat4f m = -1.0f;
  
    printf ("GetProperty('%s', matf&): ", name);
    
    properties.GetProperty (name, m);
    
    printf ("[%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f]\n", 
      m [0][0], m [0][1], m [0][2], m [0][3], m [1][0], m [1][1], m [1][2], m [1][3], m [2][0], m [2][1], m [2][2], m [2][3], m [3][0], m [3][1], m [3][2], m [3][3]);    
  )
}

int main ()
{
  printf ("Results of properties_conversion1_test:\n");
  
  try
  {
    PropertyMap properties;

    properties.SetProperty ("String",                "hello world");
    properties.SetProperty ("EmptyString",           "");
    properties.SetProperty ("StringWithFloat",       "3.14");
    properties.SetProperty ("StringWithVector",      "1 2.5 3 4");
    properties.SetProperty ("StringWithSmallVector", "1 2.5 4");
    properties.SetProperty ("StringWithBigVector",   "1 2 3 4 5");
    properties.SetProperty ("StringWithMatrix",      "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16");
    properties.SetProperty ("Integer",               13);
    properties.SetProperty ("Float",                 1.5f);
    properties.SetProperty ("Vector",                math::vec4f (1.0f, 2.0f, 3.0f, 4.0f));
    properties.SetProperty ("Matrix",                math::mat4f (3.0f));

    for (size_t i=0; i<properties.Size (); i++)
      test (properties, properties.PropertyName (i));    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
