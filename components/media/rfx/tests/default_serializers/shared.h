#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/iterator.h>

#include <math/matrix.h>

#include <common/property_map.h>

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
    
    if (properties.PropertyElementsCount (i) > 1)
      printf ("(");
    
    for (size_t j=0, count=properties.PropertyElementsCount (i); j<count; j++)
    {
      if (j)
        printf (", ");
    
      switch (type)
      {
        case PropertyType_String:
          printf ("'%s'", properties.GetString (i));
          break;
        case PropertyType_Integer:
          printf ("%d", properties.GetIntegerArray (i)[j]);
          break;
        case PropertyType_Float:
          printf ("%.3f", properties.GetFloatArray (i)[j]);
          break;
        case PropertyType_Vector:
        {
          vec4f v = properties.GetVectorArray (i)[j];
          printf ("[%.3f %.3f %.3f %.3f]", v.x, v.y, v.z, v.w);
          break;
        }
        case PropertyType_Matrix:
        {
          mat4f m = properties.GetMatrixArray (i)[j];
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
    
    if (properties.PropertyElementsCount (i) > 1)
      printf (")");    
      
    printf ("\n");      
  }
}

void dump (const Material& material, const char* id)
{
  stl::string tags;
  
  for (size_t i=0; i<material.TagsCount (); i++)
  {
    if (i)
      tags += " ";
      
    tags += material.Tag (i);
  }  

  printf ("material:\n  id='%s'\n  name='%s'\n  program='%s'\n  tags='%s'\n  properties:\n", id, material.Name (), material.Program (), tags.c_str ());
  
  dump (material.Properties (), "    ");
  
  printf ("  texmaps:\n");
  
  for (size_t i=0; i<material.TexmapCount (); i++)
  {
    printf ("    semantic='%s', image='%s', sampler='%s'\n", material.Texmap (i).Semantic (), material.Texmap (i).Image (), material.Texmap (i).Sampler ());
  }  
}

void dump (const Shader& shader, const char* id)
{
  printf ("shader:\n  id='%s'\n  name='%s'\n  profile='%s'\n  source_length=%u\n  source:\n%s\n",
    id, shader.Name (), shader.Profile (), shader.SourceCodeSize (), shader.SourceCode ());
}
