#include <cstdio>
#include <common/singleton.h>

using namespace common;

struct My {};

typedef Singleton<My> MySingleton;

int main ()
{
  printf ("Results of singleton_log_test:\n");    

  SingletonLog::SetLogHandler (&SingletonLog::DebugLogHandler);
  
  MySingleton::Instance ();
  
  return 0;
}
