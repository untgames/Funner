#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/iterator.h>

#include <math/matrix.h>

#include <common/property_map.h>

#include <media/rfx/effect_library.h>
#include <media/rfx/material_library.h>
#include <media/rfx/shader_library.h>

using namespace media::rfx;
using namespace common;
using namespace math;

void dump (const PropertyMap& properties, const char* indent)
{
  for (size_t i=0; i<properties.Size (); i++)
  {
    printf ("%s%s=", indent, properties.PropertyName (i));
  
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
    
    printf ("\n");
  }
}

void dump (const TechniquePass& pass)
{
  printf ("        pass\n          properties:\n");
  dump (pass.Properties (), "            ");
}

void dump (const Technique& technique)
{
  printf ("    technique:\n      name='%s'\n      passes:\n", technique.Name ());
  
  for (size_t i=0; i<technique.PassesCount (); i++)
  {
    dump (technique.Pass (i));
  }
  
  printf ("    properties:\n");
  dump (technique.Properties (), "      ");
}

void dump (const EffectParameter& param)
{
  printf ("    param:\n      name='%s'\n      type=%s\n      count=%u\n", param.Name (), get_name (param.Type ()), param.ElementsCount ());
}

void dump (const Effect& effect, const char* id)
{
  printf ("effect:\n  id='%s'\n  name='%s'\n  parameters:\n", id, effect.Name ());
  
  for (size_t i=0; i<effect.ParametersCount (); i++)
  {
    dump (effect.Parameter (i));
  }
  
  printf ("  techniques:\n");
  
  for (size_t i=0; i<effect.TechniquesCount (); i++)
  {
    dump (effect.Technique (i));
  }
}
