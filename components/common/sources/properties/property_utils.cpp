#include "shared.h"

namespace common
{

const char* get_name (PropertyType type)
{
  switch (type)
  {
    case PropertyType_String:  return "string";
    case PropertyType_Integer: return "integer";
    case PropertyType_Float:   return "float";
    case PropertyType_Vector:  return "vector";
    case PropertyType_Matrix:  return "matrix";
    default:
      throw xtl::make_argument_exception ("common::get_name(PropertyType)", "type", type);
  }
}

size_t get_size (PropertyType type)
{
  switch (type)
  {
    case PropertyType_String:  return sizeof (size_t);
    case PropertyType_Integer: return sizeof (int);
    case PropertyType_Float:   return sizeof (float);
    case PropertyType_Vector:  return sizeof (math::vec4f);
    case PropertyType_Matrix:  return sizeof (math::mat4f);
    default:
      throw xtl::make_argument_exception ("common::get_size(PropertyType)", "type", type);
  }
}

}
