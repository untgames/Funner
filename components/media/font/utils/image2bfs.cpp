#include <stdio.h>

#include <stl/string>
#include <xtl/uninitialized_storage.h>

#include <common/xml_writer.h>

#include <media/image.h>

using namespace media;
using namespace common;
using stl::string;

size_t GetBpp (PixelFormat pf)
{
  switch (pf)
  {
    case PixelFormat_RGB8:   return 3;
    case PixelFormat_RGB16:  return 4;
    case PixelFormat_BGR8:   return 3;
    case PixelFormat_RGBA8:  return 4;
    case PixelFormat_RGBA16: return 8;
    case PixelFormat_BGRA8:  return 4;
    case PixelFormat_L8:     return 1;
    case PixelFormat_A8:     return 1;
    case PixelFormat_LA8:    return 2;
    default: printf ("Unknown pixel format\n"); return 0;  
  }
}

size_t FileName (const char* file_name)
{
  const char*  ext;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return len;

  ext = file_name + len;

  for (; len && (*ext != '.'); len--, ext--);

  if (!len)
    return strlen(file_name);

  return len;
}

int main (int argc, char *argv[])
{
  if (argc != 2)
  {
    printf ("Usage: image2bfs imagename\n");
    return 1;
  }

  try
  {
    Image   img (argv [1]);
    
    size_t  bpp          = GetBpp (img.Format ());
    char*   border_color = (char*)img.Bitmap ();

    if (!bpp)
      return 1;    

    string bfs_file_name (argv [1], FileName (argv [1]));

    bfs_file_name += ".bfs";

    XmlWriter bfs_file (bfs_file_name.c_str ());
    
    xtl::uninitialized_storage<char> mask_buffer (img.Width () * img.Height ());

    char* mask = mask_buffer.data ();

    memset (mask, 1, mask_buffer.size ());
    
    XmlNodeScope font_node (bfs_file, "Font");
    
    bfs_file.WriteAttribute ("Name", bfs_file_name.c_str ());
    bfs_file.WriteAttribute ("FontFile", argv [1]);
    bfs_file.WriteAttribute ("FirstCharCode", 0);
    
    XmlNodeScope glyphs_node (bfs_file, "Glyphs");

    mask [0] = 0;

    for (size_t i=0; i<img.Height (); i++)
    {
      for (size_t j=0; j<img.Width (); j++)
      {
        if (mask [i * img.Width () + j])
          if (!memcmp (border_color, &border_color[(i * img.Width () + j ) * bpp], bpp))
          {
            size_t width = 0, height = 0;

            for (; !memcmp(border_color, &border_color[(i * img.Width () + j + width) * bpp], bpp)  && (j + width < img.Width ()); width++);
            for (; !memcmp(border_color, &border_color[((i + height) * img.Width () + j) * bpp], bpp) && (i + height < img.Height ()); height++);

            size_t width2 = 0, height2 = 0;

            for (; !memcmp(border_color, &border_color[((i + height - 1) * img.Width () + j + width2) * bpp], bpp) && (j + width2 < img.Width ()); width2++);
            for (; !memcmp(border_color, &border_color[((i + height2) * img.Width () + j + width - 1) * bpp], bpp) && (i + height2 < img.Height ()); height2++);

            if (width != width2 || height != height2)
            {
              printf ("incorrect image format, not square bbox detected\n");
              continue;
            }

            if (width < 3 || height < 3)
            {
              printf ("incorrect image format, to small bbox detected\n");
              continue;
            }

            for (size_t m = 0; m < height; m++)
              for (size_t n = 0; n < width; n++)
                mask [(i + m) * img.Width () + j + n] = 0;
                
            XmlNodeScope glyph_node (bfs_file, "Glyph");
  
            bfs_file.WriteAttribute ("XPos",   j + 1);
            bfs_file.WriteAttribute ("YPos",   i + 1);
            bfs_file.WriteAttribute ("Width",  width - 2);
            bfs_file.WriteAttribute ("Height", height - 2);

            j += width - 1;
          }
      }
    }
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
    return 1;
  }                                               
  catch (...)
  {                                               
    printf ("exception..."); 
    return 1;
  }                                               

  return 0;
} 
