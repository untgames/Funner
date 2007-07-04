#include <common/strlib.h>
#include <stdio.h>

using namespace common;
using namespace stl;

int main ()
{
  printf ("Results of path1_test:\n");
    
  const char* p1 [] = {"hello/world/base.suffix",".suffix","base",".","x/y.z"};

  for (int i=0;i<4;i++)  
  {
    printf ("basename ('%s')='%s'\n",p1 [i],common::basename (p1 [i]).c_str ());
    printf ("suffix ('%s')='%s'\n",p1 [i],common::suffix (p1 [i]).c_str ());
  }

  printf ("basename ('%s')='%s'\n",p1 [4],common::basename (string (p1 [4])).c_str ());
  printf ("suffix ('%s')='%s'\n",p1 [4],common::suffix (string (p1 [4])).c_str ());
  
  const char* p2 [] = {"x/y/z","x","/x","/","a/b/c"};
  
  for (int i=0;i<4;i++)
  {
    printf ("dir ('%s')='%s'\n",p2 [i],dir (p2 [i]).c_str ());
    printf ("notdir ('%s')='%s'\n",p2 [i],notdir (p2 [i]).c_str ());
  }  
  
  printf ("dir ('%s')='%s'\n",p2 [4],dir (string (p2 [4])).c_str ());
  printf ("notdir ('%s')='%s'\n",p2 [4],notdir (string (p2 [4])).c_str ());  

  return 0;
}
