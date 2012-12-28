#ifdef _MSC_VER
  typedef unsigned __int64 uint64;
#else
  typedef unsigned long long uint64;
#endif

namespace xtl
{

namespace io
{

bool read (const char* string, uint64& value);

}

}

#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace xtl
{

namespace io
{

inline bool read (const char* string, uint64& value)
{
  if (!string || !*string)
    return false;

#ifdef _MSC_VER
  uint64 tmp = _strtoui64 (string, (char**)&string, 0);
#else
  uint64 tmp = strtoull (string, (char**)&string, 0);
#endif

  if (!*string)
  {
    value = tmp;
    return true;
  }

  if (*string != '.')
    return false;

  while (*++string)
    if (!isdigit ((unsigned char)*string))
      return false;

  value = tmp;

  return true;
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, uint64& value)
{
  return detail::read_value (iter, value);
}

}

}

namespace
{

const size_t MAX_CORRECT_VALUE = 1000000000;

//„тение float значени€, которое в случае отрицательного значени€ превращаетс€ в unsigned long long
float read_xfl_float (const ParseNode& node, const char* name)
{
  float float_value = get<float> (node, name);

  if (float_value < MAX_CORRECT_VALUE)
    return float_value;

  uint64 unsigned_value = get<uint64> (node, name);

  unsigned_value = 0 - unsigned_value;

  return -(float)unsigned_value;
}

}

/*
    –азбор кадра анимации
*/

void XflParser::ParseFrameElement (Parser::Iterator iter, Frame::FrameElementList& collection, FrameElementType type)
{
  FrameElement element (type);

  element.SetName       (get<const char*> (*iter, "libraryItemName"));
  element.SetFirstFrame (get<size_t>      (*iter, "firstFrame", 0));

  math::vec2f translation;

  translation.x = read_xfl_float (*iter, "matrix.Matrix.tx");
  translation.y = read_xfl_float (*iter, "matrix.Matrix.ty");

  element.SetTranslation (translation);

  math::vec2f transformation_point;

  transformation_point.x = get<float> (*iter, "transformationPoint.Point.x");
  transformation_point.y = get<float> (*iter, "transformationPoint.Point.y");

  element.SetTransformationPoint (transformation_point);

  collection.Add (element);
}
