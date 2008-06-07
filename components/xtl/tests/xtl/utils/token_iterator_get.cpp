#include <stdio.h>
#include <xtl/token_iterator.h>

using xtl::io::get;

void checked_get (xtl::io::token_iterator<const char*>& iter)
{
  try
  {
    double x = get<double> (iter);    

    printf ("value=%g\n", x);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}

int main ()
{
  printf ("Results of token_iterator_get:\n");
   
  const char* tokens [] = {"error", "1", "hello", "3.14"};
  xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens, tokens + sizeof (tokens)/sizeof(*tokens));  
  
  printf ("value=%d\n", get<int> (iter, -1));
  ++iter;
  printf ("value=%d\n", get<int> (iter, -1));
  printf ("value='%s'\n", get<const char*> (iter, "__default__"));  

  checked_get (iter);
  checked_get (iter);

  return 0;
}
