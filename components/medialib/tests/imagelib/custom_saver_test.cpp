#include <stdio.h>
#include <time.h>
#include <common/exception.h>
#include <common/file.h>
#include <media/image.h>
#include <memory.h>
#include <tr1/functional>

using namespace medialib;
using namespace common;

const char* file_name = "data/pic1.jpg";

void MyPngSaver (const char* file_name, const Image& img, ImagePixelFormat pf)
{
  FILE* save_file;

  if ((save_file = fopen (file_name, "w+")) == NULL)
    printf( "The file %s was not opened\n", file_name);

  fprintf (save_file, "This file was created by MyPngSaver - custom save function, when image '%s' was attemted to save", img.Name());

  fclose (save_file);
}

void main ()
{
  try
  {
    printf ("Results of custom saver test:\n");

    ImageSystem::RegisterSaveCodec ("png", &MyPngSaver);

    Image image(file_name);

    image.Save("results/custom_saver.png");

    FileHash file_hash;

    FileSystem::GetFileHash ("results/custom_saver.png",file_hash);

    printf ("File 'results/custom_saver.png' hash: {");

    for (size_t i=0;i<15;i++)
      printf ("%02x,",file_hash.md5 [i]);

    printf ("%02x}\n",file_hash.md5 [15]);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
}
