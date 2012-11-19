#include "shared.h"

namespace social
{

namespace facebook
{

//replace percent escapes using utf-8 encoding
stl::string replace_percent_escapes (const char* str)
{
  if (!str)
    throw xtl::make_null_argument_exception ("social::facebook::replace_percent_escapes", "str");

  stl::string return_value;

  return_value.resize (xtl::xstrlen (str));

  size_t current_out_symbols = 0;

  for (; *str; str++)
  {
    if ((*str == '%') && isxdigit (str [1]) && isxdigit (str [2]))
    {
      /* this is two hexadecimal digits following a '%' */
      char hexstr [3] = { str [1], str [2], 0 };

      ((unsigned char*)return_value.data ()) [current_out_symbols] = strtoul(hexstr, 0, 16) & 0xff;

      str += 2;
    }
    else
      return_value [current_out_symbols] = *str;

    current_out_symbols++;
  }

  return_value.resize (current_out_symbols);

  return return_value;
}

//return parameter from url, or empty string if not found
stl::string get_url_parameter (const char* url, const char* param_name)
{
  static const char* METHOD_NAME = "social::facebook::get_url_parameter";

  if (!url)
    throw xtl::make_null_argument_exception (METHOD_NAME, "url");

  if (!param_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "param_name");

  const char* param_start = strstr (url, param_name);

  if (!param_start)
    return "";

  char previous_char = '?';

  if (param_start != url)
    previous_char = param_start [-1];

  if (previous_char != '?' && previous_char != '&' && previous_char != '#')  //this is not param, but partial match
    return get_url_parameter (param_start + 1, param_name);

  param_start = param_start + xtl::xstrlen (param_name);

  const char* param_end = strstr (param_start, "&");

  if (!param_end) //last param
    param_end = param_start + xtl::xstrlen (param_start);

  return replace_percent_escapes (stl::string (param_start, param_end).c_str ());
}

}

}
