#include "shared.h"

using namespace syslib;
using namespace syslib::android;

namespace
{

typedef void (*sighandler_t)(int);

sighandler_t prev_segv_handler = 0; //предыдущий обработчик сигнала SEGV

// дамп карты памяти
void dump_maps ()
{
  static char line [256];
  
  FILE *fp = fopen ("/proc/self/maps", "r");  
  
  if (!fp)
    return;

  while (fgets (line, sizeof (line), fp)) 
  {
    size_t len = strlen (line);
    
    len--;
    
    line [len] = 0;
    
    if (!strstr (line, "/system/") && !strstr (line, "/dev/") && !strstr (line, "[stack]") && line [len-1] != ' ')
      printf ("segment %s\n", line);
  }  
  
  fclose (fp);  
  fflush (stdout);
  
  Application::Sleep (1000);
}

// обработчик сигналогв
void sighandler (int signum)
{
  switch (signum)
  {
    case SIGSEGV:
      printf ("Crash detected\n");
      dump_maps ();
      prev_segv_handler (signum);
      break;
    default:
      break;
  }
}

}

namespace syslib
{

namespace android
{

/// регистрация обратчиков аварийного завершения
void register_crash_handlers ()
{
  prev_segv_handler = signal(SIGSEGV, sighandler);
}

}

}
