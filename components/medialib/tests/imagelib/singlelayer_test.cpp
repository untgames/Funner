#include <stdio.h>
#include <time.h>
#include <common/exception.h>
#include <common/file.h>
#include <media/image.h>
#include <memory.h>

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
    printf ("Results of single layer  image test:\n");

    Image image1, image2 (file_name2), image3 (image2,PF_RGBA8), image4, image5 (file_name3);
    ImageSystem::RegisterSaveCodec ("tiff", &Image::DefaultSaver);

    image1 = image2;
    print_format (&image5);
    image2.Resize (204, 152, 1);

    image4.Load (file_name1);

    image4.Save("results/image4 (loading test).bmp");
    
    printf ("image4 name - %s\n", image4.Name());
    image4.Rename ("new_name");
    printf ("image4 new name - %s\n", image4.Name());
    
    print_format (&image4);
    image4.Convert (PF_RGBA16);
    print_format (&image4);
    
    printf ("image4 width - %u, height - %u, depth - %u\n", image4.Width(), image4.Height(), image4.Depth());
    image4.Resize (1000, 1000, 1);
    printf ("new image4 width - %u, height - %u, depth - %u\n", image4.Width(), image4.Height(), image4.Depth());
    
    image4.Save("results/image4 (PF convert and resize test).tif");
    
    image3.Save("results/image3.bmp");
    image2.Save("results/image2.bmp", PF_RGBA16);
    image2.Save("results/image2.jpg", PF_RGBA16);
    image2.Save("results/image2.tiff", PF_RGBA16);
    image2.Save("results/image2.tga", PF_RGBA8);
    image1.Save("results/image1.bmp", PF_RGBA8);

    image1.PutImage (10,10,0,60,60,1,image5.Format(),image5.Bitmap());
    image1.Save("results/image1with5 (PutImage test).bmp", PF_RGB8);
    
    FileHash file_hash;

    FileSystem::GetFileHash ("results/image1with5 (PutImage test).bmp",file_hash);

    printf ("File 'results/image1with5 (PutImage test).bmp' hash: {");

    for (size_t i=0;i<15;i++)
      printf ("%02x,",file_hash.md5 [i]);

    printf ("%02x}\n",file_hash.md5 [15]);

    Image().Swap(image4);

    image4.Save("results/image4-2 (exception test).bmp");
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  return 0;
} 
