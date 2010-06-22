#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";

int main ()
{
  try
  {
    printf ("Results of animated_cursor_test:\n");

    Image image ("data/aero_busy.ani");
    
    printf ("%u frames (%ux%ux%s):\n", image.Depth (), image.Width (), image.Height (), get_format_name (image.Format ()));
    
    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);
    
    for (size_t i=0; i<image.Depth (); i++)
    {
      unsigned char hash [16];
      
      md5 (image.Bitmap (i), get_bytes_per_pixel (image.Format ()) * image.Width () * image.Height (), hash);
      
      printf ("  image #%02u hash: {", i + 1);

      for (size_t i=0; i<15; i++)
        printf ("%02x,", hash [i]);

      printf ("%02x}\n", hash [15]);      
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  return 0;
}
