#include <cmath>

#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/utility>

#include <common/component.h>
#include <common/file.h>

#include <media/compressed_image.h>

using namespace common;
using namespace media;

namespace components
{

namespace pvr_loader
{

/*
    Описание формата PVR
*/

static char PVR_VERSION [] = { 0x50, 0x56, 0x52, 0x03 };

typedef size_t             uint32_t;
typedef unsigned long long uint64_t;

enum PixelFormat
{
  PixelFormat_PVRTC2RGB,
  PixelFormat_PVRTC2RGBA,
  PixelFormat_PVRTC4RGB,
  PixelFormat_PVRTC4RGBA,
};

#ifdef _MSC_VER
  #pragma pack(push,1)
  #define PACKED_STRUCTURE
#else
  #define PACKED_STRUCTURE __attribute__ ((__packed__))
#endif

struct PACKED_STRUCTURE PVRTexHeader
{
  uint32_t version;
  uint32_t flags;
  uint64_t pixel_format;
  uint32_t color_space;
  uint32_t channel_type;
  uint32_t height;
  uint32_t width;
  uint32_t depth;
  uint32_t surfaces_count;
  uint32_t faces_count;
  uint32_t mips_count;
  uint32_t meta_data_size;
};

struct PACKED_STRUCTURE PVRMetaDataHeader
{
  uint32_t fourcc;
  uint32_t key;
  uint32_t data_size;
};

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

/*
    Изображение, сжатое в формате PVR
*/

class PvrCompressedImage: public ICustomCompressedImage
{
  public:
///Загрузка изображения
    PvrCompressedImage (const char* file_name)
    {
      xtl::compile_time_assert<sizeof (uint64_t) == 8> ();

      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");

        InputFile file (file_name);

          //разбор заголовка файла
        
        PVRTexHeader header;
        
        if (file.Read (&header, sizeof (PVRTexHeader)) != sizeof (PVRTexHeader))
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Error at read header (%u bytes from start of file)", file_name, sizeof (PVRTexHeader));
          
        if (memcmp (&header.version, &PVR_VERSION, sizeof (uint32_t)))
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Wrong tag", file_name);

        format = (size_t)(header.pixel_format & 0xffffffff);
        
        if ((format != PixelFormat_PVRTC2RGB && format != PixelFormat_PVRTC2RGBA && format != PixelFormat_PVRTC4RGB && format != PixelFormat_PVRTC4RGBA) || (header.pixel_format >> 32))
          throw xtl::format_not_supported_exception ("", "PVR file '%s' with given format %u not supported (only PVRTC4 & PVRTC2 supported)", file_name, format);

        if (header.color_space)
          throw xtl::format_not_supported_exception ("", "Only linear color space supported");

        if (header.depth > 1)
          throw xtl::format_not_supported_exception ("", "3d textures not supported");

        if (header.surfaces_count > 1)
          throw xtl::format_not_supported_exception ("", "Texture arrays not supported");

        layers_count = header.faces_count;

        file.Seek (header.meta_data_size, FileSeekMode_Current);

        size_t width  = header.width;
        size_t height = header.height;

        this->width  = width;
        this->height = height;

          //чтение данных
          
        size_t data_length = file.Size () - file.Tell ();

        data.resize (data_length, false);

        size_t read_size = file.Read (data.data (), data.size ());
        
        if (read_size != data.size ())
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Error at read %u bytes from file (read_size=%u)", file_name, data.size (), read_size);

          // Calculate the data size for each texture level and respect the minimum number of blocks
          
        mip_levels.reserve (header.mips_count);

        size_t data_offset = 0,
               bpp         = format == PixelFormat_PVRTC4RGB || format == PixelFormat_PVRTC4RGBA  ? 4 : 2;

        while (data_offset < data_length)
        {
          size_t block_size, width_blocks, height_blocks;

          if (bpp == 4)
          {
            block_size    = 4 * 4; // Pixel by pixel block size for 4bpp
            width_blocks  = width / 4;
            height_blocks = height / 4;
          }
          else
          {
            block_size    = 8 * 4; // Pixel by pixel block size for 2bpp
            width_blocks  = width / 8;
            height_blocks = height / 4;
          }

            // Clamp to minimum number of blocks

          if (width_blocks < 2)  width_blocks  = 2;
          if (height_blocks < 2) height_blocks = 2;

          size_t data_size = width_blocks * height_blocks * ((block_size  * bpp) / 8);

          CompressedImageBlockDesc mip_level;

          mip_level.offset = data_offset;
          mip_level.size   = data_size;

          mip_levels.push_back (mip_level);

          data_offset += data_size;
          width       /= 2;
          height      /= 2;
          width        = width ? width : 1;
          height       = height ? height : 1;
        }        
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::PvrCompressedImage::PvrCompressedImage");        
        throw;
      }
    }
  
///Ширина изображения
    size_t Width ()
    {
      return width;
    }
    
///Высота изображения
    size_t Height ()
    {
      return height;
    }

///Количество слоёв
    size_t LayersCount ()
    {
      return layers_count; //формат поддерживает только однослойные изображения
    }

///Количество мип-уровней
    size_t MipsCount ()
    {
      return mip_levels.size ();
    }
    
///Формат изображения
    const char* Format ()
    {
      switch (format)
      {
        case PixelFormat_PVRTC2RGB:  return "rgb_pvrtc2";
        case PixelFormat_PVRTC2RGBA: return "rgba_pvrtc2";
        case PixelFormat_PVRTC4RGB:  return "rgb_pvrtc4";
        case PixelFormat_PVRTC4RGBA: return "rgba_pvrtc4";
        default:                     throw xtl::format_operation_exception ("", "Internal error. Wrong format %u", format);
      }
    }

///Возвращение буфера данных
    const void* Data ()
    {
      return data.data ();
    }

///Возвращение блоков
    const CompressedImageBlockDesc* Blocks ()
    {
      return &mip_levels [0];
    }
    
  private:
    typedef xtl::uninitialized_storage<char>      Buffer;        
    typedef stl::vector<CompressedImageBlockDesc> MipLevelArray;

  private:
    size_t        format;       //формат изображения
    size_t        width;        //ширина изображения
    size_t        height;       //высота изображения
    size_t        layers_count; //количество слоев
    Buffer        data;         //данные изображения
    MipLevelArray mip_levels;   //мип-уровни
};

/*
   Компонент
*/

class Component
{
  public:
    Component ()
    {
      CompressedImageManager::RegisterLoader ("pvr", &Component::CreatePvrImage);
    }

  private:
    static ICustomCompressedImage* CreatePvrImage (const char* file_name)
    {
      return new PvrCompressedImage (file_name);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> PvrLoader ("media.compressed_image.pvr_loader");

}

}

}
