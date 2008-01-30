#include "shared.h"

struct TextureSize
{
  size_t width;
  size_t height;
  size_t layers;
};

//получение ближайшей сверху степени двойки
size_t next_higher_power_of_two (size_t k) 
{
  k--;

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//получение количества mip-уровней
size_t get_mips_count (size_t size) //оптимизировать
{
  return (size_t)(log ((float)next_higher_power_of_two (size)) / log (2.f)) + 1;
}

//получение количества mip-уровней
size_t get_mips_count (size_t width, size_t height)
{
  return width > height ? get_mips_count (width) : get_mips_count (height);
}

int myrand ()
{
  static int holdrand = 1;
  
  return ((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
}

bool test_texture (const TextureDesc& tex_desc, IDevice* device)
{
  printf ("%s %ux%ux%u@%s %s:\n", get_name (tex_desc.dimension), tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format), 
          tex_desc.generate_mips_enable ? "auto-mips" : "manual-mips");

  try
  {
    xtl::com_ptr<ITexture> texture (device->CreateTexture (tex_desc), false);
   
    xtl::uninitialized_storage <char> src_buffer (tex_desc.width * tex_desc.height * 4),
                                      dst_buffer (src_buffer.size ());

    for (size_t i = 0; i < src_buffer.size (); i++)
      src_buffer.data ()[i] = (char)myrand ();

    for (size_t i = 0; i < tex_desc.layers; i++)
    {  
      size_t width  = tex_desc.width;
      size_t height = tex_desc.height;

      for (size_t j = 0; get_mips_count (tex_desc.width, tex_desc.height); j++)
      {
        memset (dst_buffer.data (), 0, dst_buffer.size ());
        
        size_t data_size = width * height;

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
          case PixelFormat_D24S8: data_size *= 4; break;
          default: return false;  
        }

        texture->SetData (i, j, 0, 0, width, height, tex_desc.format, src_buffer.data ());

        texture->GetData (i, j, 0, 0, width, height, tex_desc.format, dst_buffer.data ());

        printf ("  layer %u, mip %u: ", i, j);

        if (memcmp (src_buffer.data (), dst_buffer.data (), data_size))
          printf ("FAIL\n");
        else 
          printf ("Ok\n");

        if (width > 1)  width  /= 2;
        if (height > 1) height /= 2;
      }
    }

    return true;
  }
  catch (std::exception& e)
  {
    printf ("FAIL\n%s\n", e.what ());
    return false;
  }
}

int main ()
{
  printf ("Results of all_textures_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (all_textures_test)");

    bool        status[TextureDimension_Num][PixelFormat_Num][2][2]; 
    TextureSize sizes [TextureDimension_Num] = {(8, 1, 1), (8, 8, 1), (8, 8, 2), (8, 8, 6)};  

    memset (status, 0, sizeof status);

    TextureDesc desc;
    
    for (size_t i = 0; i < TextureDimension_Num; i++)
      for (size_t j = 0; j < PixelFormat_Num; j++)
      {
        desc.dimension            = (TextureDimension)i;
        desc.width                = sizes[i].width;
        desc.height               = sizes[i].height;
        desc.layers               = sizes[i].layers;
        desc.format               = (PixelFormat)j;
        desc.bind_flags           = BindFlag_Texture;
        desc.generate_mips_enable = false;

        status [i][j][0][0] = test_texture (desc, test.device.get ());

        desc.width  -= 4;
        desc.height -= 4;

        status [i][j][0][1] = test_texture (desc, test.device.get ());

        desc.generate_mips_enable = true;

        status [i][j][1][0] = test_texture (desc, test.device.get ());

        desc.width  += 4;
        desc.height += 4;
 
        status [i][j][1][1] = test_texture (desc, test.device.get ());
      }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
