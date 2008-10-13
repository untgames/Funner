#include "shared.h"

#if defined (_MSC_VER) || defined (__CYGWIN__)
  const char* LIBRARY_NAME = "kernel32.dll";
  const char* SYMBOL_NAME  = "LoadLibraryA";
#elif defined (__APPLE__)
  const char* LIBRARY_NAME = "kernel32.dll";
  const char* SYMBOL_NAME  = "LoadLibraryA";
#else
  #error Unknown platform
#endif

int main ()
{
  printf ("Results of dll_test:\n");

  try
  {
    DynamicLibrary library;

    library.Load (LIBRARY_NAME);

    printf ("Symbol %s\n", library.GetSymbol (SYMBOL_NAME) ? "found" : "not found");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
