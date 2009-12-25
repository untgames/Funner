#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t DEFAULT_LAYERS_COUNT = 512;

}

/*
    Разбор библиотеки анимаций
*/

void XflParser::ParseTimelines (common::Parser::Iterator iter)
{
  for (Parser::NamesakeIterator i = iter->First ("DOMTimeline"); i; ++i)
  {
    Timeline new_timeline;

    ParseTimeline (i, new_timeline);

    document.Timelines ().Add (new_timeline);
  }
}

/*
    Разбор анимации
*/

void XflParser::ParseTimeline (Parser::Iterator iter, Timeline& timeline)
{
  timeline.SetName (get<const char*> (*iter, "name"));

  timeline.Layers ().Reserve (DEFAULT_LAYERS_COUNT);

  for (Parser::NamesakeIterator i = iter->First ("layers.DOMLayer"); i; ++i)
  {
    Layer new_layer;

    ParseLayer (i, new_layer);

    timeline.Layers ().Add (new_layer);
  }
}
