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
    Разбор корневого узла
*/

void XflParser::ParseRoot (Parser::Iterator iter)
{
  if (!iter)
    raise_parser_exception (*iter, "Wrong file format. No '%s' tag", ROOT_TAG);

  document.SetFrameRate (get<float> (*iter, "frameRate"));
  document.SetWidth     (get<size_t> (*iter, "width"));
  document.SetHeight    (get<size_t> (*iter, "height"));

  math::vec3f background_color;

  if (!ReadHexColor (get<const char*> (*iter, "backgroundColor"), background_color))
    raise_parser_exception (*iter, "Wrong color format in 'backgroundColor' attribute");

  document.SetBackgroundColor (background_color);

    //разбор библиотек

  ParseLibraries (iter);

  if (!path_prefix.empty ())
    for (Document::ResourceList::Iterator iter = document.Resources ().CreateIterator (); iter; ++iter)
    {
      stl::string full_resource_path = common::format ("%s/%s", path_prefix.c_str (), iter->Path ());

      iter->SetPath (full_resource_path.c_str ());
    }

  CheckDocument ();
}

/*
    Разбор библиотек
*/

void XflParser::ParseLibraries (Parser::Iterator iter)
{
  for_each_child (*iter, "media",     xtl::bind (&XflParser::ParseResources, this, _1));
  for_each_child (*iter, "symbols",   xtl::bind (&XflParser::ParseSymbols,   this, _1));
  for_each_child (*iter, "timelines", xtl::bind (&XflParser::ParseTimelines, this, _1));
}
