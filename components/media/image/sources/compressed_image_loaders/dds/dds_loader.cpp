#include <cmath>

#include <stl/algorithm>
#include <stl/vector>
#include <stl/string>

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

// surface description flags
const unsigned long DDSF_CAPS           = 0x00000001l;
const unsigned long DDSF_HEIGHT         = 0x00000002l;
const unsigned long DDSF_WIDTH          = 0x00000004l;
const unsigned long DDSF_PITCH          = 0x00000008l;
const unsigned long DDSF_PIXELFORMAT    = 0x00001000l;
const unsigned long DDSF_MIPMAPCOUNT    = 0x00020000l;
const unsigned long DDSF_LINEARSIZE     = 0x00080000l;
const unsigned long DDSF_DEPTH          = 0x00800000l;

// pixel format flags
const unsigned long DDSF_ALPHAPIXELS    = 0x00000001l;
const unsigned long DDSF_FOURCC         = 0x00000004l;
const unsigned long DDSF_RGB            = 0x00000040l;
const unsigned long DDSF_RGBA           = 0x00000041l;

// dwCaps1 flags
const unsigned long DDSF_COMPLEX         = 0x00000008l;
const unsigned long DDSF_TEXTURE         = 0x00001000l;
const unsigned long DDSF_MIPMAP          = 0x00400000l;

// dwCaps2 flags
const unsigned long DDSF_CUBEMAP            = 0x00000200l;
const unsigned long DDSF_CUBEMAP_POSITIVEX  = 0x00000400l;
const unsigned long DDSF_CUBEMAP_NEGATIVEX  = 0x00000800l;
const unsigned long DDSF_CUBEMAP_POSITIVEY  = 0x00001000l;
const unsigned long DDSF_CUBEMAP_NEGATIVEY  = 0x00002000l;
const unsigned long DDSF_CUBEMAP_POSITIVEZ  = 0x00004000l;
const unsigned long DDSF_CUBEMAP_NEGATIVEZ  = 0x00008000l;
const unsigned long DDSF_CUBEMAP_ALL_FACES  = 0x0000FC00l;
const unsigned long DDSF_VOLUME             = 0x00200000l;

// compressed texture types
const unsigned long FOURCC_UNKNOWN       = 0;

const unsigned long FOURCC_R8G8B8        = 20;
const unsigned long FOURCC_A8R8G8B8      = 21;
const unsigned long FOURCC_X8R8G8B8      = 22;
const unsigned long FOURCC_R5G6B5        = 23;
const unsigned long FOURCC_X1R5G5B5      = 24;
const unsigned long FOURCC_A1R5G5B5      = 25;
const unsigned long FOURCC_A4R4G4B4      = 26;
const unsigned long FOURCC_R3G3B2        = 27;
const unsigned long FOURCC_A8            = 28;
const unsigned long FOURCC_A8R3G3B2      = 29;
const unsigned long FOURCC_X4R4G4B4      = 30;
const unsigned long FOURCC_A2B10G10R10   = 31;
const unsigned long FOURCC_A8B8G8R8      = 32;
const unsigned long FOURCC_X8B8G8R8      = 33;
const unsigned long FOURCC_G16R16        = 34;
const unsigned long FOURCC_A2R10G10B10   = 35;
const unsigned long FOURCC_A16B16G16R16  = 36;

const unsigned long FOURCC_L8            = 50;
const unsigned long FOURCC_A8L8          = 51;
const unsigned long FOURCC_A4L4          = 52;

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

const unsigned long FOURCC_D16_LOCKABLE  = 70;
const unsigned long FOURCC_D32           = 71;
const unsigned long FOURCC_D24X8         = 77;
const unsigned long FOURCC_D16           = 80;

const unsigned long FOURCC_D32F_LOCKABLE = 82;

const unsigned long FOURCC_L16           = 81;

// Floating point surface formats

// s10e5 formats (16-bits per channel)
const unsigned long FOURCC_R16F          = 111;
const unsigned long FOURCC_G16R16F       = 112;
const unsigned long FOURCC_A16B16G16R16F = 113;

// IEEE s23e8 formats (32-bits per channel)
const unsigned long FOURCC_R32F          = 114;
const unsigned long FOURCC_G32R32F       = 115;
const unsigned long FOURCC_A32B32G32R32F = 116;

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
  uint32 dwFourCC;      //Four-character codes for specifying compressed or custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat member of that structure indicates the true format. When using a four-character code, dwFlags must include DDPF_FOURCC
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
    throw xtl::format_operation_exception ("", "Error at read DDS file '%s'", file_name);
}

