#include "shared.h"

/*
    ѕолучение размера пиксел€
*/

size_t get_pixel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:   return 3;
    case PixelFormat_RGBA8:  return 4;
    case PixelFormat_L8:     return 1;
    case PixelFormat_A8:     return 1;
    case PixelFormat_LA8:    return 2;
    case PixelFormat_D16:    return 2;
    case PixelFormat_D24X8:  return 4;
    case PixelFormat_D24S8:  return 4;
    case PixelFormat_S8:     return 1;
    default:                 return 0;
  }
}

size_t get_pixel_used_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_D24X8:  return 3;
    default:                 return get_pixel_size (format);
  }
}

/*
    ѕечать, заполнение, сравнение строк пикселей
*/

int myrand ()
{
  static int holdrand = 1;
  
  return ((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
}

void init_line (size_t width, size_t pixel_size, size_t pixel_used_size, unsigned char* data)
{
  unsigned char* pixel = data;

  for (size_t i=0; i<width; i++, pixel += pixel_size)
  {
    size_t j;
    
    for (j=0; j<pixel_used_size; j++) pixel [j] = myrand () & 63;
    for (;j<pixel_size; j++)          pixel [j] = 0;
  }    
}

void print_line (size_t width, size_t pixel_size, size_t pixel_used_size, const unsigned char* data)
{
  const unsigned char* pixel = data;

  for (size_t i=0; i<width; i++, pixel += pixel_size)
  {
    for (size_t j=0; j<pixel_used_size; j++)
      printf ("%02x", pixel [pixel_size-j-1]);

    printf (" ");
  }
}

void print_line_diff (size_t width, size_t pixel_size, size_t pixel_used_size, const unsigned char* line1, const unsigned char* line2)
{
  const unsigned char *pixel1 = line1, *pixel2 = line2;

  for (size_t i=0; i<width; i++, pixel1 += pixel_size, pixel2 += pixel_size)
  {
    for (size_t j=0; j<pixel_size; j++)
      printf ("%02x", (unsigned char)(pixel2 [pixel_size-j-1] - pixel1 [pixel_size-j-1]));

    printf (" ");
  }
}

void init_buffer (size_t width, size_t height, size_t pixel_size, size_t pixel_used_size, void* data)
{
  unsigned char* line = reinterpret_cast<unsigned char*> (data);
  
  for (size_t i=0, line_size=width*pixel_size; i<height; i++, line += line_size)
    init_line (width, pixel_size, pixel_used_size, line);
}

void print_buffer (size_t width, size_t height, size_t pixel_size, size_t pixel_used_size, const void* data)
{
  const unsigned char* line = reinterpret_cast<const unsigned char*> (data);
  
  for (size_t i=0, line_size=width*pixel_size; i<height; i++, line += line_size)
  {
    printf ("    ");
    print_line (width, pixel_size, pixel_used_size, line);
    printf ("\n");
  }
}

void print_buffer_diff (size_t width, size_t height, size_t pixel_size, size_t pixel_used_size, const void* buffer1, const void* buffer2)
{
  const unsigned char* line1 = reinterpret_cast<const unsigned char*> (buffer1);
  const unsigned char* line2 = reinterpret_cast<const unsigned char*> (buffer2);
  
  for (size_t i=0, line_size=width*pixel_size; i<height; i++, line1 += line_size, line2 += line_size)
  {
    printf ("    ");
    print_line (width, pixel_size, pixel_used_size, line1);
    printf ("    ");
    print_line (width, pixel_size, pixel_used_size, line2);
    printf ("    ");
    print_line_diff (width, pixel_size, pixel_used_size, line1, line2);
    printf ("\n");
  }
}

/*
    “естирование операции установки / чтени€ данных дл€ буфера указанного формата
*/

void test_buffer (ITexture& render_buffer, size_t x, size_t y, size_t width, size_t height, PixelFormat format)
{
  size_t pixel_size      = get_pixel_size (format),
         pixel_used_size = get_pixel_used_size (format);
         
  if (!pixel_size || !pixel_used_size || pixel_size < pixel_used_size)
  {
//    printf ("wrong format %s (pixel_size=%u, pixel_used_size=%u)\n", get_name (format), pixel_size, pixel_used_size);
    return;
  }
  
  printf ("  test format %s: ", get_name (format));    
  
  try
  {
    typedef xtl::uninitialized_storage<unsigned char> Buffer;
    
    Buffer src_buffer (pixel_size * width * height), dst_buffer (pixel_size * width * height);
    
    init_buffer (width, height, pixel_size, pixel_used_size, src_buffer.data ());
    
    render_buffer.SetData (0, 0, x, y, width, height, format, src_buffer.data ());
    render_buffer.GetData (0, 0, x, y, width, height, format, dst_buffer.data ());
    
    if (!memcmp (src_buffer.data (), dst_buffer.data (), src_buffer.size ()))
    {
      printf ("Ok!\n");
    }
    else
    {
      printf ("FAIL!\n  Mismatch map (source buffer | destination buffer | difference):\n");
      print_buffer_diff (width, height, pixel_size, pixel_used_size, src_buffer.data (), dst_buffer.data ());
    }    
  }
  catch (std::exception& exception)
  {
    printf ("EXCEPTION: %s\n", exception.what ());
  }
}

void test_buffer (ITexture& render_buffer)
{
  static const size_t width = 4, height = 4, x = 20, y = 10;

  for (int i=0; i<PixelFormat_Num; i++)
  {
    PixelFormat format = static_cast<PixelFormat> (i);
    
    test_buffer (render_buffer, x, y, width, height, format);
  }
}

int main ()
{
  try
  {
    printf ("Results of render_buffers_test:\n");
    
    Test test (L"OpenGL device test window (render_buffers)", "");
    
    Color4f clear_color;
    
    clear_color.red   = 1.0f / 255.0f;
    clear_color.green = 2.0f / 255.0f;
    clear_color.blue  = 3.0f / 255.0f;
    clear_color.alpha = 4.0f / 255.0f;

    test.device->ClearViews (ClearFlag_All, clear_color, 0, 0);

    printf ("Color back-buffer test:\n");

    TexturePtr back_color_buffer = test.device->OSGetRenderTargetView ()->GetTexture ();

    test_buffer (*back_color_buffer);

    printf ("Color front-buffer test:\n");

    TexturePtr front_color_buffer (test.device->CreateRenderTargetTexture (test.swap_chain.get (), 0), false);

    test_buffer (*front_color_buffer);    

    printf ("Depth-stencil buffer test:\n");

    TexturePtr depth_stencil_buffer = test.device->OSGetDepthStencilView ()->GetTexture ();

    test_buffer (*depth_stencil_buffer);    
    
    printf ("Application-created color buffer test:\n");
    
    TextureDesc desc;
    
    memset (&desc, 0, sizeof (desc));
    
    desc.dimension    = TextureDimension_2D;
    desc.width        = 512;
    desc.height       = 512;
    desc.layers       = 1;
    desc.format       = PixelFormat_RGBA8;
    desc.bind_flags   = BindFlag_RenderTarget;
    desc.access_flags = AccessFlag_ReadWrite;

    TexturePtr color_render_buffer = test.device->CreateTexture (desc);

    test_buffer (*color_render_buffer);
    
    printf ("Application-created depth-stencil buffer test:\n");

    desc.format     = PixelFormat_D24S8;
    desc.bind_flags = BindFlag_DepthStencil;

    TexturePtr depth_stencil_render_buffer = test.device->CreateTexture (desc);

    test_buffer (*depth_stencil_render_buffer);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
