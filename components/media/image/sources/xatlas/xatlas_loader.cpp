#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

/*
    Загрузка карты картинок
*/

namespace
{

void xatlas_load (const char* file_name, Atlas& atlas)
{
  ParseLog         log;
  Parser           p (log, file_name);
  Parser::Iterator iter = p.Root ()->First("Atlas");
  stl::string      string_buffer;
  Atlas            new_atlas;

  static const char* METHOD_NAME = "media::xatlas_load";

  for (Parser::NamesakeIterator i = iter->First ("Tile"); i; ++i)
    if (!test (i, "Id") || !test (i, "XPosition") || !test (i, "YPosition") || !test (i, "Width") || !test (i, "Heigth"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      break;
    }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      throw xtl::format_operation_exception (METHOD_NAME, log.Message(i));

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Atlas' root tag");
    
  if (!test (iter, "Image"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Image' property");
    
  read (iter, "Image", string_buffer, "");
  new_atlas.SetImage (string_buffer.c_str ());

  for (Parser::NamesakeIterator i = iter->First ("Tile"); i; ++i)
  {
    Tile new_tile;

    read (i, "Id",        string_buffer);
    read (i, "XPosition", new_tile.origin.x);
    read (i, "YPosition", new_tile.origin.y);
    read (i, "Width",     new_tile.size.x);
    read (i, "Heigth",    new_tile.size.y);

    new_tile.name = string_buffer.c_str ();

    new_atlas.Insert (new_tile);
  }

  atlas.Swap (new_atlas);
}

/*
   Компонент загрузки карт картинок
*/

class XAtlasLoaderComponent
{
  public:
    //загрузка компонента
    XAtlasLoaderComponent () 
    {
      AtlasManager::RegisterLoader ("xatlas", &xatlas_load);
    }
};

extern "C"
{

ComponentRegistrator<XAtlasLoaderComponent> XAtlasLoader ("media.image.atlas.loaders.xatlas");

}

}
