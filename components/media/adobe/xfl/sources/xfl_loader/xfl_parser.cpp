#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

namespace
{

const char* LOG_NAME = "media.adobe.xfl.XflParser";
const char* ROOT_TAG = "DOMDocument";

}

/*
    Конструктор / деструктор
*/

XflParser::XflParser (const char* file_name, const char* path_prefix, Document& in_document)
  : document (in_document), parser (file_name), log (LOG_NAME), path_prefix (path_prefix)
{
  try
  {
    ParseRoot (parser.Root ().First (ROOT_TAG));
  }
  catch (...)
  {
    PrintLog ();
    throw;
  }

  PrintLog ();
}

XflParser::~XflParser ()
{
}

/*
    Печать лога парсинга
*/

void XflParser::PrintLog ()
{
  const ParseLog& parser_log = parser.Log ();

  try
  {  
    for (size_t i = 0; i < parser_log.MessagesCount (); ++i)
      log.Print (parser_log.Message (i));
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Разбор корневого узла
*/

void XflParser::ParseRoot (Parser::Iterator iter)
{
  if (!iter)
    raise_parser_exception (parser.Root (), "Wrong file format. No '%s' tag", ROOT_TAG);

  document.SetFrameRate (get<float> (*iter, "frameRate"));
  document.SetWidth     (get<size_t> (*iter, "width"));
  document.SetHeight    (get<size_t> (*iter, "height"));

    //TODO read color

    //разбор библиотек

  ParseLibraries (iter);

   //TODO rebuild paths
}

/*
    Разбор библиотек
*/

void XflParser::ParseLibraries (Parser::Iterator iter)
{
/*  for_each_child (*iter, "library_effects", bind (&XflParser::ParseLibraryEffects, this, _1));
  for_each_child (*iter, "library_materials", bind (&XflParser::ParseLibraryMaterials, this, _1));
  for_each_child (*iter, "library_geometries", bind (&XflParser::ParseLibraryGeometries, this, _1));
  for_each_child (*iter, "library_controllers", bind (&XflParser::ParseLibraryControllers, this, _1));
  for_each_child (*iter, "library_lights", bind (&XflParser::ParseLibraryLights, this, _1));
  for_each_child (*iter, "library_cameras", bind (&XflParser::ParseLibraryCameras, this, _1));
  for_each_child (*iter, "library_visual_scenes", bind (&XflParser::ParseLibraryVisualScenes, this, _1));*/
}
