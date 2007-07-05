#include <common/callback.h>
#include <stdio.h>

using namespace common;

struct A
{
         void test41 (int a,int b,int c,int d)               { printf ("A::test41 (%d,%d,%d,%d)\n",a,b,c,d);  }
  static void test42 (int a,int b,int c,int d)               { printf ("A::test42 (%d,%d,%d,%d)\n",a,b,c,d);  }
  static void test43 (int a,int b,int c,int d,void* context) { printf ("A::test43 (%d,%d,%d,%d,%X)\n",a,b,c,d,(size_t)context);  }
         void test31 (int a,int b,int c)                     { printf ("A::test31 (%d,%d,%d)\n",a,b,c);  }
  static void test32 (int a,int b,int c)                     { printf ("A::test32 (%d,%d,%d)\n",a,b,c);  }
  static void test33 (int a,int b,int c,void* context)       { printf ("A::test33 (%d,%d,%d,%X)\n",a,b,c,(size_t)context);  }
         void test21 (int a,int b)                           { printf ("A::test21 (%d,%d)\n",a,b);  }
  static void test22 (int a,int b)                           { printf ("A::test22 (%d,%d)\n",a,b);  }
  static void test23 (int a,int b,void* context)             { printf ("A::test23 (%d,%d,%X)\n",a,b,(size_t)context);  }
         void test11 (int a)                                 { printf ("A::test11 (%d)\n",a);  }
  static void test12 (int a)                                 { printf ("A::test12 (%d)\n",a);  }
  static void test13 (int a,void* context)                   { printf ("A::test13 (%d,%X)\n",a,(size_t)context);  }
         void test01 ()                                      { printf ("A::test01 ()\n");  }
  static void test02 ()                                      { printf ("A::test02 ()\n");  }
  static void test03 (void* context)                         { printf ("A::test03 (%X)\n",(size_t)context);  }
};

int main ()
{
  printf ("Results of callback_test:\n");

  A a;

  CallbackHandler<int,int,int,int> (a,&A::test41).operator () (1,2,3,4);
  CallbackHandler<int,int,int,int> (&A::test42).operator () (1,2,3,4);
  CallbackHandler<int,int,int,int> (A::test43,(void*)0xDEADBEEF).operator () (1,2,3,4);
  CallbackHandler<int,int,int>     (a,&A::test31).operator () (1,2,3);
  CallbackHandler<int,int,int>     (&A::test32).operator () (1,2,3);
  CallbackHandler<int,int,int>     (A::test33,(void*)0xDEADBEEF).operator () (1,2,3);
  CallbackHandler<int,int>         (a,&A::test21).operator () (1,2);
  CallbackHandler<int,int>         (&A::test22).operator () (1,2);
  CallbackHandler<int,int>         (A::test23,(void*)0xDEADBEEF).operator () (1,2);
  CallbackHandler<int>             (a,&A::test11).operator () (1);
  CallbackHandler<int>             (&A::test12).operator () (1);
  CallbackHandler<int>             (A::test13,(void*)0xDEADBEEF).operator () (1);
  CallbackHandler<>                (a,&A::test01).operator () ();
  CallbackHandler<>                (&A::test02).operator () ();
  CallbackHandler<>                (A::test03,(void*)0xDEADBEEF).operator () ();

  return 0;
}
