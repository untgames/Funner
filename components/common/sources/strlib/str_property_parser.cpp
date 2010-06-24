#include <xtl/function.h>

#include <common/property_map.h>
#include <common/strlib.h>

using namespace common;

namespace
{

struct PropertyMapBuilder
{ 
  void operator () (const char* name, const char* value)
  {
    map.SetProperty (name, value);
  }
  
  PropertyMap map;
};

}

namespace common
{

PropertyMap parse_init_string (const char* init_string)
{
  PropertyMapBuilder builder;
  
  parse_init_string (init_string, builder);
  
  return builder.map;
}

}
