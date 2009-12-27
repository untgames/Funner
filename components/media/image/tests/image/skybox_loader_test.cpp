#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";
const char* file_name   = "data/pic1.jpg.skybox";

void print_hash (const char* file_name)
{
  FileHash file_hash;
  FileSystem::GetFileHash (file_name, file_hash);

  printf ("File '%s'\thash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  try
  {
    printf ("Results of skybox loader test:\n");

    Image image (file_name);    

    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);      

    image.Save ("results/pic1.png.skybox");

    print_hash ("results/pic1_up.png");
    print_hash ("results/pic1_down.png");
    print_hash ("results/pic1_left.png");
    print_hash ("results/pic1_right.png");
    print_hash ("results/pic1_front.png");
    print_hash ("results/pic1_back.png");
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }
  
  return 0;
}
