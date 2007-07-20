#include <stl/hash_map>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Results of hash_key_test:\n");
  
  typedef hash_map<hash_key<const char*>, string> hmap;
  
  hmap m;
  
  m ["hello"] = "world";
  m ["world"] = "hello";
  
  printf ("m ['hello']='%s'\n", m ["hello"].c_str ());
  printf ("m ['world']='%s'\n", m ["world"].c_str ());  

  return 0;
}
