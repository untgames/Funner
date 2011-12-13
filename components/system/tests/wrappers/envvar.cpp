#include "shared.h"

void check (const char* name)
{
  try
  {
    printf ("Variable '%s': ", name);
//    printf ("'%s'\n", Application::GetEnvironmentVariable (name).c_str ());

    Application::GetEnvironmentVariable (name);

    printf ("found\n");    
  }
//  catch (std::exception& exception)
//  {
//    printf ("exception: %s\n", exception.what ());
//  }
  catch (...)
  {
    printf ("not found\n");
  }
}

int main ()
{
  printf ("Results of envvar_test:\n");

  check ("PATH");
  check ("NONEXISTING_VAR");

  return 0;
}
