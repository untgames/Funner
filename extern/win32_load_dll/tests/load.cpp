#include <stdio.h>

#define CUSTOM_LOADING

#ifdef CUSTOM_LOADING
#include <load_dll.h>
#else
#include <windows.h>
#endif

char* DLL_FILE = DLL_PATH; //compiler define

const char* ENTRY_NAME = "sampleFn";

int main ()
{
  printf ("Results of load_test:\n");

#ifdef CUSTOM_LOADING
  LOAD_DLL_INFO info;

  memset (&info, 0, sizeof (info));

  DWORD result = LoadDLLFromFile (DLL_FILE, 0, DLL_SIZE_UNK, 0, &info);

  if (result)
  {
    printf ("Can't load dll '%s'\n", DLL_FILE);
    return 1;
  }

  typedef void (*Fn)();

  Fn fn = (Fn)MyGetProcAddress (&info, ENTRY_NAME);

  if (!fn)
  {
    printf ("Can't find '%s' in '%s'\n", ENTRY_NAME, DLL_FILE);
    return 1;
  }

  fflush (stdout);

  fn ();

  UnloadDLL (&info, 0);
#else

  HMODULE lib = LoadLibrary (DLL_FILE);

  if (!lib)
  {
    printf ("Can't load dll '%s'\n", DLL_FILE);
    return 1;
  }

  typedef void (*Fn)();

  Fn fn = (Fn)GetProcAddress (lib, ENTRY_NAME);

  if (!fn)
  {
    printf ("Can't find '%s' in '%s'\n", ENTRY_NAME, DLL_FILE);
    return 1;
  }

  fn ();

#endif

  printf ("end of test\n");

  return 0;
}
