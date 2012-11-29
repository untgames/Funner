#include "shared.h"

namespace
{

/* Portable character check (remember EBCDIC). Do not use isalnum() because
   its behavior is altered by the current locale.
   See http://tools.ietf.org/html/rfc3986#section-2.3
*/

bool is_unreserved (unsigned char in)
{
  switch (in) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '-': case '.': case '_': case '~': case ':': case '/':
    case '?': case '&': case '=':
      return true;
    default:
      break;
  }
  return false;
}

}

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

//make percent escaped string using utf-8 encoding
stl::string percent_escape (const char* str)
{
  if (!str)
    throw xtl::make_null_argument_exception ("social::facebook::percent_escape", "str");

  stl::string return_value;

  size_t result_length = 0;

  for (const unsigned char* current_symbol = (unsigned char*)str; *current_symbol; current_symbol++, result_length++)
    if (!is_unreserved (*current_symbol))
      result_length += 2;

  return_value.resize (result_length);

  size_t current_out_symbols = 0;

  for (const unsigned char* current_symbol = (unsigned char*)str; *current_symbol; current_symbol++)
  {
    if (is_unreserved (*current_symbol))
      return_value [current_out_symbols++] = *current_symbol;
    else
    {
      xtl::xsnprintf ((char*)return_value.data () + current_out_symbols, 4, "%%%02X", *current_symbol);

      current_out_symbols += 3;
    }
  }

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

//get first attribute value for tag with name if it is present
const char* get_named_attribute (common::ParseNode node, const char* name, const char* default_value)
{
  if (!name)
    throw xtl::make_null_argument_exception ("social::facebook::get_named_attribute", "name");

  common::ParseNode attribute_node = node.First (name);

  if (attribute_node && attribute_node.AttributesCount ())
    return attribute_node.Attribute (0);

  return default_value;
}

//parse user
User parse_user (common::ParseNode node)
{
  User return_value;

  return_value.SetId       (node.First ("id").Attribute (0));
  return_value.SetNickname (get_named_attribute (node, "username", ""));

  const char *first_name = get_named_attribute (node, "first_name", 0),
             *last_name  = get_named_attribute (node, "last_name", 0),
             *gender     = get_named_attribute (node, "gender", 0),
             *birthday   = get_named_attribute (node, "birthday", 0);

  common::PropertyMap& properties = return_value.Properties ();

  if (first_name)
    properties.SetProperty ("first_name", first_name);
  if (last_name)
    properties.SetProperty ("last_name", last_name);
  if (gender)
    properties.SetProperty ("gender", gender);
  if (birthday)
    properties.SetProperty ("birthday", birthday);

  common::ParseNode picture = node.First ("picture..data..url");

  if (picture && picture.AttributesCount ())
    properties.SetProperty ("picture", picture.Attribute (0));

  return return_value;
}

}

}
