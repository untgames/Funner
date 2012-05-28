#include "shared.h"

using namespace plarium::utility;

int main ()
{
  printf ("Results of sha256_test:\n");

  try
  {
    unsigned char hash [32];

    sha256 ("", 0, hash);

    printf ("Hash: ");

    for (size_t i = 0; i < 32; i++)
      printf ("%02x", hash [i]);

    printf ("\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
