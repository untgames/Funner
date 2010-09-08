#include <cmath>

#include <stl/algorithm>
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
    Описание формата PVR
*/

#if !defined(MAKEFOURCC)
# define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    (uint32(uint8(ch0)) | (uint32(uint8(ch1)) << 8) | \
    (uint32(uint8(ch2)) << 16) | (uint32(uint8(ch3)) << 24 ))
#endif

typedef unsigned int  uint32;
typedef unsigned char uint8;

///Идентификатор файла DDS
const char DDS_MAGIC_ID [4] = {'D', 'D', 'S', ' '};

///Блоки форматов DDS
const uint32 FOURCC_DDS  = MAKEFOURCC ('D', 'D', 'S', ' ');
const uint32 FOURCC_DXT1 = MAKEFOURCC ('D', 'X', 'T', '1');
const uint32 FOURCC_DXT2 = MAKEFOURCC ('D', 'X', 'T', '2');
const uint32 FOURCC_DXT3 = MAKEFOURCC ('D', 'X', 'T', '3');
const uint32 FOURCC_DXT4 = MAKEFOURCC ('D', 'X', 'T', '4');
const uint32 FOURCC_DXT5 = MAKEFOURCC ('D', 'X', 'T', '5');
const uint32 FOURCC_RXGB = MAKEFOURCC ('R', 'X', 'G', 'B');
const uint32 FOURCC_ATI1 = MAKEFOURCC ('A', 'T', 'I', '1');
const uint32 FOURCC_ATI2 = MAKEFOURCC ('A', 'T', 'I', '2');
const uint32 FOURCC_A2XY = MAKEFOURCC ('A', '2', 'X', 'Y');
const uint32 FOURCC_DX10 = MAKEFOURCC ('D', 'X', '1', '0');

///Флаги заголовка DDS файла
enum
{
  DDSD_CAPS        = 0x1,      //Required in every .dds file
  DDSD_HEIGHT      = 0x2,      //Required in every .dds file
  DDSD_WIDTH       = 0x4,      //Required in every .dds file
  DDSD_PITCH       = 0x8,      //Required when pitch is provided for an uncompressed texture
  DDSD_PIXELFORMAT = 0x1000,   //Required in every .dds file
  DDSD_MIPMAPCOUNT = 0x20000,  //Required in a mipmapped texture
  DDSD_LINEARSIZE  = 0x80000,  //Required when pitch is provided for a compressed texture
  DDSD_DEPTH       = 0x800000, //Required in a depth texture
};

///Флаги формата пикселей DDS файла
enum
{
  DDPF_ALPHAPIXELS  = 0x1,     //Texture contains alpha data; dwRGBAlphaBitMask contains valid data
  DDPF_ALPHA        = 0x2,     //Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data)
  DDPF_FOURCC       = 0x4,     //Texture contains compressed RGB data; dwFourCC contains valid data
  DDPF_RGB          = 0x40,    //Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (dwRBitMask, dwRBitMask, dwRBitMask) contain valid data
  DDPF_YUV          = 0x200,   //Used in some older DDS files for YUV uncompressed data (dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask)
  DDPF_LUMINANCE    = 0x20000, //Used in some older DDS files for single channel color uncompressed data (dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file
};

///Флаги Caps
enum
{
  DDSCAPS_COMPLEX = 0x8,      //Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or volume texture)
  DDSCAPS_MIPMAP  = 0x400000, //Optional; should be used for a mipmap
  DDSCAPS_TEXTURE = 0x1000,   //Required
};

///Флаги Caps2
enum
{
  DDSCAPS2_CUBEMAP           = 0x200,      //Required for a cube map
  DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,      //Required when these surfaces are stored in a cube map
  DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,      //Required when these surfaces are stored in a cube map
  DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,     //Required when these surfaces are stored in a cube map
  DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,     //Required when these surfaces are stored in a cube map
  DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,     //Required when these surfaces are stored in a cube map
  DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,     //Required when these surfaces are stored in a cube map
  DDSCAPS2_VOLUME            = 0x200000,   //Required for a volume texture
  DDSCAPS2_CUBEMAP_ALL_FACES = 0x0000FC00U //Required when all cubemap faces are stored in a cube map
};

///Тип ресурса
enum
{
  D3D10_RESOURCE_DIMENSION_UNKNOWN   = 0,
  D3D10_RESOURCE_DIMENSION_BUFFER    = 1,
  D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
  D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
  D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4,
};

///Формат пикселей DDS файла
struct DdsPixelFormat
{
  uint32 dwSize;        //Structure size; set to 32 (bytes)
  uint32 dwFlags;       //Values which indicate what type of data is in the surface
  char   dwFourCC [4];  //Four-character codes for specifying compressed or custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat member of that structure indicates the true format. When using a four-character code, dwFlags must include DDPF_FOURCC
  uint32 dwRGBBitCount; //Number of bits in an RGB (possibly including alpha) format. Valid when dwFlags includes DDPF_RGB, DDPF_LUMINANCE, or DDPF_YUV
  uint32 dwRBitMask;    //Red (or lumiannce or Y) mask for reading color data. For instance, given the A8R8G8B8 format, the red mask would be 0x00ff0000
  uint32 dwGBitMask;    //Green (or U) mask for reading color data. For instance, given the A8R8G8B8 format, the green mask would be 0x0000ff00
  uint32 dwBBitMask;    //Blue (or V) mask for reading color data. For instance, given the A8R8G8B8 format, the blue mask would be 0x000000ff
  uint32 dwABitMask;    //Alpha mask for reading alpha data. dwFlags must include DDPF_ALPHAPIXELS or DDPF_ALPHA. For instance, given the A8R8G8B8 format, the alpha mask would be 0xff000000
};

