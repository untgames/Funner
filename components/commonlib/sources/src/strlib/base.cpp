#include <common/strlib.h>
#include <string.h>
#include <stdio.h>

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

inline vector<string> split 
 (const unsigned char*   s,
  const unsigned char*   delimiters,
  const unsigned char*   spaces,
  string::allocator_type allocator)
{
  vector<string> res (allocator);
  
  if (!*s) //частный случай для пустой строки
    return res;
  
  res.reserve (8);
  
  unsigned int brk_map [8], space_map [8];
      
  memset (brk_map,0,sizeof (brk_map));
  memset (space_map,0,sizeof (space_map));
  
  for (;*delimiters;delimiters++) brk_map   [*delimiters>>5] |= 1 << (*delimiters & 31);
  for (;*spaces;spaces++)         space_map [*spaces>>5]     |= 1 << (*spaces & 31);
  
  brk_map [0] |= 1; //'/0' is a break symbol
  
  --s;
        
  do
  {       
    for (++s;space_map [*s>>5]&(1<<(*s&31));s++); //cut leading spaces
    
    const unsigned char* first = s;
    
    for (;!(brk_map [*s>>5]&(1<<(*s&31)));s++);
        
    const unsigned char* last = s;
    
    if (last != first)
    {
      for (--last;space_map [*last>>5]&(1<<(*last&31));last--); //cut trailing spaces
      ++last;
    }
    else if (!*s) break; //avoid last empty token
         
    res.push_back (string ((string::value_type*)first,last-first,allocator));   
    
  } while (*s);
  
  return res;
}

stl::vector<stl::string> split (const char* str,const char* delimiters,const char* spaces)
{
  return split ((const unsigned char*)str,(const unsigned char*)delimiters,(const unsigned char*)spaces,string::allocator_type ());
}

stl::vector<stl::string> split (const string& str,const char* delimiters,const char* spaces)
{
  return split ((const unsigned char*)str.c_str (),(const unsigned char*)delimiters,(const unsigned char*)spaces,str.get_allocator ());
}

}
