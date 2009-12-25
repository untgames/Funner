#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const size_t DEFAULT_RESOURCES_COUNT = 1024;

}

/*
    Разбор библиотеки ресурсов
*/

void XflParser::ParseResources (common::Parser::Iterator iter)
{
  document.Resources ().Reserve (DEFAULT_RESOURCES_COUNT);

  for (Parser::NamesakeIterator i = iter->First ("DOMBitmapItem"); i; ++i)
  {
    Resource new_resource;

    ParseBitmap (i, new_resource);

    document.Resources ().Add (new_resource);
  }
}

/*
    Разбор картинки
*/

void XflParser::ParseBitmap (Parser::Iterator iter, Resource& resource)
{
  resource.SetName (get<const char*> (*iter, "name"));
  resource.SetPath (get<const char*> (*iter, "href"));
}
