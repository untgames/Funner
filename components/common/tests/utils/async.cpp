#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/async.h>

using namespace common;

struct A
{
  int id;
  
  A (int in_id) : id (in_id) { printf ("A::A(%d)\n", id); }
  A (const A& a) : id (a.id) { printf ("A::A(A(%d))\n", id); }
  ~A () { printf ("A::~A(%d)\n", id); }
  
  private:
    A& operator = (const A&); //no impl
};

void callback (AsyncResult& result)
{
  printf ("  is_completed: %d\n", result.IsCompleted ());
}

A f ()
{
  return A (1);
}

A g ()
{
  throw xtl::format_operation_exception ("::g()", "Test exception");
}

int main ()
{
  printf ("Results of async_test:\n");
  
  try
  {
    AsyncResult result1 = async_invoke<A> (f, callback);
    
    printf ("result1: %d\n", result1.Result<A> ().id);
    
    AsyncResult result2 = async_invoke<A> (g, callback);
    
    printf ("result2: %d\n", result2.Result<A> ().id);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
