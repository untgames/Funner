#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";
const char* file_name   = "results/fill.bmp";

void dump_hash (const char* file_name)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  try
  {
    printf ("Results of fill_test:\n");
    
    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);    

    Image image (100, 100, 1, PixelFormat_RGB8);
    
    struct rgb8
    {
      unsigned char red, green, blue;
    };
    
    rgb8 color = {255, 0, 0};

    image.Fill (PixelFormat_Default, &color);
    
    image.Save (file_name);

    dump_hash (file_name);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  return 0;
}
