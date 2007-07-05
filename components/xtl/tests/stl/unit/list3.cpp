#include <stl/list>
#include <stdio.h>

using namespace stl;

void print (const char* message,const list<char>& x)
{
  printf ("%s: ",message);
  for (list<char>::const_iterator i=x.begin ();i!=x.end ();++i) printf ("%c",*i);
  printf ("\n");
}

int main ()
{
  printf ("Results of list3_test:\n");
  
  char array [] = {'x','l','x','t','s','s'};

  list<char> str (array,array+6);
  
  print       ("original",str);    
  str.reverse ();
  print       ("reversed",str);
  str.remove  ('x');  
  print       ("removed",str);
  str.unique  ();
  print       ("uniqued",str);    
  str.sort    ();
  print       ("sorted",str);   

  return 0;
}
