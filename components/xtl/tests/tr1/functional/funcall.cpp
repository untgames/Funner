#include "test.h"

struct A
{
  A () { id = 1; }

  void f1 (int x, const char* y, float& z)
  {
    printf ("A::f1: x=%d y='%s' z='%g\n", x, y, z);
  }  
  
  void f2 (int x, const char* y, float& z) const
  {
    printf ("A::f2 const: x=%d y='%s' z='%g\n", x, y, z);
  }    
  
  int id;
};

struct B: A
{
};

void f1 (int x, const char* y, float& z)
{
  printf ("f1: x=%d y='%s' z='%g\n", x, y, z);
}

int main ()
{
  printf ("Results of funcall_test:\n");
  
  float z = 3;
  
  funcall (&f1, 1, "Hello world", z);
  
  A a1;
  const A a2;
  const B b;
  
  funcall (&A::f1, ref (a1), 1, "Hello world", z);
  funcall (&A::f1, &a1, 1, "Hello world", z);  
  funcall (&A::f2, ref (a2), 1, "Hello world", z);
  funcall (&A::f2, &a2, 1, "Hello world", z);
  
  printf ("Memptr funcall %u\n", funcall (&A::id, ref (b)));
  printf ("Memptr funcall %u\n", funcall (&A::id, &b));

  return 0;
}
