#include <stdio.h>
#include <common/strlib.h>
#include <time.h>

using namespace common;

const size_t TOTAL = 100000;

int main ()
{
  printf ("Results of re-speed-test:\n");  

  const char* pattern = "(hello) ([[:lower:]]+)";
  const char* str     = "hello world";

  parse (str,pattern);

  clock_t start = clock ();
  
  for (size_t i=0;i<TOTAL;i++)  
    parse (str,pattern);

  clock_t end = clock ();

  printf ("cycles: %u\n",TOTAL);
  printf ("time:   %.2f\n",float (end-start)/float (CLOCKS_PER_SEC));
  printf ("speed:  %.2f\n",float (TOTAL)/float (end-start)*float (CLOCKS_PER_SEC));

  return 0;
}
