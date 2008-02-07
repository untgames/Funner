#include "shared.h"

using namespace common;

const size_t TEX_SIZE = 4;

void print_buffers (size_t size, const char* src, const char* dst, size_t line_width)
{  
  printf ("                 Source buffer                       Destination buffer\n");  

  for (size_t i=0; size; i++)
  {
    size_t line_size = size < line_width ? size : line_width, j;

    printf ("%03x| ", i * line_width);
    
    for (j=0; j<line_size; j++) printf ("%02x ", src [j] & 0xff);
    for (;j<line_width; j++)    printf ("?? ");

    printf ("    ");

    for (j=0; j<line_size; j++) printf ("%02x ", dst [j] & 0xff);
    for (;j<line_width; j++)    printf ("?? ");

    printf ("\n");

    size -= line_size;
    src  += line_size;
    dst  += line_size;
  }
}


int main ()
{
  printf ("Results of texture2d_clip_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture2d_clip_test)");    
    
    TextureDesc desc;    

    memset (&desc, 0, sizeof (desc));
          
    desc.dimension            = TextureDimension_2D;
    desc.width                = TEX_SIZE;
    desc.height               = TEX_SIZE;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGBA8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
    
    xtl::uninitialized_storage<char> src_buffer (TEX_SIZE * TEX_SIZE * 4), dst_buffer (src_buffer.size ());

    memset (src_buffer.data (), 0xff, src_buffer.size ());    
    memset (dst_buffer.data (), 0,    dst_buffer.size ());
    
    texture->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, dst_buffer.data ());
    texture->SetData (0, 0, 2, 2, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, src_buffer.data ()); 
    texture->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, dst_buffer.data ());

    print_buffers (src_buffer.size (), src_buffer.data (), dst_buffer.data (), TEX_SIZE * 4);
    
    memset (dst_buffer.data (), 0, dst_buffer.size ());    
    
    texture->GetData (0, 0, 2, 2, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, dst_buffer.data ());
    
    print_buffers (src_buffer.size (), src_buffer.data (), dst_buffer.data (), TEX_SIZE * 4);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
