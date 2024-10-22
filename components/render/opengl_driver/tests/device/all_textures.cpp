#include "shared.h"

struct TextureSize
{
  unsigned int width;
  unsigned int height;
  unsigned int layers;
};

struct TestStatus
{
  unsigned int count, successfull;
};

const char* get_short_name (TextureDimension param)
{
  switch (param)
  {
    case TextureDimension_1D:      return "1D";
    case TextureDimension_2D:      return "2D";
    case TextureDimension_3D:      return "3D";
    case TextureDimension_Cubemap: return "Cubemap";
    default:                       return "??";
  }
}

const char* get_short_name (PixelFormat param)
{
  switch (param)
  {
    case PixelFormat_RGB8:  return "RGB8 ";
    case PixelFormat_RGBA8: return "RGBA8";
    case PixelFormat_L8:    return "L8   ";
    case PixelFormat_A8:    return "A8   ";
    case PixelFormat_LA8:   return "LA8  ";
    case PixelFormat_DXT1:  return "DXT1 ";
    case PixelFormat_DXT3:  return "DXT3 ";
    case PixelFormat_DXT5:  return "DXT5 ";
    case PixelFormat_D16:   return "D16  ";
    case PixelFormat_D24X8: return "D24X8";
    case PixelFormat_D24S8: return "D24S8";
    case PixelFormat_D32:   return "D32  ";
    case PixelFormat_S8:    return "S8   ";
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
  static const unsigned int LINE_WIDTH = 16;

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

TestStatus test_texture (const TextureDesc& tex_desc, IDevice* device)
{
  TestStatus status = {0, 0};
  
  if (tex_desc.generate_mips_enable)
  {
    switch (tex_desc.format)
    {
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        return status;
      default:
        break;
    }
  }

  printf ("%s %ux%ux%u@%s %s:\n", get_name (tex_desc.dimension), tex_desc.width, tex_desc.height, tex_desc.layers, get_short_name (tex_desc.format), 
          tex_desc.generate_mips_enable ? "auto-mips" : "manual-mips");          

  try
  {
    xtl::com_ptr<ITexture> texture (device->CreateTexture (tex_desc), false);
   
    xtl::uninitialized_storage <char> src_buffer (tex_desc.width * tex_desc.height * 4),
                                      dst_buffer (src_buffer.size ());

    for (unsigned int i = 0; i < src_buffer.size (); i++)
      src_buffer.data ()[i] = (char)myrand () & 63;
//      src_buffer.data ()[i] = i & 63;      

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
       
        unsigned int level_width  = tex_desc.width >> j,
                     level_height = tex_desc.height >> j;
        
        switch (tex_desc.format)
        {
          case PixelFormat_DXT1:
          case PixelFormat_DXT3:
          case PixelFormat_DXT5:
               //ўла ў­Ёў ­ЁҐ а §¬Ґа®ў га®ў­п Ї® Ў«Ё¦ ©иҐ¬г б­Ё§г зЁб«г, ¤Ґ«пйҐ¬гбп ­  4

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

          switch (tex_desc.format)
          {
            case PixelFormat_DXT1:  data_size /= 2; break;
            case PixelFormat_DXT3:
            case PixelFormat_DXT5:
            case PixelFormat_L8:
            case PixelFormat_A8:
            case PixelFormat_S8:    break;
            case PixelFormat_LA8:
            case PixelFormat_D16:   data_size *= 2; break;
            case PixelFormat_RGB8:  data_size *= 3; break;
            case PixelFormat_RGBA8:
            case PixelFormat_D24X8:
            case PixelFormat_D24S8:
            case PixelFormat_D32:   data_size *= 4; break;
            default: return status; 
          }          
          
          texture->SetData (i, j, 0, 0, level_width, level_height, tex_desc.format, src_buffer.data ());
          texture->GetData (i, j, 0, 0, level_width, level_height, tex_desc.format, dst_buffer.data ());

          status.count++;

          if (tex_desc.format == PixelFormat_D24X8)
          {
            for (unsigned int k=3; k < src_buffer.size (); k += 4)
            {
              src_buffer.data ()[k] = 0;
              dst_buffer.data ()[k] = 0;
            }
          }

          if (!memcmp (src_buffer.data (), dst_buffer.data (), data_size))
          {
            status.successfull++;
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

void print_status_table (TestStatus status [TextureDimension_Num][PixelFormat_Num])
{
  printf ("       ");

  for (unsigned int i=0; i<TextureDimension_Num; i++)
    printf ("%s     ", get_short_name ((TextureDimension)i));
    
  printf ("\n");
    
  for (unsigned int i=0; i<PixelFormat_Num; i++)
  {
    printf ("%s| ", get_short_name ((PixelFormat)i));

    for (unsigned int j=0; j<TextureDimension_Num; j++)
    {
      TestStatus& s = status [j][i];
      
      if (!s.count)                      printf ("  -   ");
      else if (s.successfull == s.count) printf ("  +   ");
      else                               printf ("%02u/%02u ", s.successfull, s.count);
    }

    printf ("\n");
  }
}

int main ()
{
  printf ("Results of all_textures_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (all_textures_test)");

    TestStatus  status [2][2][TextureDimension_Num][PixelFormat_Num]; 
    TextureSize sizes [2][TextureDimension_Num] = {{{16, 1, 1}, {16, 16, 1}, {16, 16, 4}, {16, 16, 6}},
                                                   {{12, 1, 1}, {12, 12, 1}, {12, 12, 4}, {12, 12, 6}}};

    memset (status, 0, sizeof status);

    TextureDesc desc;
    
    for (unsigned int i = 0; i < TextureDimension_Num; i++)
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

        desc.dimension            = (TextureDimension)i;
        desc.width                = sizes [0][i].width;
        desc.height               = sizes [0][i].height;
        desc.layers               = sizes [0][i].layers;
        desc.format               = (PixelFormat)j;
        desc.bind_flags           = BindFlag_Texture;
        desc.access_flags         = AccessFlag_Read | AccessFlag_Write;
        desc.generate_mips_enable = false;

        status [0][0][i][j] = test_texture (desc, test.device.get ());

        desc.width                = sizes [1][i].width;
        desc.height               = sizes [1][i].height;
        desc.layers               = sizes [1][i].layers;

        status [0][1][i][j] = test_texture (desc, test.device.get ());

        desc.generate_mips_enable = true;

        status [1][1][i][j] = test_texture (desc, test.device.get ());
 
        desc.width                = sizes [0][i].width;
        desc.height               = sizes [0][i].height;
        desc.layers               = sizes [0][i].layers;

        status [1][0][i][j] = test_texture (desc, test.device.get ());
      }
      
    static const char *mips_titles [2] = {"manual-mips", "auto-mips"},
                      *pot_titles [2] = {"power of two", "non power of two"};
      
    for (unsigned int i=0; i<2; i++)
      for (unsigned int j=0; j<2; j++)
      {
        printf ("Status table for %s textures with %s:\n", pot_titles [i], mips_titles [j]);
        
        print_status_table (status [j][i]);
        
        printf ("*************************************\n");
      }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
