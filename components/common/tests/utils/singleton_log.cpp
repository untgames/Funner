#include <cstdio>
#include <common/singleton.h>

using namespace common;

void print_log (SingletonLog::Event event, const std::type_info& type)
{
  const char* event_message = "";

  switch (event)
  {
    case SingletonLog::Event_BeforeCreate:      event_message = "before create"; break;
    case SingletonLog::Event_AfterCreate:       event_message = "after create"; break;
    case SingletonLog::Event_BeforeDestroy:     event_message = "before destroy"; break;
    case SingletonLog::Event_AfterDestroy:      event_message = "after destroy"; break;
    case SingletonLog::Event_AfterServiceInit:  event_message = "after service init"; break;
    case SingletonLog::Event_BeforeServiceDone: event_message = "before service done"; break;    
    default: break; //for gcc
  }

  printf ("%s '%s'\n", event_message, type.name ());
}

struct My {};

typedef Singleton<My> MySingleton;

int main ()
{
  printf ("Results of singleton_log_test:\n");    
  
  SingletonLog::SetLogHandler (&print_log);
  
  MySingleton::Instance ();
  
  return 0;
}
