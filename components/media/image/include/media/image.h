#ifndef MEDIALIB_IMAGE_HEADER
#define MEDIALIB_IMAGE_HEADER

#include <stl/auto_ptr.h>

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

namespace media
{

//implementation forwards
class ImageImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_Default,
  PixelFormat_RGB8,
  PixelFormat_RGB16,
  PixelFormat_BGR8,
  PixelFormat_RGBA8,
  PixelFormat_RGBA16,
  PixelFormat_BGRA8,
  PixelFormat_L8,
  PixelFormat_A8,
  PixelFormat_LA8,
  
  PixelFormat_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CubemapLayer
{
  CubemapLayer_PositiveX,
  CubemapLayer_NegativeX,
  CubemapLayer_PositiveY,
  CubemapLayer_NegativeY,
  CubemapLayer_PositiveZ,
  CubemapLayer_NegativeZ
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_format_name     (PixelFormat); //��� �������
size_t      get_bits_per_pixel  (PixelFormat); //���������� ��� �� �������
size_t      get_bytes_per_pixel (PixelFormat); //���������� ���� �� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� ���� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LayersCloneMode
{
  LayersCloneMode_Copy,     //���� ����������
  LayersCloneMode_Capture,  //���� ������������� (�������� ����� ���������� �������� ������������)
  
  LayersCloneMode_Default = LayersCloneMode_Copy
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Image
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
             Image  ();
             Image  (const Image& source, PixelFormat format = PixelFormat_Default);
             Image  (size_t width, size_t height, size_t depth, PixelFormat format, const void* data = 0);
             Image  (size_t layers_count, Image* layers, LayersCloneMode clone_mode = LayersCloneMode_Default);
    explicit Image  (const char* file_name, PixelFormat format = PixelFormat_Default);
             ~Image ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Image& operator = (const Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Image Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name, PixelFormat format = PixelFormat_Default, const char* format_specific_flags = "");

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������/���������� �� ��������� (� ������� ������ ������!!!)
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void DefaultLoader (const char* file_name, Image& image);
    static void CubemapLoader (const char* file_name, Image& image);
    static void SkyBoxLoader  (const char* file_name, Image& image);
    static void DefaultSaver  (const char* file_name, const Image& image, const char* format_specific_flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (PixelFormat new_format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () const;
    size_t Height () const;
    size_t Depth  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize (size_t width, size_t height, size_t depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void*       Bitmap (size_t z = 0);
    const void* Bitmap (size_t z = 0) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ � �������������� ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data);
    void GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Image&);
    
  private:
    Image (ImageImpl*);

  private:
    ImageImpl* impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Image&, Image&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ������������ �������� ������ ��������
//////////////////////////////////////////////////////////////////////////////////////////////////
void crop_by_alpha (const Image& image, size_t crop_alpha, size_t& crop_x, size_t& crop_y, size_t& crop_width, size_t& crop_height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<
  void (const char* file_name, Image& image),
  void (const char* file_name, const Image& image, const char* format_specific_flags)>
ImageManager;

}

#endif