///Заголовок DDS файла
struct DdsHeader
{
  uint32         dwSize;            //Size of structure. This member must be set to 124
  uint32         dwFlags;           //Flags to indicate which members contain valid data
  uint32         dwHeight;          //Surface height (in pixels)
  uint32         dwWidth;           //Surface width (in pixels)
  uint32         dwLinearSize;      //The number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture. The pitch must be DWORD aligned
  uint32         dwDepth;           //Depth of a volume texture (in pixels), otherwise unused
  uint32         dwMipMapCount;     //Number of mipmap levels, otherwise unused
  uint32         dwReserved1 [11];  //Unused
  DdsPixelFormat ddpf;              //The pixel format
  uint32         dwCaps;            //Specifies the complexity of the surfaces stored
  uint32         dwCaps2;           //Additional detail about the surfaces stored
  uint32         dwCaps3;           //Unused
  uint32         dwCaps4;           //Unused
  uint32         dwReserved2;       //Unused
};

///Дополнительный заголовок для поддержки массивов текстур DX10
struct DdsHeaderDx10
{
  uint32 dxgiFormat;         //The surface pixel format
  uint32 resourceDimension;  //Identifies the type of resource being used
  uint32 miscFlag;           //Identifies other, less common options for resources
  uint32 arraySize;          //The number of elements in the array
  uint32 reserved;           //Reserved for future use
};

/*
    Утилиты
*/

template <class T>
void read (File& file, const char* file_name, T& value)
{
  if (file.Read (&value, sizeof (T)) != sizeof (T))
    throw xtl::format_operation_exception ("", "Error at read DDS file '%s'", sizeof (PVRTexHeader), file_name);
}

/*
    Изображение, сжатое в формате DDS
*/

class DdsCompressedImage: public ICustomCompressedImage
{
  public:
///Загрузка изображения
    DdsCompressedImage (const char* file_name)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");

        InputFile file (file_name);

          //чтение заголовков

        char id [4];
        
        read (file, file_name, id);
        
        if (memcmp (id, DDS_MAGIC_ID, sizeof (DDS_MAGIC_ID)))
          throw xtl::format_operation_exception ("", "Invalid DDS file. Wrong magic id");
          
        DdsHeader     header;
        DdsHeaderDx10 header_dx10;        
        bool          has_dx10_header = false;

        read (file, file_name, header);
        
        if ((header.ddpf.dwFlags & DDPF_FOURCC) && !memcmp (header.ddpf.dwFourCC, DDS_FOURC_DX10))
        {
            //чтение дополнительного заголовка для поддержки DX10                    
            
          read (file, file_name, header_dx10);

          has_dx10_header = true;
        }
        
          //проверка корректности
          
        CheckSupported (header, &header_dx10);
        
          //чтение данных

        size_t data_size = file.Size () - file.Tell ();

        data.resize (data_size, false);
        
        size_t read_size = file.Read (data.data (), data.size ());

        if (read_size != data.size ())
          throw xtl::format_operation_exception ("", "Invalid DDS file '%s'. Error at read %u bytes from file (read_size=%u)", file_name, data.size (), read_size);

          //варианты загрузки
          
        if (has_dx10_header)
        {
          
        }
        else
        {         
          bool is_uncompressed_texture_2d = DDSD_PI
          
          if ()
          {
          }
        }
      }
      catch (xtl::exception& e)
      {
        if (file_name)
        {
          e.touch ("media::DdsCompressedImage::DdsCompressedImage('%s')", file_name);
        }
        else
        {
          e.touch ("media::DdsCompressedImage::DdsCompressedImage");
        }
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
      return layers_count;
    }

///Количество мип-уровней
    size_t MipsCount ()
    {
      return mips_count;
    }
    
///Формат изображения
    const char* Format ()
    {
      ????
    }

///Возвращение буфера данных
    const void* Data ()
    {
      return data.data ();
    }

