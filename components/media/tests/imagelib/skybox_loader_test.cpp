#include <stdio.h>
#include <common/exception.h>
#include <common/file.h>
#include <media/image.h>
#include <xtl/functional>

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
    printf ("Results of cubemax loader test:\n");

    Image image (file_name);

    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);

    image.Save ("results/pic1.jpg.skybox");

    print_hash ("results/pic1_up.jpg");
    print_hash ("results/pic1_down.jpg");
    print_hash ("results/pic1_left.jpg");
    print_hash ("results/pic1_right.jpg");
    print_hash ("results/pic1_front.jpg");
    print_hash ("results/pic1_back.jpg");
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
