#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t DEFAULT_FRAMES_COUNT = 2;

}

/*
    Разбор слоя анимации
*/

void XflParser::ParseLayer (Parser::Iterator iter, Layer& layer)
{
  layer.SetName (get<const char*> (*iter, "name"));

  layer.Frames ().Reserve (DEFAULT_FRAMES_COUNT);

  for (Parser::NamesakeIterator i = iter->First ("frames.DOMFrame"); i; ++i)
  {
    Frame new_frame;

    ParseFrame (i, new_frame);

    if (new_frame.Elements ().Size ())
      layer.Frames ().Add (new_frame);
  }
}
