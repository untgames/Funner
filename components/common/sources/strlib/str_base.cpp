#include <common/strlib.h>
#include <string.h>
#include <stdio.h>

#include <xtl/common_exceptions.h>

using namespace stl;

namespace common
{

/*
    Функции работы с именами путей
*/

inline string basename (const char* src, size_t len, string::allocator_type allocator)
{
  for (const char* s=src+len;len--;)
    if (*--s == '.')
      return string (src, s-src, allocator);

  return string (src, allocator);
}

inline string suffix (const char* src, size_t len, string::allocator_type allocator)
{
  for (const char *end=src+len, *s=end;s!=src;)
    if (*--s == '.')
      return string (s, end, allocator);

  return string ("", allocator);
}

inline string dir (const char* src, size_t len, string::allocator_type allocator)
{
  for (const char* s=src+len;len--;)
    if (*--s == '/')
      return string (src, s-src+1, allocator);

  return string ("./", allocator);
}

inline string notdir (const char* src, size_t len, string::allocator_type allocator)
{
  for (const char* s=src+len;s!=src;)
    if (*--s == '/')
      return string (s+1, len-(s-src)-1, allocator);

  return string (src, allocator);
}

string basename (const string& s)
{
  return basename (s.c_str (), s.size (), s.get_allocator ());
}

string basename (const char* s)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::basename", "s");

  return basename (s, strlen (s), string::allocator_type ());
}

string suffix (const string& s)
{
  return suffix (s.c_str (), s.size (), s.get_allocator ());
}

string suffix (const char* s)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::suffix", "s");

  return suffix (s, strlen (s), string::allocator_type ());
}

string dir (const string& s)
{
  return dir (s.c_str (), s.size (), s.get_allocator ());
}

string dir (const char* s)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::dir", "s");

  return dir (s, strlen (s), string::allocator_type ());
}

string notdir (const string& s)
{
  return notdir (s.c_str (), s.size (), s.get_allocator ());
}

string notdir (const char* s)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::notdir", "s");

  return notdir (s, strlen (s), string::allocator_type ());
}

/*
    Compress / decompress
      Реализация функций основана на коде glib
*/

inline string compress (const char* src, size_t len, string::allocator_type allocator)
{
  string res (allocator);
  string::iterator dst;

  res.fast_resize (len);

  for (dst=res.begin ();len--;src++, dst++)
  {
    switch (*src)
    {
      case '\\':
        switch (*++src)
        {
          case '\0': break;
          case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
          {
            *dst  = 0;
            for (const char* end=src+3;src != end && *src >= '0' && *src <= '7';src++)
              *dst = (*dst * 8) + (*src - '0');
            src--;
            break;
          }
          case 'b': *dst = '\b'; break;
          case 'f': *dst = '\f'; break;
          case 'n': *dst = '\n'; break;
          case 'r': *dst = '\r'; break;
          case 't': *dst = '\t'; break;
          default:
            *dst = *src;
            break;
        }
        break;
      default:
        *dst = *src;
        break;
    }
  }

  res.fast_resize (dst-res.begin ());

  return res;
}

inline string decompress (const char* src, size_t len, const char* exceptions, string::allocator_type allocator)
{
  string res (allocator);
  string::iterator dst;

  res.fast_resize (len*4); //len * 4 - maximum new string size

  unsigned char excmap [256];

  memset (excmap, 0, sizeof (excmap));

  if (exceptions)
    for (const unsigned char* i=(const unsigned char*)exceptions;*i;excmap [*i++]=1);

  for (dst=res.begin ();len--;src++, dst++)
  {
    if (excmap [(unsigned char)*src])
    {
      *dst = *src;
      continue;
    }

    switch (*src)
    {
      case '\b': *dst++ = '\\'; *dst = 'b';  break;
      case '\f': *dst++ = '\\'; *dst = 'f';  break;
      case '\n': *dst++ = '\\'; *dst = 'n';  break;
      case '\r': *dst++ = '\\'; *dst = 'r';  break;
      case '\t': *dst++ = '\\'; *dst = 't';  break;
      case '\\': *dst++ = '\\'; *dst = '\\'; break;
      case '"':  *dst++ = '\\'; *dst = '"';  break;
      default:
        if (*src < ' ' || *src >= 0177)
        {
          *dst++ = '\\';
          *dst++ = '0' + (((*src) >> 6) & 07);
          *dst++ = '0' + (((*src) >> 3) & 07);
          *dst++ = '0' + ((*src) & 07);
        }
        else *dst = *src;
    }
  }

  res.fast_resize (dst-res.begin ());

  return res;
}

string compress (const char* s)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::compress", "s");

  return compress (s, strlen (s), string::allocator_type ());
}

string compress (const string& s)
{
  return compress (s.c_str (), s.size (), s.get_allocator ());
}

string decompress (const char* s, const char* exceptions)
{
  if (!s)
    throw xtl::make_null_argument_exception ("common::decompress", "s");
    
  if (!s)
    throw xtl::make_null_argument_exception ("common::decompress", "exceptions");    

  return decompress (s, strlen (s), exceptions, string::allocator_type ());
}

string decompress (const string& s, const char* exceptions)
{
  return decompress (s.c_str (), s.size (), exceptions, s.get_allocator ());
}

/*
    Разбиение строки
*/

