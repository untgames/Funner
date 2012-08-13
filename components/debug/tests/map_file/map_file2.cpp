#include "shared.h"

void print_log (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of map_file2_test:\n");
  
  common::LogFilter filter ("debug.*", &print_log);
  
  try
  {
    MapFile file ("data/app.map", "gcc");
    
    Symbol* s = file.FindSymbol (0xc790);
    
    if (!s) printf ("symbol not found\n");
    else    printf ("symbol '%s' found\n", s->Name ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
