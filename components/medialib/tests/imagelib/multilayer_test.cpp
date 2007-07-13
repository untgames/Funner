#include <stdio.h>
#include <common/exception.h>
#include <common/file.h>
#include <media/image.h>
#include <tr1/functional>

using namespace medialib;
using namespace common;

const char* file_name1 = "data/pic1.jpg";
const char* file_name2 = "data/pic2.tga";
const char* file_name3 = "data/pic3.bmp";
      
void print_format (Image* image)
{
  printf ("image %s format - ", image->Name());
  switch (image->Format ())
  {
    case PF_RGB8:    printf ("RGB8");   break;
    case PF_RGB16:   printf ("RGB16");  break;
    case PF_RGBA8:   printf ("RGBA8");  break;
    case PF_RGBA16:  printf ("RGBA16"); break;
    case PF_BGR8:    printf ("BGR8");   break;
    case PF_BGRA8:   printf ("BGRA8");  break;
    case PF_L8:      printf ("L8");     break;
    case PF_A8:      printf ("A8");     break;
    case PF_LA8:     printf ("LA8");    break;
    default:         printf ("Unknown");
  }
  printf ("\n");
}   

int main ()
{
  try
  {
    printf ("Results of multilayer image test:\n");
    ImageSystem::RegisterSaveCodec ("dds", &Image::DefaultSaver);
    ImageSystem::RegisterSaveCodec ("cubemap", &Image::DefaultSaver);
    ImageSystem::RegisterSaveCodec ("skybox", &Image::DefaultSaver);
    MultilayerImageBuilder img_builder;

    Image img, img2 (file_name3);    

    img_builder.InsertLayer (file_name2);
    img_builder.CaptureLayer (img2);
    img_builder.Finish (img);
    
    printf ("multilayer image name - '%s'\n", img.Name());
    img.Rename ("new_name");
    printf ("multilayer image new name - '%s'\n", img.Name());
    
    print_format (&img);
    img.Convert (PF_RGB8);
    print_format (&img);
    
    img.Resize (400, 400, 4);
    img.Save("results/multilayer_image.dds", PF_RGBA16);

    printf ("multilayer image width - %u, height - %u, depth - %u\n", img.Width(), img.Height(), img.Depth());
    img.Resize (1000, 1000, 6);
    printf ("new multilayer image width - %u, height - %u, depth - %u\n", img.Width(), img.Height(), img.Depth());
    
    FileHash file_hash;

    img.Save("results/multilayer_image.bmp.skybox", PF_DEFAULT);

    FileSystem::GetFileHash ("results/multilayer_image_nx.bmp",file_hash);

    printf ("File 'results/multilayer_image_nx.bmp' hash: {");

    for (size_t i=0;i<15;i++)
      printf ("%02x,",file_hash.md5 [i]);

    printf ("%02x}\n",file_hash.md5 [15]);

    img2.Resize (10,10,1);
  }
  catch (std::exception& exception)               
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }
  
  return 0;                                               
}
