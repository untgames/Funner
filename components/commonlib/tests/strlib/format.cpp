#include <common/strlib.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of format_test:\n");
  
  printf ("%s\n",format ("string::format %s, x=%c, y=%d, pi=%.4f","test",'A',12,3.1415f).c_str ());
  printf ("%ls\n",format (L"wstring::format %ls, x=%lc, y=%d, pi=%.4f",L"test",L'A',12,3.1415f).c_str ());

  return 0;
}
