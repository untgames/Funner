#ifndef RENDER_MANAGER_TEXTURE_HEADER
#define RENDER_MANAGER_TEXTURE_HEADER

#include <render/manager/render_target.h>

namespace render
{

namespace manager
{

//implementation forwards
class TextureImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureDimension
{
  TextureDimension_2D,
  TextureDimension_3D,
  TextureDimension_Cubemap,
  
  TextureDimension_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_RGB8,
  PixelFormat_RGBA8,
  PixelFormat_L8,
  PixelFormat_A8,
  PixelFormat_LA8,
  PixelFormat_DXT1,
  PixelFormat_DXT3,
  PixelFormat_DXT5,
  PixelFormat_RGB_PVRTC2,
  PixelFormat_RGB_PVRTC4,
  PixelFormat_RGBA_PVRTC2,
  PixelFormat_RGBA_PVRTC4,
  PixelFormat_ATC_RGB_AMD,
  PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD,
  PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
  PixelFormat_D16,
  PixelFormat_D24X8,
  PixelFormat_D24S8,
  PixelFormat_D32,
  PixelFormat_S8,

  PixelFormat_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_name (PixelFormat); //��������� ����� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture  (const Texture&);
    ~Texture ();

    Texture& operator = (const Texture&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    ();
    void        SetName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureDimension Dimension () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format () const;
    size_t      Width  () const;
    size_t      Height () const;
    size_t      Depth  () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::RenderTarget RenderTarget (size_t layer=0, size_t mip_level=0) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const media::Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture (size_t layer, size_t mip_level, media::Image& image);
    void Capture (size_t mip_level, media::Image& image);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Texture&);
  
  private:
    Texture (TextureImpl*);
  
  private:
    TextureImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Texture&, Texture&);

}

}

#endif