#include <stdint.h>

#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/file.h>

#include <media/compressed_image.h>

using namespace common;
using namespace media;

namespace
{

//gl internal formats
const int ETC1_RGB8_OES = 0x8D64;

/*
    KTX format description
*/

const unsigned char KTX_IDENTIFIER [] = { 0xab, 0x4b, 0x54, 0x58, 0x20, 0x31, 0x31, 0xbb, 0x0d, 0x0a, 0x1a, 0x0a }; //ktx header identifier

#pragma pack(push,1)

//KTX file format header (https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/)
struct KtxHeader
{
  char     identifier [12];         //identifier
  uint32_t endianness;              //endianness marker
  uint32_t gl_type;                 //OpenGL API type
  uint32_t gl_type_size;            //data type size for endianness conversion
  uint32_t gl_format;               //OpenGL API format
  uint32_t gl_internal_format;      //OpenGL API internal format
  uint32_t gl_base_internal_format; //OpenGL API base internal format
  uint32_t width;                   //level 0 width
  uint32_t height;                  //level 0 height
  uint32_t depth;                   //level 0 depth
  uint32_t array_elements_count;    //array elements count for array texture
  uint32_t faces_count;             //cubemap faces count
  uint32_t mipmap_levels_count;     //mipmap levels count
  uint32_t key_value_data_size;     //bytes of key-value data block
};

#pragma pack(pop)

/*
    Image data contained in KTX format
*/

class KtxCompressedImage: public ICustomCompressedImage
{
  public:
///Loading ktx file
    KtxCompressedImage (const char* file_name)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");

        InputFile file (file_name);

          //process file header
        
        KtxHeader header;
        
        if (file.Read (&header, sizeof (KtxHeader)) != sizeof (KtxHeader))
          throw xtl::format_operation_exception ("", "Invalid KTX file '%s'. Error at read header (%u bytes from start of file)", file_name, sizeof (KtxHeader));
          
        if (memcmp (header.identifier, KTX_IDENTIFIER, sizeof (KTX_IDENTIFIER)))
          throw xtl::format_operation_exception ("", "Invalid KTX file '%s'. Wrong identifier", file_name);

        if (header.endianness != 0x04030201)
          throw xtl::format_not_supported_exception ("", "Endianness conversion not supported");
        
        if (header.gl_type)
          throw xtl::format_not_supported_exception ("", "KTX with uncompressed data not supported (gl_type != 0)");

        if (header.gl_type_size > 1) //this should be != 1 according to specification, but arm's etcpack sets this field to 0
          throw xtl::format_not_supported_exception ("", "KTX with uncompressed data not supported (gl_type_size > 1)");

        if (header.gl_format)
          throw xtl::format_not_supported_exception ("", "KTX with uncompressed data not supported (gl_format != 0)");

        switch (header.gl_internal_format)
        {
          case ETC1_RGB8_OES: format = "etc1"; break;
          default:
            throw xtl::format_not_supported_exception ("", "gl internal format %d not supported", header.gl_internal_format);
        }

        width        = header.width;
        height       = header.height;
        layers_count = header.depth ? header.depth : header.faces_count;

        if (header.array_elements_count)
          throw xtl::format_not_supported_exception ("", "Array textures not supported");

        if (header.depth && header.faces_count > 0)
          throw xtl::format_not_supported_exception ("", "3d multilayer textures not supported");

        file.Seek (header.key_value_data_size, FileSeekMode_Current);

          //reading data
        unsigned int data_length = (unsigned int)(file.Size () - file.Tell ());

        data.reserve (data_length, false);
          
        mip_levels.reserve (header.mipmap_levels_count);

        for (uint32_t i = 0; i < header.mipmap_levels_count; i++)
        {
          uint32_t mip_size;

          if (file.Read (&mip_size, sizeof (mip_size)) != sizeof (mip_size))
            throw xtl::format_operation_exception ("", "Invalid KTX file '%s'. Error at read mip level %u size", file_name, i);

          CompressedImageBlockDesc mip_desc;

          mip_desc.offset = data.size ();
          mip_desc.size   = mip_size;

          mip_levels.push_back (mip_desc);

          data.resize (data.size () + mip_size);

          if (file.Read (data.data () + mip_desc.offset, mip_size) != mip_size)
            throw xtl::format_operation_exception ("", "Invalid KTX file '%s'. Error at read mip level %u data", file_name, i);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::KtxCompressedImage::KtxCompressedImage");
        throw;
      }
    }
  
///Image width
    unsigned int Width ()
    {
      return width;
    }
    
///Image height
    unsigned int Height ()
    {
      return height;
    }

///Layers count
    unsigned int LayersCount ()
    {
      return layers_count;
    }

///Mip levels count
    unsigned int MipsCount ()
    {
      return (unsigned int)mip_levels.size ();
    }
    
///Format name
    const char* Format ()
    {
      return format.c_str ();
    }

///Image data
    const void* Data ()
    {
      return data.data ();
    }

///Mip levels data
    const CompressedImageBlockDesc* Blocks ()
    {
      return &mip_levels [0];
    }
    
  private:
    typedef xtl::uninitialized_storage<char>      Buffer;        
    typedef stl::vector<CompressedImageBlockDesc> MipLevelArray;

  private:
    stl::string   format;       //format name
    uint32_t      width;        //image width
    uint32_t      height;       //image height
    uint32_t      layers_count; //image depth
    Buffer        data;         //image data
    MipLevelArray mip_levels;   //mip levels
};

/*
   Component
*/

class Component
{
  public:
    Component ()
    {
      CompressedImageManager::RegisterLoader ("ktx", &Component::CreateKtxImage);
    }

  private:
    static ICustomCompressedImage* CreateKtxImage (const char* file_name)
    {
      return new KtxCompressedImage (file_name);
    }
};


}

namespace components
{

namespace ktx_loader
{

extern "C"
{

common::ComponentRegistrator<Component> KtxLoader ("media.compressed_image.ktx_loader");

}

}

}
