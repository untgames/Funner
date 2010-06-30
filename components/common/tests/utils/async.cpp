#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/async.h>

using namespace common;

struct A: public xtl::reference_counter
{
  int id;

  A (int in_id) : id (in_id) { printf ("A::A(%d)\n", id); }
  A (const A& a) : id (a.id) { printf ("A::A(A(%d))\n", id); }
  ~A () { printf ("A::~A(%d)\n", id); }

  private:
    A& operator = (const A&); //no impl
};

typedef xtl::intrusive_ptr<A> APtr;

void callback (AsyncResult& result)
{
  printf ("  is_completed: %d\n", result.IsCompleted ());
}

APtr f ()
{
  return APtr (new A (1), false);
}

APtr g ()
{
  throw xtl::format_operation_exception ("::g()", "Test exception");
}

int main ()
{
  printf ("Results of async_test:\n");
  
  try
  {
    AsyncResult result1 = async_invoke<APtr> (f, callback);

    printf ("result1: %d\n", result1.Result<APtr> ()->id);

    AsyncResult result2 = async_invoke<APtr> (g, callback);
    
    printf ("result2: %d\n", result2.Result<APtr> ()->id);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