///Возвращение блоков
    const CompressedImageBlockDesc* Blocks ()
    {
      return &blocks [0];
    }
    
  private:
    static stl::string GetFourCCString (size_t code)
    {
      stl::string result;

      result.reserve (4);

      result.push_back ((char)((code >> 24) & 0xFF));
      result.push_back ((char)((code >> 16) & 0xFF));
      result.push_back ((char)((code >> 8) & 0xFF));
      result.push_back ((char)(code & 0xFF));

      return result;
    }
  
    static void CheckValid (const DdsHeader& header)
    {
      if (header.dwSize != sizeof (header))
        throw xtl::format_operation_exception ("", "Invalid DDS file. Invalid header dwSize %u (must be %u)", 
          header.dwSize, sizeof (header));
          
      if (header.dwFourCC != FOURCC_DDS)
        throw xtl::format_operation_exception ("", "Invalid DDS file. Invalid dwFourCC '%s' (must be '%s ')", 
          GetFourCCString (header.dwFourCC).c_str (), GetFourCCString (FOURCC_DDS).c_str ());  
      
      static const uint32 required = DDSD_WIDTH | DDSD_HEIGHT /* | DDSD_CAPS | DDSD_PIXELFORMAT */;
      
      if (!(header.dwFlags & DDSD_HEIGHT))
        throw xtl::format_operation_exception ("", "Invalid DDS file. No DDSD_HEIGHT flag set");

      if (!(header.dwFlags & DDSD_WIDTH))
        throw xtl::format_operation_exception ("", "Invalid DDS file. No DDSD_WIDTH flag set");
            
      if (header.ddpf.dwSize != sizeof (DdsPixelFormat))
        throw xtl::format_operation_exception ("", "Invalid DDS file. Invalid pixel format dwSize %u (must be %u)", 
          header.ddpf.dwSize, sizeof (DdsPixelFormat));

      if (!(header.dwCaps & DDSCAPS_TEXTURE))
        throw xtl::format_operation_exception ("", "Invalid DDS file. No DDSCAPS_TEXTURE");
    }
    
    static void CheckSupported (const DdsHeader& header, const DdsHeaderDx10* header_dx10)
    {
      CheckValid (header);

      if (header_dx10)
      {
        return; //поддерживаются все варианты
      }
      else
      {
        if (header.ddpf.dwFlags & DDPF_FOURCC)
        {
          switch (header.ddpf.dwFourCC)
          {
            case FOURCC_DXT1:
            case FOURCC_DXT2:
            case FOURCC_DXT3:
            case FOURCC_DXT4:
            case FOURCC_DXT5:
            case FOURCC_ATI1:
            case FOURCC_ATI2:
              break;
            default:
              throw xtl::format_not_supported_exception ("", "Unsupported DDS file. Unknown fourcc '%s'",
                GetFourCCString (header.ddpf.dwFourCC).c_str ());
          }
        }
        else if (header.ddpf.dwFlags & DDPF_RGB)
        {
          // All RGB formats are supported now.
        }
        else
        {
          throw xtl::format_not_supported_exception ("", "Unsupported DDS file. Wrong dwFlags: no DDPF_FOURCC and no DDPF_RGB (dwFlags=%08x)", header.ddpf.dwFlags);
        }

        if (IsTextureCube (header) && (header.dwCaps2 & DDSCAPS2_CUBEMAP_ALL_FACES) != DDSCAPS2_CUBEMAP_ALL_FACES)
          throw xtl::format_not_supported_exception ("", "Unsupported DDS file. Cubemap must have all faces (wrong dwCaps2)");

        if (IsTexture3D (header))
        {
          throw xtl::format_not_supported_exception ("", "Unsupported DDS file. 3D textures not supported");
        }
      }      
    }
    
    static bool IsTextureCube (const DdsHeader& header)
    {
      return (header.dwCaps2 & DDSCAPS2_CUBEMAP) != 0;
    }
      
    static bool IsTexture3D (const DdsHeader& header, const DdsHeaderDx10* header_dx10)
    {
      if (header_dx10)
      {
        return header_dx10.dwResourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D;
      }
      else
      {
        return (header.dwCaps2 & DDSCAPS2_VOLUME) != 0;
      }
    }

    static bool IsUncompressedTexture2D (const DdsHeader& header)
    {
      return header.ddpf.dwFlags & DDPF_RGB != 0;
    }
    
    static bool IsCompressedTexture2D (const DdsHeader& header)
    {
      return header.ddpf.dwFlags & DDPF_FOURCC != 0;
    }    
    
    static bool IsMipmappedTexture (const DdsHeader& header)
    {
      return header.dwFlags & DDSD_MIPMAPCOUNT && header.dwCaps & DDSCAPS_MIPMAP && header.dwCaps & DDSCAPS_COMPLEX;
    }
    
    static size_t GetMipLevelSize (const DdsHeader& header, size_t width, size_t height)
    {
      size_t factor = 1;
    }
    
  private:
    typedef xtl::uninitialized_storage<char>      Buffer;        
    typedef stl::vector<CompressedImageBlockDesc> BlockDescArray;

  private:
    size_t        width;        //ширина изображения
    size_t        height;       //высота изображения
    size_t        layers_count; //количество слоёв
    size_t        mips_count;   //количество mip-уровней
    Buffer        data;         //данные изображения
    MipLevelArray blocks;       //дескрипторы блоков
};

/*
   Компонент
*/

class Component
{
  public:
    Component ()
    {
      CompressedImageManager::RegisterLoader ("dds", &Component::CreateDdsImage);
    }

  private:
    static ICustomCompressedImage* CreateDdsImage (const char* file_name)
    {
      return new DdsCompressedImage (file_name);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> DdsLoader ("media.compressed_image.dds_loader");

}

}