namespace
{

class WordParser
{
  public:
    WordParser (const char* string, const char* delimiters, const char* spaces, const char* brackets) : pos ((unsigned char*)string)
    {
      memset (delimiters_map, 0, sizeof (delimiters_map));
      memset (space_map, 0, sizeof (space_map));
      memset (open_brackets_map, 0, sizeof (open_brackets_map));
      memset (close_brackets_map, 0, sizeof (close_brackets_map));

      for (; *delimiters; delimiters++) delimiters_map [(unsigned char)*delimiters] = true;
      for (; *spaces; spaces++)         space_map [(unsigned char)*spaces] = true;

      for (; brackets [0] && brackets [1]; brackets += 2)
      {
        open_brackets_map [(unsigned char)brackets [0]] = true;
        close_brackets_map [(unsigned char)brackets [1]] = true;
      }

      delimiters_map [(unsigned char)'\0'] = true;
      close_brackets_map [(unsigned char)'\0'] = true;
    }

    typedef stl::pair<const char*, const char*> Word;

    Word NextWord ()
    {
      if (!*pos)
        return Word ((const char*)pos, (const char*)pos);

      for (; space_map [*pos]; pos++); //cut leading spaces

      const unsigned char *first, *last;
      bool word_in_brackets = false;

      if (open_brackets_map [*pos]) //found open bracket
      {
        first = ++pos;

        for (; !close_brackets_map [*pos]; pos++);

        last = pos;

        if (pos [0] && delimiters_map [pos [1]])
          ++pos;

        word_in_brackets = true;
      }
      else
      {
        first = pos;

        for (; !delimiters_map [*pos]; pos++);

        last = pos;
      }

      if (*pos)
        pos++;

      if (last != first && !word_in_brackets)
      {
        for (--last; space_map [*last]; last--); //cut trailing spaces

        ++last;
      }

      return Word ((const char*)first, (const char*)last);
    }

    bool EndOfString () const { return *pos == '\0'; }

  private:
    typedef bool BoolMap [256];

  private:
    BoolMap        delimiters_map, space_map, open_brackets_map, close_brackets_map;
    unsigned char* pos;
};

inline string word (const char* str, size_t word_index, const char* delimiters, const char* spaces, const char* brackets, string::allocator_type allocator)
{
  if (!*str)
    return "";
    
  if (!delimiters)
    throw xtl::make_null_argument_exception ("common::word", "delimiters");
    
  if (!spaces)
    throw xtl::make_null_argument_exception ("common::word", "spaces");    
    
  if (!brackets)
    throw xtl::make_null_argument_exception ("common::word", "brackets");

  WordParser parser (str, delimiters, spaces, brackets);

  WordParser::Word word;

  for (size_t i=0; i<=word_index; i++)
    word = parser.NextWord ();

  return string (word.first, word.second - word.first, allocator);
}

inline void split (const char* str, const char* delimiters, const char* spaces, const char* brackets, string::allocator_type allocator, StringArray& res)
{
  if (!str)
    throw xtl::make_null_argument_exception ("common::split", "str");

  if (!delimiters)
    throw xtl::make_null_argument_exception ("common::split", "delimiters");
    
  if (!spaces)
    throw xtl::make_null_argument_exception ("common::split", "spaces");    
    
  if (!str)
    throw xtl::make_null_argument_exception ("common::split", "brackets");    

  if (!*str) //частный случай для пустой строки
    return;

  res.Reserve (8);

  WordParser parser (str, delimiters, spaces, brackets);

  do
  {
    WordParser::Word word = parser.NextWord ();

    if (word.first != word.second || !parser.EndOfString ())
      res.Add (string (word.first, word.second - word.first, allocator).c_str ());

  } while (!parser.EndOfString ());
}

}

string word (const char* str, size_t word_index, const char* delimiters, const char* spaces, const char* brackets)
{
  return word (str, word_index, delimiters, spaces, brackets, string::allocator_type ());
}

string word (const string& str, size_t word_index, const char* delimiters, const char* spaces, const char* brackets)
{
  return word (str.c_str (), word_index, delimiters, spaces, brackets, str.get_allocator ());
}

StringArray split (const char* str, const char* delimiters, const char* spaces, const char* brackets)
{
  StringArray res;

  split (str, delimiters, spaces, brackets, string::allocator_type (), res);

  return res;
}

StringArray split (const string& str, const char* delimiters, const char* spaces, const char* brackets)
{
  StringArray res;

  split (str.c_str (), delimiters, spaces, brackets, str.get_allocator (), res);

  return res;
}

/*
    Отсечение пробелов
*/

inline stl::string strip (const char* str, size_t len, const char* spaces, string::allocator_type allocator)
{
  const char* end = str + len - 1;
  
  for (;end >= str; --end)
  {
    const char* s = spaces;
    
    for (; *s && *s != *end; s++);

    if (!*s)
      break;
  }
  
  return stl::string (str, end + 1, allocator);
}

stl::string strip (const char* str, const char* spaces)
{
  if (!str)
    throw xtl::make_null_argument_exception ("common::strip", "str");
    
  if (!spaces)
    throw xtl::make_null_argument_exception ("common::strip", "str");    

  return strip (str, strlen (str), spaces, string::allocator_type ());
}

inline stl::string trim (const char* str, size_t len, const char* spaces, string::allocator_type allocator)
{
  const char* end = str + len - 1;

  for (;end >= str; --end)
  {
    const char* s = spaces;
    
    for (; *s && *s != *end; s++);

    if (!*s)
      break;
  }
  
  const char* start = str;
  
  for (;end >= start; ++start)
  {
    const char* s = spaces;
    
    for (; *s && *s != *start; s++);

    if (!*s)
      break;
  }  
  
  return stl::string (start, end + 1, allocator);
}

stl::string trim (const char* str, const char* spaces)
{
  if (!str)
    throw xtl::make_null_argument_exception ("common::trim", "str");
    
  if (!spaces)
    throw xtl::make_null_argument_exception ("common::trim", "spaces");

  return trim (str, strlen (str), spaces, string::allocator_type ());
}

}
