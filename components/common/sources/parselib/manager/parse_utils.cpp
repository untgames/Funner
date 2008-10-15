#include "shared.h"

namespace common
{

/*
    Генерация исключения парсера
*/

void raise_parser_exception (const ParseNode& node, const char* format, ...)
{
  static const char* METHOD_NAME = "common::raise_parser_exception";

  if (!format)
    throw xtl::make_null_argument_exception (METHOD_NAME, "format");
    
  va_list args;

  va_start (args, format);    

  node.Log ().VError (node, format, args);

  stl::string message = vformat (format, args);

  throw xtl::format_exception<ParserException> (METHOD_NAME, "%s(%u): node '%s': %s", node.Source (), node.LineNumber (),
    node.Name (), message.c_str ());
}

}
