#include <common/parser.h>

using namespace common;

int main ()
{
  printf ("Results of log_test:\n");
  
  ParseLog log;
  
  for (size_t i=0;i<10;i++)
  {
    log.Error   ("filename",i,"Hello world!");
    log.Warning ("filename",i,"Hello world!");
  }
  
  log.FatalError ("Hello world!");
  
  printf ("Parse log:\n");
  
  for (size_t i=0;i<log.MessagesCount ();i++)
    printf ("%s\n",log.Message (i));

  return 0;
}
