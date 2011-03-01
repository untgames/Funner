#ifndef MEDIALIB_RFX_TESTS_SHARED_HEADER
#define MEDIALIB_RFX_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <stl/string>

#include <math/matrix.h>

#include <media/rfx/material.h>
#include <media/rfx/shader.h>

using namespace media;
using namespace media::rfx;
using namespace math;
using namespace common;

void dump (const Shader& shader)
{
  printf ("shader (name='%s', profile='%s', source_size=%u, source='%s')", shader.Name (), shader.Profile (),
    shader.SourceCodeSize (), shader.SourceCode ());
}

void dump (const PropertyMap& properties)
{
  printf ("[");
  
  for (size_t i=0; i<properties.Size (); i++)
  {
    if (i)
      printf (", ");
      
    printf ("%s=", properties.PropertyName (i));
  
    PropertyType type = properties.PropertyType (i);
    
    switch (type)
    {
      case PropertyType_String:
        printf ("'%s'", properties.GetString (i));
        break;
      case PropertyType_Integer:
        printf ("%d", properties.GetInteger (i));
        break;
      case PropertyType_Float:
        printf ("%.3f", properties.GetFloat (i));
        break;
      case PropertyType_Vector:
      {
        vec4f v = properties.GetVector (i);
        printf ("[%.3f %.3f %.3f %.3f]", v.x, v.y, v.z, v.w);
        break;
      }
      case PropertyType_Matrix:
      {
        mat4f m = properties.GetMatrix (i);
        printf ("[[%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f] [%.3f %.3f %.3f %.3f]]",
          m [0][0], m [0][1], m [0][2], m [0][3], m [1][0], m [1][1], m [1][2], m [1][3], m [2][0], m [2][1], m [2][2], m [2][3],
          m [3][0], m [3][1], m [3][2], m [3][3]);
        break;
      }
      default:
      {
        stl::string s;
        
        properties.GetProperty (i, s);
        
        printf ("'%s'", s.c_str ());
        break;
      }
    }
  }
  
  printf ("]");
}

void dump (const Material& material)
{
  printf ("material (name='%s', effect='%s', properties=", material.Name (), material.Effect ());
  
  dump (material.Properties ());
  
  printf (")");
}

template <class T>
void dump (const char* prefix, const T& object)
{
  printf ("%s: ", prefix);
  dump (object);
  printf ("\n");
}

#endif
