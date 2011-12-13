#include "shared.h"

const char*  file_names []    = {"data/test4.wxf", "data/test5.wxf"};
const size_t file_names_count = sizeof (file_names) / sizeof (*file_names);

int main ()
{
  printf ("Results of parse_concat_test:\n");
  
  try
  {
    ParseNode nodes [file_names_count];
    
    for (size_t i=0; i<file_names_count; i++)
      nodes [i] = Parser (file_names [i]).Root ();

    printf ("Parse of %u files successfull\n", file_names_count);
    
    ParseNode root = concat (file_names_count, nodes);

    print (root, 0);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
