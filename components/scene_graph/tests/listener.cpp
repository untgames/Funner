#include "shared.h"

int main ()
{
  printf ("Results of listener_test:\n");
  
  Listener::Pointer listener (Listener::Create ());

  listener->SetGain (0.4f);
  printf ("Listener gain = %f\n", listener->Gain ());  

  return 0;
}
