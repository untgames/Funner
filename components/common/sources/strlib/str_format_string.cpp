#include <common/strlib.h>
#include <xtl/function.h>

namespace
{

typedef xtl::function<void (const char* prefix, const char* replacement_tag)> NewTokenHandler;

/*
    Утилиты
*/

void parse_replacement (const char*& s, stl::string& replacement_tag)
{
  const char* end = s;

  replacement_tag.clear ();

  for (;;end++)
  {
    switch (*end)
    {
      case '}':
        replacement_tag.append (s+1, end);
        s = end;
        return;
      case '\0':
        return;
    }
  }
}

}

namespace common
{

/*
    Разбор форматированных строк
*/

void parse_format_string (const char* format_string, const xtl::function<void (const char* prefix, const char* replacement_tag)>& fn)
{
  const char* prefix_begin = format_string, *s = format_string;
  
  stl::string current_prefix, replacement_tag;

  current_prefix.reserve (strlen (format_string));

  for (;;s++)
  {
    switch (*s)
    {
      case '\0':
        if (prefix_begin != s)
        {
          current_prefix.append (prefix_begin, s);
          
          fn (current_prefix.c_str (), "");
        }

        return;
      case '{':
      {
        if (s [1] == '{')
        {
          current_prefix.append (prefix_begin, s + 1);

          prefix_begin = s + 1;

          break;
        }

        const char* prefix_end = s;

        parse_replacement (s, replacement_tag);

        current_prefix.append (prefix_begin, prefix_end);
        
        fn (current_prefix.c_str (), replacement_tag.c_str ());

        prefix_begin = s+1;
        
        current_prefix.clear ();

        break;
      }
    }
  }
}

}
