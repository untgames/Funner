#include <common/strlib.h>
#include <string.h>
#include <stdio.h>

#include <stl/bitset>

using namespace stl;

namespace common
{

/*
    Функции работы с именами путей
*/

inline string basename (const char* src,size_t len,string::allocator_type allocator)
{
  for (const char* s=src+len;len--;)
    if (*--s == '.')
      return string (src,s-src,allocator);
  
  return string (src,allocator);
}

inline string suffix (const char* src,size_t len,string::allocator_type allocator)
{
  for (const char* s=src+len;s!=src;)
    if (*--s == '.')
      return string (s,len-(s-src),allocator);
  
  return string ("",allocator);
}

inline string dir (const char* src,size_t len,string::allocator_type allocator)
{
  for (const char* s=src+len;len--;)
    if (*--s == '/')
      return string (src,s-src+1,allocator);
  
  return string ("./",allocator);
}

inline string notdir (const char* src,size_t len,string::allocator_type allocator)
{
  for (const char* s=src+len;s!=src;)
    if (*--s == '/')
      return string (s+1,len-(s-src)-1,allocator);
  
  return string (src,allocator);
}

string basename (const string& s)
{
  return basename (s.c_str (),s.size (),s.get_allocator ());
}

string basename (const char* s)
{
  return basename (s,strlen (s),string::allocator_type ());
}

string suffix (const string& s)
{
  return suffix (s.c_str (),s.size (),s.get_allocator ());
}

string suffix (const char* s)
{
  return suffix (s,strlen (s),string::allocator_type ());
}

string dir (const string& s)
{
  return dir (s.c_str (),s.size (),s.get_allocator ());
}

string dir (const char* s)
{
  return dir (s,strlen (s),string::allocator_type ());
}

string notdir (const string& s)
{
  return notdir (s.c_str (),s.size (),s.get_allocator ());
}

string notdir (const char* s)
{
  return notdir (s,strlen (s),string::allocator_type ());
}

/*
    Compress / decompress
      Реализация функций основана на коде glib
*/

inline string compress (const char* src,size_t len,string::allocator_type allocator)
{
  string res (allocator);
  string::iterator dst;
  
  res.fast_resize (len);

  for (dst=res.begin ();len--;src++,dst++)
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

inline string decompress (const char* src,size_t len,const char* exceptions,string::allocator_type allocator)
{
  string res (allocator);   
  string::iterator dst;  
  
  res.fast_resize (len*4); //len * 4 - maximum new string size
  
  unsigned char excmap [256];
  
  memset (excmap,0,sizeof (excmap));  
  
  if (exceptions)
    for (const unsigned char* i=(const unsigned char*)exceptions;*i;excmap [*i++]=1);

  for (dst=res.begin ();len--;src++,dst++)
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
  return compress (s,strlen (s),string::allocator_type ());
}

string compress (const string& s)
{
  return compress (s.c_str (),s.size (),s.get_allocator ());
}

string decompress (const char* s,const char* exceptions)
{
  return decompress (s,strlen (s),exceptions,string::allocator_type ());
}

string decompress (const string& s,const char* exceptions)
{
  return decompress (s.c_str (),s.size (),exceptions,s.get_allocator ());
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
      for (; *delimiters; delimiters++) delimiters_map.set ((unsigned char)*delimiters);
      for (; *spaces; spaces++)         space_map.set ((unsigned char)*spaces);

      for (; brackets [0] && brackets [1]; brackets += 2)
      {
        open_brackets_map.set ((unsigned char)brackets [0]);
        close_brackets_map.set ((unsigned char)brackets [1]);
      }

      delimiters_map.set ('\0');
      close_brackets_map.set ('\0');
    }
    
    typedef stl::pair<const char*, const char*> Word;
    
    Word NextWord ()
    {
      if (!*pos)
        return Word ((const char*)pos, (const char*)pos);

      for (; space_map [*pos]; pos++); //cut leading spaces
      
      const unsigned char *first, *last;
      
      if (open_brackets_map [*pos]) //found open bracket
      {
        first = ++pos;

        for (; !close_brackets_map [*pos]; pos++);

        last = pos;

        if (pos [0] && delimiters_map [pos [1]])
          ++pos;
      }
      else
      {
        first = pos;
        
        for (; !delimiters_map [*pos]; pos++);
        
        last = pos;
      }      

      if (*pos)
        pos++;

      if (last != first)
      {
        for (--last; space_map [*last]; last--); //cut trailing spaces

        ++last;
      }

      return Word ((const char*)first, (const char*)last);
    }

    bool EndOfString () const { return *pos == '\0'; }

  private:
    bitset<256>    delimiters_map, space_map, open_brackets_map, close_brackets_map;
    unsigned char* pos;  
};

inline string word (const char* str, size_t word_index, const char* delimiters, const char* spaces, const char* brackets, string::allocator_type allocator)
{
  if (!*str)
    return "";

  WordParser parser (str, delimiters, spaces, brackets);
  
  WordParser::Word word;

  for (size_t i=0; i<=word_index; i++)
    word = parser.NextWord ();

  return string (word.first, word.second - word.first, allocator);
}

inline void split (const char* str, const char* delimiters, const char* spaces, const char* brackets, string::allocator_type allocator, StringArray& res)
{
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

}
