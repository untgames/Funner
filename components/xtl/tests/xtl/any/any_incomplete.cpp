#include "shared.h"

class A
{
  public:
    virtual ~A () {}

  private:
    virtual void test () = 0;
};

class B: public A
{
  public:
    void test () { printf ("hello from B::test\n"); }
};

int main ()
{
  printf ("Results of any_incomplete_test:\n");

  try
  {
    B b;

    any a (b);

    any_multicast<B&> (a).test ();

    any_multicast<A&> (a);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