/*
    Изображение, сжатое в формате DDS
 */

class DdsCompressedImage: public ICustomCompressedImage
{
  public:
    ///Загрузка изображения
    DdsCompressedImage (const char* file_name)
      : bytes_per_block (0)
    {
      static const char* METHOD_NAME = "media::DdsCompressedImage::DdsCompressedImage";

      if (!file_name)
        throw xtl::make_null_argument_exception ("", "file_name");

      try
      {
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

        if ((header.ddpf.dwFlags & DDPF_FOURCC) && (header.ddpf.dwFourCC == FOURCC_DX10))
        {
          //чтение дополнительного заголовка для поддержки DX10

          read (file, file_name, header_dx10);

          has_dx10_header = true;
        }

        //проверка корректности

        CheckSupported (header, &header_dx10);

        //варианты загрузки
        /*
          if (has_dx10_header)
          {
            //пока не поддерживаем
          }
         */
        // check if image is a volume texture
        if ((header.dwCaps2 & DDSF_VOLUME) && (header.dwDepth > 0))
          depth = header.dwDepth;
        else
          depth = 0;

        // There are flags that are supposed to mark these fields as valid, but some dds files don't set them properly
        width  = header.dwWidth;
        height = header.dwHeight;

        if (header.dwFlags & DDSF_MIPMAPCOUNT)
          mips_count = header.dwMipMapCount;
        else
          mips_count = 1;

        //check cube-map faces
        if (header.dwCaps2 & DDSF_CUBEMAP)
        {
          //this is a cubemap, count the faces
          layers_count = 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) ? 1 : 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) ? 1 : 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) ? 1 : 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) ? 1 : 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) ? 1 : 0;
          layers_count += (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) ? 1 : 0;

          //check for a complete cubemap
          if ( (layers_count != 6) || (width != height) )
            throw xtl::format_operation_exception ("", "Invalid DDS file '%s'. Cube-map not complete.", file_name);
        }
        else
        {
          //not a cubemap
          layers_count = 1;
        }

        // figure out what the image format is
        if (header.ddpf.dwFlags & DDSF_FOURCC) 
        {
          format = header.ddpf.dwFourCC;

          switch (format)
          {
            case FOURCC_DXT1:
              bytes_per_block = 8;
              break;
            case FOURCC_DXT3:
              bytes_per_block = 16;
              break;
            case FOURCC_DXT5:
              bytes_per_block = 16;
              break;
              //these are unsupported for now
            case FOURCC_ATI1:
            case FOURCC_ATI2:
            case FOURCC_DXT2:
            case FOURCC_DXT4:
            case FOURCC_R8G8B8:
            case FOURCC_A8R8G8B8:
            case FOURCC_X8R8G8B8:
            case FOURCC_R5G6B5:
            case FOURCC_A8:
            case FOURCC_A2B10G10R10:
            case FOURCC_A8B8G8R8:
            case FOURCC_X8B8G8R8:
            case FOURCC_A2R10G10B10:
            case FOURCC_A16B16G16R16:
            case FOURCC_L8:
            case FOURCC_A8L8:
            case FOURCC_L16:
            case FOURCC_R16F:
            case FOURCC_A16B16G16R16F:
            case FOURCC_R32F:
            case FOURCC_A32B32G32R32F:
            case FOURCC_UNKNOWN:
            case FOURCC_X1R5G5B5:
            case FOURCC_A1R5G5B5:
            case FOURCC_A4R4G4B4:
            case FOURCC_R3G3B2:
            case FOURCC_A8R3G3B2:
            case FOURCC_X4R4G4B4:
            case FOURCC_A4L4:
            case FOURCC_D16_LOCKABLE:
            case FOURCC_D32:
            case FOURCC_D24X8:
            case FOURCC_D16:
            case FOURCC_D32F_LOCKABLE:
            case FOURCC_G16R16:
            case FOURCC_G16R16F:
            case FOURCC_G32R32F:
            default:
              throw xtl::format_operation_exception ("", "Invalid DDS file '%s'. Unsupported image format.", file_name);
          }
        }

        //чтение данных

        size_t data_size = 0;

        for (size_t level = 0; level < mips_count; level++)
          data_size += GetMipLevelSize (level) * layers_count;

        if (data_size != file.Size () - file.Tell ())
          throw xtl::format_operation_exception ("", "Invalid DDS file, file contains %u bytes of data, but computeddata size is %u", file.Size () - file.Tell (), data_size);

        data.resize (data_size, false);

        blocks.reserve (layers_count * mips_count);

        xtl::uninitialized_storage<unsigned char> read_buffer (GetMipLevelSize (0));

        size_t data_offset = 0;

        for (size_t layer = 0; layer < layers_count; layer++)
        {
          size_t w = width, h = height, d = depth ? depth : 1;

          for (size_t level = 0; level < mips_count; level++)
          {
            size_t mip_level_size = GetMipLevelSize (level);

            if (file.Read (read_buffer.data (), mip_level_size) != mip_level_size)
              throw xtl::format_operation_exception ("", "Can't read data from file");

            CompressedImageBlockDesc element;

            element.offset = data_offset;
            element.size   = mip_level_size;

            blocks.push_back (element);

            // Flip & copy to actual pixel buffer
            size_t        row_size           = ((w + 3) / 4) * bytes_per_block;
            unsigned char *current_block     = read_buffer.data (),
                          *destination_block = (unsigned char*)data.data () + data_offset + ((h + 3) / 4 - 1) * row_size;

            for (size_t i = 0; i < (h + 3) / 4; i++)
            {
              memcpy (destination_block, current_block, row_size);

              switch (format)
              {
                case FOURCC_DXT1:
                  for (size_t j = 0; j < row_size / bytes_per_block; j++)
                    FlipDXT1BlockFull (destination_block + j * bytes_per_block);
                  break;
                case FOURCC_DXT3:
                  for (size_t j = 0; j < row_size / bytes_per_block; j++)
                    FlipDXT3BlockFull (destination_block + j * bytes_per_block);
                  break;
                case FOURCC_DXT5:
                  for (size_t j = 0; j < row_size / bytes_per_block; j++)
                    FlipDXT5BlockFull (destination_block + j * bytes_per_block);
                  break;
                default:
                  throw xtl::format_operation_exception ("", "Don't know how to flip data for format %d", format);
              }

              current_block     += row_size;
              destination_block -= row_size;
            }

            //reduce mip sizes
            w = ( w > 1) ? w >> 1 : 1;
            h = ( h > 1) ? h >> 1 : 1;
            d = ( d > 1) ? d >> 1 : 1;

            data_offset += mip_level_size;
          }
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("%s ('%s')", METHOD_NAME, file_name);
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
      switch (format)
      {
        case FOURCC_DXT1: return "DXT1";
        case FOURCC_DXT2: return "DXT2";
        case FOURCC_DXT3: return "DXT3";
        case FOURCC_DXT4: return "DXT4";
        case FOURCC_DXT5: return "DXT5";
        case FOURCC_ATI1: return "ATI1";
        case FOURCC_ATI2: return "ATI2";
        default:
          return "Unknown";
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
      return &blocks [0];
    }

  private:
    //Разворот по вертикали
    void FlipDXT1BlockFull(unsigned char *data)
    // A DXT1 block layout is:
    // [0-1] color0.
    // [2-3] color1.
    // [4-7] color bitmap, 2 bits per pixel.
    // So each of the 4-7 bytes represents one line, flipping a block is just
    // flipping those bytes.
    // Note that http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
    // contains an error in the last line: data[6]=data[5] is a bug!
    {
       unsigned char tmp;

       tmp=data[4];
       data[4]=data[7];
       data[7]=tmp;

       tmp=data[5];
       data[5]=data[6];
       data[6]=tmp;
    }

    void FlipDXT3BlockFull(unsigned char *block)
    // Flips a full DXT3 block in the y direction.
    {
      // A DXT3 block layout is:
      // [0-7]  alpha bitmap, 4 bits per pixel.
      // [8-15] a DXT1 block.

      // We can flip the alpha bits at the byte level (2 bytes per line).
      unsigned char tmp = block[0];
      block[0] = block[6];
      block[6] = tmp;
      tmp = block[1];
      block[1] = block[7];
      block[7] = tmp;
      tmp = block[2];
       block[2] = block[4];
      block[4] = tmp;
      tmp = block[3];
      block[3] = block[5];
      block[5] = tmp;
      // And flip the DXT1 block using the above function.
      FlipDXT1BlockFull(block + 8);
    }

    void FlipDXT5BlockFull(unsigned char *block)
    // From http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
    // Original source contained bugs; fixed here.
    {
      // A DXT5 block layout is:
      // [0]    alpha0.
      // [1]    alpha1.
      // [2-7]  alpha bitmap, 3 bits per pixel.
      // [8-15] a DXT1 block.

      // The alpha bitmap doesn't easily map lines to bytes, so we have to
      // interpret it correctly.  Extracted from
      // http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt :
      //
      //   The 6 "bits" bytes of the block are decoded into one 48-bit integer:
      //
      //     bits = bits_0 + 256 * (bits_1 + 256 * (bits_2 + 256 * (bits_3 +
      //                   256 * (bits_4 + 256 * bits_5))))
      //
      //   bits is a 48-bit unsigned integer, from which a three-bit control code
      //   is extracted for a texel at location (x,y) in the block using:
      //
      //       code(x,y) = bits[3*(4*y+x)+1..3*(4*y+x)+0]
      //
      //   where bit 47 is the most significant and bit 0 is the least
      //   significant bit.
      //QBitDump(block+2,6);

      // From Chromium (source was buggy)
      unsigned int line_0_1 = block[2] + 256 * (block[3] + 256 * block[4]);
      unsigned int line_2_3 = block[5] + 256 * (block[6] + 256 * block[7]);
      // swap lines 0 and 1 in line_0_1.
      unsigned int line_1_0 = ((line_0_1 & 0x000fff) << 12) |
      ((line_0_1 & 0xfff000) >> 12);
      // swap lines 2 and 3 in line_2_3.
      unsigned int line_3_2 = ((line_2_3 & 0x000fff) << 12) |
      ((line_2_3 & 0xfff000) >> 12);
      block[2] = line_3_2 & 0xff;
      block[3] = (line_3_2 & 0xff00) >> 8;
      block[4] = (line_3_2 & 0xff0000) >> 16;
      block[5] = line_1_0 & 0xff;
      block[6] = (line_1_0 & 0xff00) >> 8;
      block[7] = (line_1_0 & 0xff0000) >> 16;

      // And flip the DXT1 block using the above function.
      FlipDXT1BlockFull(block+8);
    }
    
    stl::string GetFourCCString (size_t code)
    {
      stl::string result;

      result.reserve (4);

      result.push_back ((char)(code & 0xFF));
      result.push_back ((char)((code >> 8) & 0xFF));
      result.push_back ((char)((code >> 16) & 0xFF));
      result.push_back ((char)((code >> 24) & 0xFF));

      return result;
    }
  
    void CheckValid (const DdsHeader& header)
    {
      if (header.dwSize != sizeof (header))
        throw xtl::format_operation_exception ("", "Invalid DDS file. Invalid header dwSize %u (must be %u)", 
                                               header.dwSize, sizeof (header));

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

    void CheckSupported (const DdsHeader& header, const DdsHeaderDx10* header_dx10)
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

        if (IsTexture3D (header, header_dx10))
        {
          throw xtl::format_not_supported_exception ("", "Unsupported DDS file. 3D textures not supported");
        }
      }      
    }

    bool IsTextureCube (const DdsHeader& header)
    {
      return (header.dwCaps2 & DDSCAPS2_CUBEMAP) != 0;
    }

    bool IsTexture3D (const DdsHeader& header, const DdsHeaderDx10* header_dx10)
    {
      if (header_dx10)
      {
        return header_dx10->resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D;
      }
      else
      {
        return (header.dwCaps2 & DDSCAPS2_VOLUME) != 0;
      }
    }

    size_t GetMipLevelSize (size_t mip)
    {
      static const char* METHOD_NAME = "media::DdsCompressedImage::GetMipLevelSize";

      if (!bytes_per_block)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't calculate mip level size, data block size unknown");

      if (mip >= mips_count)
        throw xtl::make_range_exception (METHOD_NAME, "mip", mip, 0u, mips_count);

      size_t w = width, h = height, d = depth ? depth : 1;

      for (size_t level = 0; level < mip; level++)
      {
        //reduce mip sizes
        w = (w > 1) ? w >> 1 : 1;
        h = (h > 1) ? h >> 1 : 1;
        d = (d > 1) ? d >> 1 : 1;
      }

      int block_width  = (w + 3) / 4;
      int block_height = (h + 3) / 4;

      return block_width * block_height * d * bytes_per_block;
    }

  private:
    typedef xtl::uninitialized_storage<char>      Buffer;        
    typedef stl::vector<CompressedImageBlockDesc> BlockDescArray;

  private:
    size_t         bytes_per_block; //размер блока данных
    uint32         format;          //формат изображения
    size_t         width;           //ширина изображения
    size_t         height;          //высота изображения
    size_t         depth;           //глубина изображения
    size_t         layers_count;    //количество слоёв
    size_t         mips_count;      //количество mip-уровней
    Buffer         data;            //данные изображения
    BlockDescArray blocks;          //дескрипторы блоков
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
