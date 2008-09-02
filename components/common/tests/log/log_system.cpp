#include "shared.h"

void critical_log_handler (const char* log_name, const char* message)
{
  printf ("New log message received in 'critical' log handler from log '%s': '%s'\n", log_name, message);
}

void all_log_handler (const char* log_name, const char* message)
{
  printf ("New log message received in 'all' log handler from log '%s': '%s'\n", log_name, message);
}

int main ()
{
  printf ("Results of log_system_test:\n");
  
  try
  {
    LogFilter filter1 ("*critical*", &critical_log_handler),
              filter2 ("*", &all_log_handler);

    Log log1 ("file_system_critical"),
        log2 ("critical_system_log"),
        log3 ("trivial_log");
    
    log1.Print  ("Can't acquire file system access");
    log2.Printf ("System fail at %u::%u", 17, 15);
    log3.Printf ("System start...");
  }
  catch (std::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }  

  return 0;
}
