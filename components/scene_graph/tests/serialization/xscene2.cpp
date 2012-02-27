#include "shared.h"

void print_log (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of xscene2_test:\n");

  try
  {
    SceneManager manager;
    
    manager.LoadScene ("data/test2.xscene", "", &print_log);
    
    Node::Pointer node = manager.CreateScene ("scene1");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
