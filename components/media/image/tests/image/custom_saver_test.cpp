#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";
const char* file_name = "data/pic1.jpg";

void MyBinSaver (const char* file_name, const Image& img, const char*)
{
  FILE* save_file;

  if ((save_file = fopen (file_name, "w+")) == NULL)
    printf( "The file %s was not opened\n", file_name);

  fprintf (save_file, "This file was created by MyBinSaver - custom save function, when image '%s' was attemted to save", img.Name());

  fclose (save_file);
}

int main ()
{
  try
  {
    printf ("Results of custom saver test:\n");

    ImageManager::RegisterSaver ("bin", &MyBinSaver);

    Image image(file_name);

    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);

    image.Save("results/custom_saver.bin");

    FileHash file_hash;

    FileSystem::GetFileHash ("results/custom_saver.bin",file_hash);

    printf ("File 'results/custom_saver.bin' hash: {");

    for (size_t i=0;i<15;i++)
      printf ("%02x,",file_hash.md5 [i]);

    printf ("%02x}\n",file_hash.md5 [15]);
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
