#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t MAX_CORRECT_VALUE = 1000000000;

//Чтение float значения, которое в случае отрицательного значения превращается в unsigned long long
float read_xfl_float (const ParseNode& node, const char* name)
{
  float float_value = get<float> (node, name);

  if (float_value < MAX_CORRECT_VALUE)
    return float_value;

  uint64_t unsigned_value = get<uint64_t> (node, name);

  unsigned_value = 0 - unsigned_value;

  return -(float)unsigned_value;
}

}

/*
    Разбор кадра анимации
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
