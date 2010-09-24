#include "shared.h"

const char* f (float x, const char* y, int z, size_t w)
{
  printf ("f(%g,%s,%d,%u)\n", x, y, z, w);

  return "all ok";
}

int main ()
{
  printf ("Results of signature_test:\n");

  try
  {
    Invoker f1 = make_invoker (&f);
    
    printf ("signature: %s\n", f1.OverloadSignature (0).ToString ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
