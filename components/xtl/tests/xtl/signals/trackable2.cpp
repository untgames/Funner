#include "test.h"

template <int id> struct handler
{
  void operator () () const
  {
    printf ("handler%d\n", id);
  }
};

int main ()
{
  printf ("Results of trackable2_test:\n");
  
  connection c1;  
  
  trackable t1;

  t1.connect_tracker (handler<1> ());

  {
    trackable t2;
    
    t2.connect_tracker (handler<2> ());
    t2.connect_tracker (handler<21> (), t1);
    t1.connect_tracker (handler<12> (), t2);
    
    connection c2 = t2.connect_tracker (handler<0> (),  t1),
               c3 = t2.connect_tracker (handler<-1> (), t1),
               c4 = t2.connect_tracker (handler<-2> (), t1);
               
    c4.block ();
               
    c1 = c3;
    
    c2.disconnect ();
    
    printf ("end of t2\n");
  }
  
  printf ("end of t1 & c1\n");  

  return 0;
}
