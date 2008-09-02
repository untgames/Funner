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
  printf ("Results of log_system2_test:\n");
  
  try
  {
    LogFilter filter1 ("*critical*", &critical_log_handler),
              filter2 ("*", &all_log_handler);

    Log file_system_critical_log ("file_system_critical");

    file_system_critical_log.Print ("Can't acquire file system access");

    Log file_system_critical_log2 ("");

    file_system_critical_log2 = file_system_critical_log;

    file_system_critical_log2.Print ("Can't acquire file system access again");

    Log file_system_critical_log3 (file_system_critical_log);

    file_system_critical_log3.Print ("Can't acquire file system access and again");

    Log trivial_log ("");

    trivial_log.Printf ("System fail at %u::%u", 17, 15);
    trivial_log.Printf ("System start...");    
  }
  catch (xtl::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }  

  return 0;
}
