#include <cstdio>
#include <exception>

#include <common/singleton.h>

using namespace common;

struct Test
{
  typedef common::Singleton<Test> MySingleton;

  Test () { MySingleton::Instance (); }
};

int main ()
{
  printf ("Results of singleton_recursive_init_test:\n");
  
  try
  {
    Test::MySingleton::Instance ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
