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

  va_list args_copy;

  va_copy (args_copy, args);

  node.Log ().VError (node, format, args_copy);

  va_end (args_copy);

  stl::string message = vformat (format, args);

  va_end (args);

  throw xtl::format_exception<ParserException> (METHOD_NAME, "%s(%u): node '%s': %s", node.Source (), node.LineNumber (),
    node.Name (), message.c_str ());
}

}
