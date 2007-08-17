#include "shared.h"

typedef com_ptr<Listener> ListenerPtr;

int main ()
{
  printf ("Results of listener_test:\n");
  
  ListenerPtr listener (Listener::Create (), false);

  listener->SetGain (0.4f);
  printf ("Listener gain = %f\n", listener->Gain ());  

  return 0;
}
