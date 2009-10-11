#include <cmath>

#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/file.h>

#include <media/compressed_image.h>

#pragma pack(1)

using namespace common;
using namespace media;

namespace
{

/*
    Описание формата PVR (взято из PVRTextureLoader)
*/

const size_t PVR_TEXTURE_FLAG_TYPE_MASK = 0xff;

static char gPVRTexIdentifier [] = "PVR!";

typedef size_t uint32_t;

enum
{
  kPVRTextureFlagTypePVRTC_2 = 24,
  kPVRTextureFlagTypePVRTC_4
};

typedef struct _PVRTexHeader
{
  uint32_t headerLength;
  uint32_t height;
  uint32_t width;
  uint32_t numMipmaps;
  uint32_t flags;
  uint32_t dataLength;
  uint32_t bpp;
  uint32_t bitmaskRed;
  uint32_t bitmaskGreen;
  uint32_t bitmaskBlue;
  uint32_t bitmaskAlpha;
  uint32_t pvrTag;
  uint32_t numSurfs;
} PVRTexHeader;

/*
    Изображение, сжатое в формате PVR
*/

class PvrCompressedImage: public ICustomCompressedImage
{
  public:
///Загрузка изображения
    PvrCompressedImage (const char* file_name)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");
                    
        InputFile file (file_name);
        
          //разбор заголовка файла
        
        PVRTexHeader header;
        
        if (file.Read (&header, sizeof (PVRTexHeader)) != sizeof (PVRTexHeader))
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Error at read header (%u bytes from start of file)", file_name, sizeof (PVRTexHeader));
          
        if (memcmp (&header.pvrTag, &gPVRTexIdentifier, sizeof (uint32_t)))
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Wrong tag", file_name);
                 
        format = header.flags & PVR_TEXTURE_FLAG_TYPE_MASK;
        
        if (format != kPVRTextureFlagTypePVRTC_4 && format != kPVRTextureFlagTypePVRTC_2)
          throw xtl::format_not_supported_exception ("", "PVR file '%s' with given format %u not supported (only PVRTC4 & PVRTC2 supported)", file_name, format);
                     
        size_t width  = header.width;
        size_t height = header.height;

        this->width  = width;
        this->height = height;

        has_alpha = header.bitmaskAlpha != 0;
        
          //чтение данных
          
        size_t data_length = header.dataLength;

        data.resize (data_length, false);
        
        size_t read_size = file.Read (data.data (), data.size ());
        
        if (read_size != data.size ())
          throw xtl::format_operation_exception ("", "Invalid PVR file '%s'. Error at read %u bytes from file (read_size=%u)", file_name, data.size (), read_size);
        
          // Calculate the data size for each texture level and respect the minimum number of blocks
          
        mip_levels.reserve (GetReservedMipsCount (width, height));
          
        size_t data_offset = 0;

        while (data_offset < data_length)
        {
          size_t block_size, width_blocks, height_blocks, bpp;

          if (format == kPVRTextureFlagTypePVRTC_4)
          {
            block_size    = 4 * 4; // Pixel by pixel block size for 4bpp
            width_blocks  = width / 4;
            height_blocks = height / 4;
            bpp           = 4;
          }
          else
          {
            block_size    = 8 * 4; // Pixel by pixel block size for 2bpp
            width_blocks  = width / 8;
            height_blocks = height / 4;
            bpp           = 2;
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
      return 1; //формат поддерживает только однослойные изображения
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
        case kPVRTextureFlagTypePVRTC_2: return has_alpha ? "rgba_pvrtc2" : "rgb_pvrtc2";
        case kPVRTextureFlagTypePVRTC_4: return has_alpha ? "rgba_pvrtc4" : "rgb_pvrtc4";
        default:                         throw xtl::format_operation_exception ("", "Internal error. Wrong format %u", format);
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
///Получение количества мип-уровней (не точное, необходимо для резервирования количества элементов в массиве)
    static size_t GetReservedMipsCount (size_t width, size_t height)
    {
      static const float COEF = 1.0f / log (2.0f);
      
      size_t size = width > height ? width : height;

      return (size_t)(log ((float)size) * COEF) + 1;
    }
    
  private:
    typedef xtl::uninitialized_storage<char>      Buffer;        
    typedef stl::vector<CompressedImageBlockDesc> MipLevelArrray;

  private:
    size_t         format;     //формат изображения
    bool           has_alpha;  //присутствует ли в изображении альфа-канал
    size_t         width;      //ширина изображения
    size_t         height;     //высота изображения
    Buffer         data;       //данные изображения
    MipLevelArrray mip_levels; //мип-уровни
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
