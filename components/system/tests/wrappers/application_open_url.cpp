#include "shared.h"

int main ()
{
  printf ("Results of application_open_url:\n");
  
  try
  {
    Application::OpenUrl ("http://www.microsoft.com");
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
