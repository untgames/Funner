#include "shared.h"

int main ()
{
  printf ("Results of material2_test:\n");
  
  TestMaterial::Pointer material = TestMaterial::Create ();
  
  material->Rename ("Material1");
  
  printf ("Material '%s'\n", material->Name ());
  
  return 0;
}
