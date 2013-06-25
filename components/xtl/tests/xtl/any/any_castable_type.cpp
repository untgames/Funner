#include "shared.h"

class A
{
  public:
    virtual ~A () {}
};

class B: public A
{
};

int main ()
{
  printf ("Results of any_castable_type_test:\n");

  try
  {
    B b;

    any a (static_cast<A*> (&b));

    if (&typeid (B) != &a.castable_type ()) printf ("test failed: castable type is %s\n", a.castable_type ().name ());
    else                                    printf ("test passed\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
