#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

/*
    Разбор кадра анимации
*/

void XflParser::ParseFrameElement (Parser::Iterator iter, Frame::FrameElementList& collection, FrameElementType type)
{
  FrameElement element (type);

  element.SetName       (get<const char*> (*iter, "libraryItemName"));
  element.SetFirstFrame (get<size_t>      (*iter, "firstFrame", 0));

  math::vec2f translation;

  translation.x = get<float> (*iter, "matrix.Matrix.tx");
  translation.y = get<float> (*iter, "matrix.Matrix.ty");

  element.SetTranslation (translation);

  math::vec2f transformation_point;

  transformation_point.x = get<float> (*iter, "transformationPoint.Point.x");
  transformation_point.y = get<float> (*iter, "transformationPoint.Point.y");

  element.SetTransformationPoint (transformation_point);

  collection.Add (element);
}
