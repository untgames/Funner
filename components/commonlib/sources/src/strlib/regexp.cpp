#include <common/strlib.h>
#include <locale.h>
#include <pcre.h>

using namespace stl;

namespace common
{

const int MAX_TOKENS = 32; //максимальнео количество токенов при разбиении

static const char*          loc_name  = NULL;
static const unsigned char* loc_tables = NULL;

static int _parse (const char* str,const char* pattern,int* offset_buf=0,size_t max_buf_size=0)
{
    //opt!!!
  const char* cur_locale = setlocale (LC_COLLATE,NULL);

  if (cur_locale != loc_name)
  {
    loc_tables = pcre_maketables ();
    loc_name   = cur_locale;
  }

  const char* error = "";
  int err_offset = -1;
    
  pcre *re = pcre_compile (pattern,0,&error,&err_offset,loc_tables);

  if (!re)
    return -1;
  
  return pcre_exec (re,NULL,str,strlen (str),0,0,offset_buf,max_buf_size);
}

vector<string> parse (const char* str,const char* pattern)
{  
  int offset [MAX_TOKENS*3]; //требование библиотеки  

  int tokens_count = _parse (str,pattern,offset,MAX_TOKENS*3);

  vector<string> res;

  if (tokens_count <= 0)
    return res;    

  res.reserve (tokens_count);

  for (int i=1;i<tokens_count;i++)
    res.push_back (string (str+offset [2*i],str+offset [2*i+1]));

  return res;
}

string replace (const char* str,const char* pattern,const char* replacement)
{
  int offset [MAX_TOKENS*3]; //требование библиотеки
  int tokens_count = _parse (str,pattern,offset,MAX_TOKENS*3);

  if (tokens_count <= 0)
    return string ();

  string res;
  
  res.fast_resize (strlen (str)+strlen (replacement));

  string::iterator dst = res.begin ();
  
  for (const char* src=replacement;;src++,dst++)
    switch (*src)
    {
      case '\0':  return *dst = 0, res;
      case '\\':
        switch (*++src)
        {
          case '\0': return *dst = 0, res;
          case '0': case '1': case '2': case '3': case '4': 
          case '5': case '6': case '7': case '8': case '9':
          {                       
            int index = *src-'0';
        
            for (;isdigit (*++src);index=index*10+(*src-'0'));

            src--;
            
            if (index < tokens_count)
            {
              int first = offset [2*index], len = offset [2*index+1]-first;

              memcpy (dst,str+first,len);
              dst += len-1;
            }
            
            break;
          }
          case 'b': *dst = '\b'; break;
          case 'f': *dst = '\f'; break;
          case 'n': *dst = '\n'; break;
          case 'r': *dst = '\r'; break;
          case 't': *dst = '\t'; break;
          default:  *dst = *src; break;
        }
        break;  
      default:      *dst = *src; break;
    }
}

bool rematch (const char* string,const char* pattern)
{
  return _parse (string,pattern) != 0;
}

}
