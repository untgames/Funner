#include "shared.h"

Thread thread [2];

int thread2_run ()
{
  printf ("thread2 started\n");

  thread [0].Cancel ();
  
  return 0;
}

int thread1_run ()
{
  printf ("thread1 started\n");
  
  printf ("enter critical section\n");
  
  CriticalSection::Enter ();
  
  printf ("enter critical section (again)\n");  
  
  CriticalSection::Enter (); //recursive init
  
  thread [1] = Thread (&thread2_run);
  
  size_t start = milliseconds ();

  while (milliseconds () - start < 1000);
  
  printf ("exit critical section\n");
  
  CriticalSection::Exit ();
  
  start = milliseconds ();

  while (milliseconds () - start < 1000);  
  
  printf ("exit critical section (again)\n");
  
  CriticalSection::Exit ();  
  
  for (;;);    

  return 12;
}

int main ()
{
  printf ("Results of critical_section_test:\n");

  try    
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    thread [0] = Thread (&thread1_run);

    printf ("join thread2: %d\n", thread [1].Join ());
    printf ("join thread1: %d\n", thread [0].Join ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
