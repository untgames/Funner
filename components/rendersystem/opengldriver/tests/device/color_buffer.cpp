#include "shared.h"

struct Color3ub
{
  unsigned char red, green, blue;
};

void dump (size_t width, size_t height, const Color3ub* buffer)
{
  const Color3ub* pos = buffer;
  
  for (size_t i=0; i<height; i++)
  {
    for (size_t j=0; j<width; j++, pos++)
      printf ("%02x%02x%02x ", pos->red, pos->green, pos->blue);

    printf ("\n");
  }
}

void dump_mismatch (size_t width, size_t height, const Color3ub* src, const Color3ub* dst)
{
  for (size_t i=0; i<height; i++)
  {
    for (size_t j=0; j<width; j++, src++, dst++)
      printf ("%02x%02x%02x ", unsigned char (dst->red-src->red), unsigned char (dst->green-src->green),
        unsigned char (dst->blue-src->blue));

    printf ("\n");
  }
}

int main ()
{
  try
  {
    printf ("Results of color_buffer_test:\n");
    
    Test test (L"OpenGL device test window (color_buffer)");       

    TexturePtr color_buffer = test.device->OSGetRenderTargetView ()->GetTexture ();
    
    const size_t width = 10, height = 10;
    const size_t x = 200, y = 100;
    
    typedef xtl::uninitialized_storage<Color3ub> Buffer;
    
    Buffer src_buffer (width * height);
    
    Color3ub* pos = src_buffer.data ();
    
    for (size_t i=0; i<height; i++)
      for (size_t j=0; j<width; j++, pos++)
      {
        pos->red   = unsigned char (i);
        pos->green = unsigned char (j);
        pos->blue  = unsigned char (i * j);
      }
                
    color_buffer->SetData (0, 0, x, y, width, height, PixelFormat_RGB8, src_buffer.data ());
    
    Buffer dst_buffer (width * height);
    
    color_buffer->GetData (0, 0, x, y, width, height, PixelFormat_RGB8, dst_buffer.data ());
    
    if (!memcmp (src_buffer.data (), dst_buffer.data (), src_buffer.size () * sizeof (Color3ub)))
      printf ("Color buffer SetData/GetData match!\n");
    else
    {
      printf ("Color buffer SetData/GetData mismatch!\n");
      
      printf ("Buffer before SetData:\n");
      dump   (width, height, src_buffer.data ());
      printf ("Buffer after GetData:\n");
      dump   (width, height, dst_buffer.data ());
      printf ("Mismatch map:\n");
      dump_mismatch (width, height, src_buffer.data (), dst_buffer.data ());
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
