#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

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
   Чтение цвета из строки вида #ffffff
*/

bool XflParser::ReadHexColor (const char* hex_string, math::vec3f& color)
{
  unsigned int int_value = 0;

  if (*hex_string != '#')
    return false;

  if (!xtl::io::read (hex_string + 1, int_value))
    return false;

  color.x = int_value / 255 / 255;
  color.y = int_value / 255 % 255;
  color.z = int_value % 255;

  return true;
}

/*
   Чтение флоата из узла вида "0, value"
*/

float XflParser::ReadFloatFromVec2 (Parser::Iterator iter, const char* property_name)
{
  const char* vec_string = get<const char*> (*iter, property_name);
  float       return_value;

  if (sscanf (vec_string, "0,%f", &return_value) != 1)
    raise_parser_exception (*iter, "Wrong float value format, must be '0, value'");

  return return_value;
}
