#include <stl/vector>
#include <stdio.h>

using namespace stl;

struct A
{
  A ():name (0) { printf ("...construct A%d\n",name); }
  A (int _name) : name (_name) { printf ("...construct A%d\n",name); }
  A (const A& a) { printf ("...copy construct A%d\n",name=a.name); }
  ~A () { printf ("...destruct A%d\n",name); }
  
  A& operator = (const A& a) {
    printf ("...assign A%d = A%d\n",name,a.name);
    name = a.name;
    return *this;     
  }
  
  int name;
};

int main ()
{
  printf ("Results of vec09_test:\n");

  int array [] = {1,2,3,4,5,6,7,8,9};
  A* a = (A*)array;
  vector<A> v;
  
//  v.reserve (9);  
  
  printf ("begin\n");
  
  v.push_back (a [0]);
  v.push_back (a [1]);
  v.push_back (a [2]);

  printf ("insert at begin\n");
    
  v.insert (v.begin (),a+3,a+9);
  
  printf ("end\n"); 
  
  return 0;
}

