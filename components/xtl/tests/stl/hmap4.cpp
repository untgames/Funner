#include <stl/hash_map>

int main ()
{
  try
  {
    printf ("Results of hmap4_test:\n");

    stl::hash_map<size_t, int> m;

    m [1u] = 5;
    m [2u] = 5;

    m [3u] = 5;

    m.erase (1u);
    m.erase (2u);

    m [1u] = 5;

    printf ("before deleting map\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  printf ("map deleted successfully\n");

  return 0;
}
