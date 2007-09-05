#include <stdio.h>
#include <common/exception.h>
#include <common/file.h>
#include <stl/string>
#include <media/image.h>

using namespace medialib;
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

void WriteProperty (StdFile* file, const char* name, const char* value)
{
  file->Write (" ", 1);
  file->Write (name, strlen (name));
  file->Write ("=\"", 2);
  file->Write (value, strlen (value));
  file->Write ("\"", 1);
}

void WriteOpenTag (StdFile* file, const char* tag_name, size_t level)
{
  for (size_t i = 0; i < level * 2; i++)
    file->Write (" ", 1);

  file->Write ("<", 1);
  file->Write (tag_name, strlen (tag_name));
}

void WriteTagClose (StdFile* file, const char* tag_name, size_t level)
{
  for (size_t i = 0; i < level * 2; i++)
    file->Write (" ", 1);

  file->Write ("</", 2);
  file->Write (tag_name, strlen (tag_name));
  file->Write (">\n", 2);
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
    Image   img (argv[1]);
    size_t  bpp = GetBpp (img.Format ());
    char*   border_color = (char*)img.Bitmap ();
    char*   mask = new char [img.Width () * img.Height () * bpp];
    string  bfs_file_name (argv[1], FileName (argv[1]));    
    bfs_file_name += ".bfs";
    StdFile bfs_file (bfs_file_name.c_str (), FILE_MODE_WRITE_ONLY);

    if (!bpp)
      return 1;

    memset (mask, 1, img.Width () * img.Height () * bpp);

    bfs_file.Write ("<?xml version=\"1.0\" encoding = \"windows-1251\"?>\n", 48);
    WriteOpenTag (&bfs_file, "Font", 0);
    WriteProperty (&bfs_file, "Name", bfs_file_name.c_str ());
    WriteProperty (&bfs_file, "FontFile", argv[1]);
    WriteProperty (&bfs_file, "FirstCharCode", "0");
    bfs_file.Write (">\n", 2);    

    WriteOpenTag (&bfs_file, "Glyphs", 1);
    bfs_file.Write (">\n", 2);    
    mask [0] = 0;

    for (size_t i = 0; i < img.Height (); i++)
    {
      for (size_t j = 0; j < img.Width (); j++)
      {
        if (mask[i * img.Width () + j])
          if (!memcmp(border_color, &border_color[(i * img.Width () + j ) * bpp], bpp))
          {
            char   buffer[8];
            size_t width = 0, height = 0;

            for (; !memcmp(border_color, &border_color[(i * img.Width () + j + width) * bpp], bpp)  && (j + width < img.Width ()); width++);
            for (; !memcmp(border_color, &border_color[((i + height) * img.Width () + j) * bpp], bpp) && (i + height < img.Height ()); height++);

            size_t width2 = 0, height2 = 0;
            for (; !memcmp(border_color, &border_color[((i + height - 1) * img.Width () + j + width2) * bpp], bpp) && (j + width2 < img.Width ()); width2++);
            for (; !memcmp(border_color, &border_color[((i + height2) * img.Width () + j + width - 1) * bpp], bpp) && (i + height2 < img.Height ()); height2++);

            if (width != width2 || height != height2)
            {
              printf ("uncorrect image format, not square bbox detected\n");
              continue;
            }
            if (width < 3 || height < 3)
            {
              printf ("uncorrect image format, to small bbox detected\n");
              continue;
            }

            for (size_t m = 0; m < height; m++)
              for (size_t n = 0; n < width; n++)
                mask [(i + m) * img.Width () + j + n] = 0;
  
            WriteOpenTag (&bfs_file, "Glyph", 2);
            WriteProperty (&bfs_file, "XPos",   _itoa (j + 1, buffer, 10));
            WriteProperty (&bfs_file, "YPos",   _itoa (i + 1, buffer, 10));
            WriteProperty (&bfs_file, "Width",  _itoa (width - 2, buffer, 10));
            WriteProperty (&bfs_file, "Height", _itoa (height - 2, buffer, 10));
            bfs_file.Write ("/>\n", 3);
            j += width - 1;  
          }
      }
    }

    WriteTagClose (&bfs_file, "Glyphs", 1);
    WriteTagClose (&bfs_file, "Font", 0);
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
