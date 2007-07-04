#include <common/singleton.h>
#include <stdio.h>

using namespace common;

struct Test1
{
  Test1 () { printf ("Test1::Test1\n"); }
  ~Test1 () { printf ("Test1::~Test1\n"); }  
  
  void Touch () { }
};

struct Test3
{
  Test3 () { printf ("Test3::Test3\n"); }
  ~Test3 () { printf ("Test3::~Test3\n"); }  
  
  void Touch () { }
};

struct Test2
{
  Test2 () {
    Singleton<Test1>::Instance ().Touch ();
    printf ("Test2::Test2\n");
  }

  ~Test2 () 
  { 
    printf ("Test2::~Test2\n");
    Singleton<Test3>::Instance ().Touch ();
  }

  void Print ()
  {
    printf ("Hello world from singleton\n");
  }  
};

int main ()
{
  printf ("Results of singleton_test:\n");
  
  Singleton<Test3>::Init ();
  Singleton<Test3>::Destroy ();
  
  printf ("Test3 is initialized: %s\n",Singleton<Test3>::IsInitialized ()?"true":"false");
  
  Singleton<Test2>::Instance ().Print ();
  
  printf ("Exit from main\n");

  return 0;
}
