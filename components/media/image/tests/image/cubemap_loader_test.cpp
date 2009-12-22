#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";
const char* file_name   = "data/pic1.jpg.cubemap";

void print_hash (const char* file_name)
{
  FileHash file_hash;
  FileSystem::GetFileHash (file_name, file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  try
  {
    printf ("Results of cubemap loader test:\n");

    Image image (file_name);

    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);

    image.Save ("results/pic1.tga.cubemap");

    print_hash ("results/pic1_px.tga");
    print_hash ("results/pic1_nx.tga");
    print_hash ("results/pic1_py.tga");
    print_hash ("results/pic1_ny.tga");
    print_hash ("results/pic1_pz.tga");
    print_hash ("results/pic1_nz.tga");
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
