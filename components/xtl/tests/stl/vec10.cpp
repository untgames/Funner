#include <cstdio>

#include <stl/vector>


class Test
{
  public:
    Test (int in_id) : ref_count (1), id (in_id) {}

    void AddRef () { ref_count++; }

    void Release () 
    {
      if (!--ref_count)
        delete this;
    }

    int Id () { return id; }

  private:
    Test (const Test&);

    Test& operator = (const Test&);

  private:
    size_t ref_count;
    int    id;
};

class SharedPointer
{
  public:
    SharedPointer (Test* in_test)
      : test (in_test)
    {
      test->AddRef ();
    }

    SharedPointer (const SharedPointer& source)
    {
      test = source.test;
      test->AddRef ();
    }

    ~SharedPointer ()
    {
      test->Release ();
    }

    SharedPointer& operator = (const SharedPointer& source)
    {
      SharedPointer tmp (source);

      stl::swap (tmp.test, test);

      return *this;
    }

    Test* get () { return test; }

  private:
    Test* test;
};

int main ()
{
  printf ("Results of vec10_test:\n");

  stl::vector<SharedPointer> tests;

  SharedPointer test1 = new Test (1), test2 = new Test (2);

  tests.reserve (4);

  tests.push_back (test1);
  tests.insert (tests.begin (), test2);

  printf ("%d\n", tests[0].get ()->Id ());
  printf ("%d\n", tests[1].get ()->Id ());

  return 0;
}