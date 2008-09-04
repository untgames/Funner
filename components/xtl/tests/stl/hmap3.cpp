#include <cstdio>

#include <stl/hash_map>
#include <stl/string>

using namespace stl;

int main ()
{
  printf ("Results of hmap3_test:\n");
  
  typedef hash_map<stl::string, int> my_map;
  
  my_map m;
  
  m["Window1.window_startup.startup_param"] = 0;
  m["Windows.Window2.Style"]                = 1;
  m["Windows.Window2.Width"]                = 1;
  m["Windows.Window2.Height"]               = 1;
  m["Windows.Window3.Style"]                = 1;
  m["Windows.Window4.Style"]                = 1;

  printf ("Count for 'Windows.Window2.Title' is %u\n", m.count ("Windows.Window2.Title"));

  return 0;
}
