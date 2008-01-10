#include "test.h"

struct A: public trackable
{
};

void p3_handler ()
{
  printf ("p3 changed\n");
}

void p4_handler ()
{
  printf ("p4 changed\n");
}

int main ()
{
  printf ("Results of trackable_ptr1_test:\n");
  
  typedef trackable_ptr<int> int_ptr;
  typedef trackable_ptr<A>   object_ptr;
  
  int x = 0;
  int_ptr p1 = &x, p2 = p1;

  object_ptr p3 = new A, p4 = p3;
  
  p3.connect (&p3_handler);
  p4.connect (&p4_handler);
  
  printf ("p3:  %s\n", p3 ? "not null" : "null");
  printf ("!p3: %s\n", !p3 ? "true" : "false");
  
  printf ("delete p3, p4\n");
  
  delete p3.get ();

  printf ("p4:  %s\n", p4 ? "not null" : "null");
  printf ("!p4: %s\n", !p4 ? "true" : "false");
  
  printf ("new p4\n");
  
  p4 = new A;
  
  printf ("p3:  %s\n", p3 ? "not null" : "null");
  printf ("!p3: %s\n", !p3 ? "true" : "false");  
  printf ("p4:  %s\n", p4 ? "not null" : "null");
  printf ("!p4: %s\n", !p4 ? "true" : "false");
  
  p3 = p4;
  
  printf ("delete p4, p3\n");
  
  delete p4.get ();

  return 0;
}
