#include "shared.h"

using namespace media;
using namespace common;

const char* results_dir = "results";
const char* file_name2 = "data/pic2.tga";
const char* file_name3 = "data/pic3.bmp";
      
void print_format (Image* image)
{
  printf ("image %s format - ", image->Name());
  switch (image->Format ())
  {
    case PixelFormat_RGB8:    printf ("RGB8");   break;
    case PixelFormat_RGB16:   printf ("RGB16");  break;
    case PixelFormat_RGBA8:   printf ("RGBA8");  break;
    case PixelFormat_RGBA16:  printf ("RGBA16"); break;
    case PixelFormat_BGR8:    printf ("BGR8");   break;
    case PixelFormat_BGRA8:   printf ("BGRA8");  break;
    case PixelFormat_L8:      printf ("L8");     break;
    case PixelFormat_A8:      printf ("A8");     break;
    case PixelFormat_LA8:     printf ("LA8");    break;
    default:         printf ("Unknown");
  }
  printf ("\n");
}   

int main ()
{
  try
  {
    printf ("Results of multi_layer_image_test:\n");
    
    Image layers [2] = {Image (file_name2), Image (file_name3)};

    Image img (2, layers);
    
    if (!FileSystem::IsDir (results_dir))
      FileSystem::Mkdir (results_dir);

    printf ("multilayer image name - '%s'\n", img.Name());
    img.Rename ("new_name");
    printf ("multilayer image new name - '%s'\n", img.Name());
    
    print_format (&img);
    img.Convert (PixelFormat_RGB8);
    print_format (&img);
    
    img.Resize (400, 400, 4);
    img.Save("results/multilayer_image.dds", PixelFormat_RGBA16);

    printf ("multilayer image width - %u, height - %u, depth - %u\n", img.Width(), img.Height(), img.Depth());
    img.Resize (1000, 1000, 6);
    printf ("new multilayer image width - %u, height - %u, depth - %u\n", img.Width(), img.Height(), img.Depth());

    img.Save("results/multilayer_image.bmp.skybox");

    printf ("File 'results/multilayer_image_up.bmp' size: %u\n", FileSystem::GetFileSize ("results/multilayer_image_down.bmp"));

    FileSystem::Remove (results_dir);
  }
  catch (std::exception& exception)               
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }
  
  return 0;                                               
}
