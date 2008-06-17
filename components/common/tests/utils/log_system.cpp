#include <cstdio>

#include <xtl/function.h>
#include <xtl/connection.h>

#include <common/log.h>

using namespace common;

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
  
  LogSystem::RegisterLogHandler ("*critical*", &critical_log_handler);
  LogSystem::RegisterLogHandler ("*", &all_log_handler);
  
  LogSystem::Print ("file_system_critical", "Can't acquire file system access");
  LogSystem::Printf ("critical_system_log", "System fail at %u::%u", 17, 15);
  LogSystem::Printf ("trivial_log", "System start...");

  return 0;
}
