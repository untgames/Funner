#include <common/strlib.h>
#include <locale.h>
#include <pcre.h>

using namespace stl;

namespace common
{

const int MAX_TOKENS = 128; //максимальное количество токенов при разбиении

static const char*          loc_name  = NULL;
static const unsigned char* loc_tables = NULL;

namespace
{

int parse_internal (const char* str,const char* pattern,const char* flags,int* offset_buf=0,size_t max_buf_size=0)
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
  
  bool find_all = false;
  
  for (;*flags; flags++)
    switch (*flags)
    {
      case 'g':
      case 'G':
        find_all = true;
        break;
      default:
        break;
    }
    
  pcre *re = pcre_compile (pattern,0,&error,&err_offset,loc_tables);

  if (!re)
    return -1;      
    
  int str_length  = strlen (str),
      matches     = pcre_exec (re,0,str,str_length,0,0,offset_buf,max_buf_size);
  
  if (matches <= 0)
    return 0;    
  
  if (!find_all)
    return matches;
    
    //поиск всех вхождений
    
  int* offset       = offset_buf;
  int  all_matches  = matches;
  
  for (;;)
  {
    int options      = 0,
        start_offset = offset [1];

    max_buf_size -= matches * 2;
    offset       += matches * 2;    

      //проверка ну пустоту предыдущего совпадения
      
    if (start_offset == offset [-2])
    {      
      if (start_offset == str_length)
        break;        

      options = PCRE_NOTEMPTY | PCRE_ANCHORED;
    }    

    matches = pcre_exec (re,0,str,str_length,start_offset,options,offset,max_buf_size);

    if (matches == PCRE_ERROR_NOMATCH)
    {
      if (options == 0)
        break;        

      offset [1] = start_offset + 1;
      matches    = 0;
      
      continue;
    }

    if (matches < 0)
      return all_matches;

    if (!matches)
      break;

    all_matches  += matches;
    start_offset  = offset [1];
  }

  return all_matches;
}

}

vector<string> parse (const char* str,const char* pattern,const char* flags)
{  
  int offset [MAX_TOKENS*3]; //требование библиотеки  

  int tokens_count = parse_internal (str,pattern,flags,offset,MAX_TOKENS*3);

  vector<string> res;

  if (tokens_count <= 0)
    return res;    

  res.reserve (tokens_count);  

//  for (int i=1;i<tokens_count;i++)
  for (int i=0;i<tokens_count;i++)
    res.push_back (string (str+offset [2*i],str+offset [2*i+1]));    

  return res;
}

string replace (const char* str,const char* pattern,const char* replacement,const char* flags)
{
  int offset [MAX_TOKENS*3]; //требование библиотеки
  int tokens_count = parse_internal (str,pattern,flags,offset,MAX_TOKENS*3);

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

bool rematch (const char* string,const char* pattern,const char* flags)
{
  return parse_internal (string,pattern,flags) != 0;
}

}
