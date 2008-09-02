#include "shared.h"

template <int index>
void print (const char* log_name, const char* message)
{
  printf ("  [print%d]#%s: %s\n", index, log_name, message);
}

int main ()
{
  printf ("Results of log_system3_test:\n");
  
  try
  {
    {
      LogFilter filter1 ("*critical*", &print<1>),
                filter2 ("*", &print<2>);

      {
        Log log ("log_critical");
        
        {
          printf ("Message\n");
          log.Print ("Test");
        
          LogFilter filter3 ("*", &print<3>);
        
          printf ("Message\n");
          log.Print ("Test");
        }
        
        printf ("Message\n");
        log.Print ("Test");
      }
    }
    
    {      
      LogFilter filter ("*_ok", &print<4>);

      Log log ("log_ok");
      
      printf ("Message\n");
      log.Print ("Test");
    }
  }
  catch (xtl::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }  

  return 0;
}
