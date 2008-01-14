#include <common/strlib.h>
#include <xtl/function.h>

namespace
{

typedef xtl::function<void (const char* property, const char* value)> NewPropertyHandler;

/*
    Утилиты
*/

//пропуск пробелов
void skip_whitespace (const char*& pos)
{
  for (const char* s=pos;;)
    switch (*s)
    {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        s++;
        break;
      default:
        pos = s;
        return;
    }
}

//разбор элемента
void parse_element (const char*& pos, stl::string& result)
{
  skip_whitespace (pos);

  const char* s = pos; 
  
  if (*s == '\'' || *s == '"')
  {
    const char scope = *s++, *first = s; 

    for (;;s++)
      switch (*s)
      {
        case '\'':
        case '"':
          if (*s != scope)
            continue;

          pos = ++s;
        
          result.assign (first, pos-first-1);

          return;
        case '\0':
          pos = s;
        
          result.assign (first, s-first);

          return;        
        default:
          break;
      }
  }
  else
  {
    for (const char* first=s;;s++)
      switch (*s)
      {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\0':
        case '\'':
        case '"':        
        case '=':
          pos = s;        
        
          result.assign (first, s-first);

          return;
        default:
          break;
      }
  }  
}

}

namespace common
{

/*
    Разбор строк инициализации (property1=value property2='string value')
*/

void parse_init_string (const char* init_string, const NewPropertyHandler& fn)
{
  stl::string tag, value;

  for (const char* pos=init_string; *pos;)
  {
    parse_element (pos, tag);  

    skip_whitespace (pos);
    
    if (*pos != '=')
      continue;      

    parse_element (++pos, value);

    fn (tag.c_str (), value.c_str ());
  }
}

}
