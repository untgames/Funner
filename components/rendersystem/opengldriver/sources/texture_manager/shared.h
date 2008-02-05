#ifndef RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER
#define RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER

#include <math.h>

#include <common/exception.h>

#include <xtl/uninitialized_storage.h>
#include <xtl/intrusive_ptr.h>

#include <render/low_level/utils.h>

#include <shared/texture_manager.h>
#include <shared/context_object.h>

#include "nv_dxt/blockdxt.h"

namespace render
{

namespace low_level
{

namespace opengl
{

///////����������� � ���������!!!
struct TextureExtensions
{
  bool has_arb_texture_compression;      //GL_ARB_texture_compression
  bool has_ext_texture_compression_s3tc; //GL_EXT_texture_compression_s3tc
  bool has_sgis_generate_mipmap;         //GL_SGIS_generate_mipmap
  
  TextureExtensions (const ContextManager& manager)
  {
    static Extension SGIS_generate_mipmap         = "GL_SGIS_generate_mipmap",
                     EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc",
                     ARB_texture_compression      = "GL_ARB_texture_compression",
                     Version_1_3                  = "GL_VERSION_1_3",
                     Version_1_4                  = "GL_VERSION_1_4";
      
    has_arb_texture_compression      = manager.IsSupported (ARB_texture_compression);
    has_ext_texture_compression_s3tc = (has_arb_texture_compression || manager.IsSupported (Version_1_3)) && manager.IsSupported (EXT_texture_compression_s3tc);
    has_sgis_generate_mipmap         = (manager.IsSupported (SGIS_generate_mipmap) || manager.IsSupported (Version_1_4));
  }
};

struct LayerDesc
{
  GLenum target;
  size_t new_index;
};

struct MipLevelDesc
{
  size_t width, height;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture : virtual public IBindableTexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture  (const ContextManager&, const TextureDesc&, GLenum target);
    ~Texture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);
    void GetDesc (BindableTextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (); //????????????
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� � ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    GLenum GetTarget    () const { return target; }
    GLuint GetTextureId () const { return texture_id; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� mip-������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetMipLevelDesc (size_t level, MipLevelDesc& desc);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetLayerDesc (size_t layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������  --- ������� �����-������������!!!
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                                      GLenum format, GLenum type, const void* buffer);
    virtual void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                                    GLenum format, size_t buffer_size, const void* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� mip-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildMipmaps (size_t x, size_t y, size_t z, size_t width, size_t height, PixelFormat format, const void* data);

  public: //???!!!private!!!
    GLenum      target;      //������� ��� ��������
    GLuint      texture_id;  //������������� ��������
    TextureDesc desc;        //���������� ��������
    size_t      mips_count;  //���������� ���-�������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1D (const ContextManager&, const TextureDesc& texture_desc);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2D (const ContextManager&, const TextureDesc&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ��������� �� ������� 2, ���������� ����� ���������� GL_EXT_texture_rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureNPOT : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureNPOT (const ContextManager&, const TextureDesc& texture_desc);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture3D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture3D (const ContextManager&, const TextureDesc&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureCubemap : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureCubemap (const ContextManager&, const TextureDesc&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetLayerDesc (size_t layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���������, ����������������� �� ��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScaledTexture: virtual public IBindableTexture, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScaledTexture (TextureManager& manager, const TextureDesc& original_desc, size_t scaled_width=0, size_t scaled_height=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);
    void GetDesc (BindableTextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);

  private:
    typedef xtl::com_ptr<IBindableTexture> TexturePtr;

  private:
    TexturePtr shadow_texture;   //������� ��������, �� ��������� �������� ������� ������
    size_t     original_width;   //������ ������������ ��������
    size_t     original_height;  //������ ������������ ��������
    float      horisontal_scale; //����������� ���������� �� �����������
    float      vertical_scale;   //����������� ���������� �� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t next_higher_power_of_two (size_t size); //��������� ��������� ������ ������� ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t texel_size           (PixelFormat format);          //������ ������� � ������
GLint  gl_internal_format   (PixelFormat format);          //���������� ������ OpenGL
GLenum gl_format            (PixelFormat format);          //������ OpenGL
GLenum gl_type              (PixelFormat format);          //��� OpenGL
bool   is_compressed_format (PixelFormat format);          //�������� �� ������ ������
bool   is_depth_format      (PixelFormat format);          //�������� �� �������� �������
size_t compressed_quad_size (PixelFormat format);          //������ ����� ������ �������� 4*4 � ������
size_t get_mips_count       (size_t size);                 //��������� ���������� mip-�������
size_t get_mips_count       (size_t width, size_t height); //��������� ���������� mip-�������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*SetTexDataFn)(size_t mip_level, size_t x, size_t y, size_t z, size_t width, size_t height, GLenum format, GLenum type, const void* data);

void scale_image_2x_down (PixelFormat format, size_t width, size_t height, const void* src, void* dest);
void scale_image         (PixelFormat format, size_t width, size_t height, size_t new_width, size_t new_height, const void* src, void* dest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � DXT �������� ��� ���������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
void   unpack_dxt (PixelFormat format, size_t width, size_t height, const void* dxt_data, void* unpacked_data); //���������� dxt
GLint  unpack_internal_format (PixelFormat format);                                                             //������������� ���������� ������ OpenGL
GLenum unpack_type            (PixelFormat format);                                                             //������������� ��� OpenGL
size_t unpack_texel_size      (PixelFormat format);                                                             //������ �������������� ������� � ������
GLenum unpack_format          (PixelFormat format);                                                             //������������� ������ OpenGL
PixelFormat unpack_pf         (PixelFormat format);                                                             //������������� ������

PixelFormat get_pixel_format (GLenum gl_format);

size_t get_compressed_image_size (PixelFormat format, size_t width, size_t height);

}

}

}

#endif
