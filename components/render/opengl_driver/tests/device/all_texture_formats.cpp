#include "shared.h"

 //Маски сравнения пикселей в результатах тестирования

unsigned int compare_mask [PixelFormat_Num][PixelFormat_Num];

struct TextureSize
{
  unsigned int width;
  unsigned int height;
  unsigned int layers;
};

struct TestStatus
{
  unsigned int count, successfull, match_count;
};

const char* get_short_name (PixelFormat param)
{
  switch (param)
  {
    case PixelFormat_RGB8:  return "RGB8 ";
    case PixelFormat_RGBA8: return "RGBA8";
    case PixelFormat_L8:    return "  L8 ";
    case PixelFormat_A8:    return "  A8 ";
    case PixelFormat_LA8:   return " LA8 ";
    case PixelFormat_DXT1:  return "DXT1 ";
    case PixelFormat_DXT3:  return "DXT3 ";
    case PixelFormat_DXT5:  return "DXT5 ";
    case PixelFormat_D16:   return " D16 ";
    case PixelFormat_D24X8: return "D24X8";
    case PixelFormat_D24S8: return "D24S8";
    case PixelFormat_D32:   return " D32 ";
    case PixelFormat_S8:    return "  S8 ";
    default:                return "?????";
  }
}

int myrand ()
{
  static int holdrand = 1;
  
  return ((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
}

void print_diff (unsigned int size, const char* src, const char* dst)
{  
  static const unsigned int LINE_WIDTH = 15;

  printf ("                 Source buffer                                          Destination buffer"
          "                 Diff buffer\n");  

  for (unsigned int i=0; size; i++)
  {
    unsigned int line_size = size < LINE_WIDTH ? size : LINE_WIDTH, j;

    printf ("%03x| ", i * LINE_WIDTH);
    
    for (j=0; j<line_size; j++) printf ("%02x ", src [j] & 0xff);
    for (;j<LINE_WIDTH; j++)    printf ("?? ");

    printf ("    ");

    for (j=0; j<line_size; j++) printf ("%02x ", dst [j] & 0xff);
    for (;j<LINE_WIDTH; j++)    printf ("?? ");

    printf ("    ");

    for (j=0; j<line_size; j++) printf ("%02x ", (dst [j] - src [j]) & 0xff);
    for (;j<LINE_WIDTH; j++)    printf ("?? ");

    printf ("\n");

    size -= line_size;
    src  += line_size;
    dst  += line_size;
  }
}

bool is_buffers_equal (const char* src, const char* dst, unsigned int size, unsigned int pixel_size, unsigned int mask)
{
  static const unsigned int masks [4] = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};

  for (unsigned int i = 0; i < size / pixel_size; i++, src+=pixel_size, dst+=pixel_size)
  {
    unsigned int src_pixel = 0, dst_pixel = 0;

    for (unsigned int j=0; j<pixel_size; j++)
    {
      src_pixel += (unsigned int) ((src[j]) << (8 * j)) & masks[j];
      dst_pixel += (unsigned int) ((dst[j]) << (8 * j)) & masks[j];
    }                                    

    src_pixel &= mask;
    dst_pixel &= mask;

    if (src_pixel != dst_pixel)
      return false;
  }

  return true;
}

TestStatus test_texture_format (ITexture* texture, PixelFormat test_format)
{
  TestStatus status = {0, 0};

  TextureDesc tex_desc;

  texture->GetDesc (tex_desc);
  
  printf ("%s %ux%ux%u@%s<->%s:\n", get_name (tex_desc.dimension), tex_desc.width, tex_desc.height, tex_desc.layers, get_short_name (tex_desc.format), 
          get_short_name (test_format));          

  try
  {
   
    xtl::uninitialized_storage <char> src_buffer (tex_desc.width * tex_desc.height * 4),
                                      dst_buffer (src_buffer.size ());

    for (unsigned int i = 0; i < src_buffer.size (); i++)
      src_buffer.data ()[i] = (char)myrand () & 63;

    for (unsigned int i = 0; i < tex_desc.layers; i++)
    {  
      unsigned int volatile mips_count = get_mips_count (tex_desc.width, tex_desc.height);

      if (is_compressed (tex_desc.format))
        mips_count -= 2;

      for (unsigned int j = 0; j < mips_count; j++)
      {
        if (tex_desc.dimension == TextureDimension_3D)
        {
          unsigned int layers_count = tex_desc.layers >> j;
          
          if (!layers_count)
            layers_count = 1;

          if (i >= layers_count)
            continue;            
        }
        
        status.count++;
        
        unsigned int level_width  = tex_desc.width >> j,
                     level_height = tex_desc.height >> j;
        
        switch (tex_desc.format)
        {
          case PixelFormat_DXT1:
          case PixelFormat_DXT3:
          case PixelFormat_DXT5:
               //выравнивание размеров уровня по ближайшему снизу числу, делящемуся на 4

            level_width  &= ~3;
            level_height &= ~3;

            if (!level_width)  level_width  = 4;
            if (!level_height) level_height = 4;

            break;
          default:
            if (!level_width)  level_width  = 1;
            if (!level_height) level_height = 1;
            break;
        }

        printf ("  layer %u, mip %u, width = %u, height = %u: ", i, j, level_width, level_height);
        
        try
        {                    
          memset (dst_buffer.data (), 0xff, dst_buffer.size ());
          
          unsigned int data_size = level_width * level_height;
          unsigned int texel_size;

          switch (test_format)
          {
            case PixelFormat_DXT1:  
              data_size /= 2; 
              texel_size = 4; 
              break;
            case PixelFormat_DXT3:
            case PixelFormat_DXT5:                  
              texel_size = 4; 
              break;
            case PixelFormat_L8:
            case PixelFormat_A8:
            case PixelFormat_S8:    
              texel_size = 1; 
              break;
            case PixelFormat_LA8:
            case PixelFormat_D16:   
              data_size *= 2; 
              texel_size = 2; 
              break;
            case PixelFormat_RGB8:  
              data_size *= 3; 
              texel_size = 3; 
              break;
            case PixelFormat_RGBA8:
            case PixelFormat_D24X8:
            case PixelFormat_D24S8: 
            case PixelFormat_D32:
              data_size *= 4; 
              texel_size = 4; 
              break;
            default: return status; 
          }
          
          texture->SetData (i, j, 0, 0, level_width, level_height, test_format, src_buffer.data ());
          texture->GetData (i, j, 0, 0, level_width, level_height, test_format, dst_buffer.data ());          
          
          status.successfull++;

          if (is_buffers_equal (src_buffer.data (), dst_buffer.data (), data_size, texel_size, compare_mask[tex_desc.format][test_format]))
          {
            status.match_count++;
            printf ("Ok\n");
          }
          else 
          {
            printf ("FAIL\n");
            print_diff (data_size, src_buffer.data (), dst_buffer.data ());
          }
        }
        catch (std::exception& e)
        {
          printf ("FAIL\nException: '%s'\n", e.what ());
        }
      }
    }
  }
  catch (std::exception& e)
  {
    printf ("FAIL\nException: '%s'\n", e.what ());
  }
  
  return status;
}

void print_status_table (TestStatus status [PixelFormat_Num][PixelFormat_Num])
{
  printf ("       ");

  for (unsigned int i=0; i<PixelFormat_Num; i++)
    printf ("%s ", get_short_name ((PixelFormat)i));
    
  printf ("\n");
    
  for (unsigned int i=0; i<PixelFormat_Num; i++)
  {
    printf ("%s| ", get_short_name ((PixelFormat)i));

    for (unsigned int j=0; j<PixelFormat_Num; j++)
    {
      TestStatus& s = status [j][i];
      
      if (!s.count)                      printf ("  N/A ");
      else if (!s.successfull)           printf ("   -  ");
      else if (s.match_count == s.count) printf ("   +  ");
      else                               printf ("%02u/%02u ", s.match_count, s.count);
    }

    printf ("\n");
  }
}

void set_compare_masks ()
{
  memset (compare_mask, 0, sizeof compare_mask);

  for (unsigned int i = 0; i < PixelFormat_Num; i++)
    compare_mask [i][i] = 0xffffffff;

  compare_mask [PixelFormat_RGB8][PixelFormat_RGBA8] = 0x00ffffff;
  compare_mask [PixelFormat_RGB8][PixelFormat_L8]    = 0x000000ff;
  compare_mask [PixelFormat_RGB8][PixelFormat_LA8]   = 0x000000ff;
  
  compare_mask [PixelFormat_RGBA8][PixelFormat_RGB8] = 0x00ffffff;
  compare_mask [PixelFormat_RGBA8][PixelFormat_L8]   = 0x000000ff;
  compare_mask [PixelFormat_RGBA8][PixelFormat_A8]   = 0x000000ff;
  compare_mask [PixelFormat_RGBA8][PixelFormat_LA8]  = 0x0000ffff;

  compare_mask [PixelFormat_L8][PixelFormat_RGB8]  = 0x000000ff;
  compare_mask [PixelFormat_L8][PixelFormat_RGBA8] = 0x000000ff;
  compare_mask [PixelFormat_L8][PixelFormat_L8]    = 0x000000ff;
  compare_mask [PixelFormat_L8][PixelFormat_LA8]   = 0x000000ff;

  compare_mask [PixelFormat_A8][PixelFormat_RGBA8] = 0xff000000;
  compare_mask [PixelFormat_A8][PixelFormat_A8]    = 0x000000ff;
  compare_mask [PixelFormat_A8][PixelFormat_LA8]   = 0x0000ff00;

  compare_mask [PixelFormat_LA8][PixelFormat_RGB8]  = 0x000000ff;
  compare_mask [PixelFormat_LA8][PixelFormat_RGBA8] = 0xff0000ff;
  compare_mask [PixelFormat_LA8][PixelFormat_L8]    = 0x000000ff;

  compare_mask [PixelFormat_D16][PixelFormat_D24X8] = 0x00ffff00;

  compare_mask [PixelFormat_D24X8][PixelFormat_D16]   = 0x0000ffff;
  compare_mask [PixelFormat_D24X8][PixelFormat_D24X8] = 0x00ffffff;

  compare_mask [PixelFormat_D24S8][PixelFormat_D16]   = 0x0000ffff;
  compare_mask [PixelFormat_D24S8][PixelFormat_D24X8] = 0x00ffffff;

}


int main ()
{
  printf ("Results of all_texture_formats_test:\n");

  set_compare_masks ();

  try
  {
    Test test (L"OpenGL device test window (all_texture_formats_test)");

    TestStatus  status [TextureDimension_Num][PixelFormat_Num][PixelFormat_Num]; 
    TextureSize sizes [TextureDimension_Num] = {{16, 1, 1}, {16, 16, 1}, {16, 16, 4}, {16, 16, 6}};

    memset (status, 0, sizeof status);

    TextureDesc desc;
    
    desc.bind_flags           = BindFlag_Texture;
    desc.access_flags         = AccessFlag_Read | AccessFlag_Write;    
    desc.generate_mips_enable = false;

    for (unsigned int i = 0; i < TextureDimension_Num; i++)
    {
      desc.dimension = (TextureDimension)i;
      desc.width     = sizes [i].width;
      desc.height    = sizes [i].height;
      desc.layers    = sizes [i].layers;

      for (unsigned int j = 0; j < PixelFormat_Num; j++)
      {
        switch (j)
        {
          case PixelFormat_RGB_PVRTC2:
          case PixelFormat_RGB_PVRTC4:
          case PixelFormat_RGBA_PVRTC2:
          case PixelFormat_RGBA_PVRTC4:
            continue;
          default:
            break;
        }
        
        desc.format = (PixelFormat)j;

        try
        {
          printf ("%s %ux%ux%u@%s:\n", get_name (desc.dimension), desc.width, desc.height, desc.layers, get_short_name (desc.format));

          xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

          for (unsigned int k = 0; k < PixelFormat_Num; k++)
          {          
            status [i][j][k] = test_texture_format (texture.get (), (PixelFormat)k);
          }
        }
        catch (std::exception& exception)
        {
          printf ("exception: %s\n", exception.what ());
        }
      }
    }
      
    for (unsigned int i=0; i<TextureDimension_Num; i++)
    {
      printf ("Status table for %s texture:\n", get_name ((TextureDimension)i));
      
      print_status_table (status [i]);
      
      printf ("*************************************\n");
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
