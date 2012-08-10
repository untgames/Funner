#include "shared.h"

void print_log (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of map_file1_test:\n");
  
  common::LogFilter filter ("debug.*", &print_log);
  
  try
  {
    MapFile file ("data/app.map", "gcc");
    
    for (MapFile::ConstIterator iter=file.CreateIterator (); iter; ++iter)
    {
      printf ("offset=0x%08x size=0x%08x name='%s'\n", iter->StartAddress (), iter->Size (), iter->Name ());
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
