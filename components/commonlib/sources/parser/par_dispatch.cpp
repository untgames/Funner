#include <common/strlib.h>
#include "dispatch.h"
#include "context.h"

using namespace common;
using namespace stl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Форматы разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ParseFormat
{
  PARSE_FORMAT_UNKNOWN,    //неизвестный формат
  PARSE_FORMAT_AUTODETECT, //формат определяется автоматически
  PARSE_FORMAT_WXF,        //WXF
  PARSE_FORMAT_XML         //XML
};

/*
    Утилиты определения формата исходного текста
*/

static ParseFormat DetectContentFormat (const char* buf)
{
  return strchr (buf,'<') ? PARSE_FORMAT_XML : PARSE_FORMAT_WXF;
}

static ParseFormat DetectFileFormat (const char* file_name)
{
  string type = suffix (file_name);
  
  if      (!string_wrappers::stricmp (type.c_str (),".wxf")) return PARSE_FORMAT_WXF;
  else if (!string_wrappers::stricmp (type.c_str (),".xml")) return PARSE_FORMAT_XML;
  else                                                       return PARSE_FORMAT_AUTODETECT;
}

ParseFormat GetParseFormat (const char* format_name)
{
  if (!format_name || !*format_name)
    return PARSE_FORMAT_AUTODETECT;
    
  if      (!string_wrappers::stricmp (format_name,"wxf"))  return PARSE_FORMAT_WXF;
  else if (!string_wrappers::stricmp (format_name,"xml"))  return PARSE_FORMAT_XML; 
  else if (!string_wrappers::stricmp (format_name,"auto")) return PARSE_FORMAT_AUTODETECT;
  else                                                     return PARSE_FORMAT_UNKNOWN;
}

/*
    ParseDispatch
*/

static void ParseBuffer (char* buf,ParseContext& context,ParseFormat format,const char* format_name)
{   
  if (!buf)
    return;

  if (format == PARSE_FORMAT_AUTODETECT)
    format = DetectContentFormat (buf);
 
  switch (format)
  {
    case PARSE_FORMAT_WXF: ParseDispatch::ParseWXF (buf,context); break;
    case PARSE_FORMAT_XML: ParseDispatch::ParseXML (buf,context); break;
    default:
      context.LogFatalError ("Unknown format '%s' at parse source '%s'",format_name,context.GetTextSourceName (buf));
      break;
  }
}

void ParseDispatch::Parse (const char* file_name,ParseContext& context,const char* format_name)
{
  if (!file_name)
  {
    context.LogFatalError ("Null file name");
    return;
  }

  ParseFormat format = GetParseFormat (format_name);

  if (format == PARSE_FORMAT_AUTODETECT)
    format = DetectFileFormat (file_name);

  ParseBuffer (context.AttachTextSource (file_name),context,format,format_name);
}

void ParseDispatch::Parse (const char* name,const char* buf,size_t size,ParseContext& context,const char* format_name)
{
  if (!buf)
  {
    context.LogFatalError ("Null buffer");
    return;
  }

  ParseBuffer (context.AttachTextSource (name,buf,size),context,GetParseFormat (format_name),format_name);
}
