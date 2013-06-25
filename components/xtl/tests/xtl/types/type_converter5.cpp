#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

class A {};
class B : public A {};

template class declcast<B*, A*>;

int main ()
{
  printf ("Results of type_converter5_test:\n");
  
  #define TEST(FROM,TO) printf ("check convertion %s -> %s: %s\n", #FROM, #TO, has_custom_cast<FROM, TO> () ? "true" : "false");
  
  TEST (B&, A&);
  TEST (B, A);
  TEST (B*, A*);
  TEST (B, A*);
  TEST (B*, A);
 
  return 0;
}
