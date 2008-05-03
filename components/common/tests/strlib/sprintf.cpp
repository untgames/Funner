#include <common/strwrap.h>
#include <stdio.h>
#include <wchar.h>

int main ()
{
  printf ("Results of sprintf_test:\n");
  
  char    char_buffer [9];
  wchar_t wchar_buffer [9];
  
  char_buffer [8] = 'A';
  wchar_buffer [8] = L'A';
  
  printf ("result='%s', len=%d\n",char_buffer,common::string_wrappers::snprintf (char_buffer,8,"Hello %s","world"));
  printf ("control: '%c'\n",char_buffer [8]);
  printf ("len=%d\n",common::string_wrappers::snprintf (0,0,"Hello %s","world"));
  printf ("result='%ls', len=%d\n",wchar_buffer,common::string_wrappers::snwprintf (wchar_buffer,8,L"Hello %ls",L"world"));
  printf ("control: '%lc'\n",wchar_buffer [8]);
  printf ("len=%d\n",common::string_wrappers::snwprintf (0,0,L"Hello %ls",L"world"));  

  return 0;
}
