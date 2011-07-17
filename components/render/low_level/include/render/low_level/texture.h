#ifndef RENDER_LOW_LEVEL_TEXTURE_HEADER
#define RENDER_LOW_LEVEL_TEXTURE_HEADER

#include <render/low_level/common.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Форматы пикселей
///  (для DXT форматов методы SetData/GetData работают со сжатыми данными)
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_RGB8,        //red:8, green:8, blue:8
  PixelFormat_RGBA8,       //red:8, green:8, blue:8, alpha:8
  PixelFormat_L8,          //luminance:8
  PixelFormat_A8,          //alpha:8
  PixelFormat_LA8,         //luminance:8, alpha:8
  PixelFormat_DXT1,        //red, green, blue, alpha - compressed
  PixelFormat_DXT3,        //red, green, blue, alpha - compressed  
  PixelFormat_DXT5,        //red, green, blue, alpha - compressed
  PixelFormat_RGB_PVRTC2,  //red, green, blue - compressed
  PixelFormat_RGB_PVRTC4,  //red, green, blue - compressed
  PixelFormat_RGBA_PVRTC2, //red, green, blue, alpha - compressed
  PixelFormat_RGBA_PVRTC4, //red, green, blue, alpha - compressed
  PixelFormat_D16,         //depth:16
  PixelFormat_D24X8,       //depth:24, x:8
  PixelFormat_D24S8,       //depth:24, stencil:8
  PixelFormat_D32,         //depth:32
  PixelFormat_S8,          //stencil:8

  PixelFormat_Num,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размерность текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureDimension
{
  TextureDimension_1D,      //одномерная текстура
  TextureDimension_2D,      //двумерная текстура
  TextureDimension_3D,      //трёхмерная текстура
  TextureDimension_Cubemap, //кубическая текстура
  
  TextureDimension_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureDesc
{
  TextureDimension dimension;             //размерность текстуры
  size_t           width;                 //ширина текстуры
  size_t           height;                //высота текстуры
  size_t           layers;                //количество слоёв (для кубических текстур), глубина (для трёхмерных текстур)
  PixelFormat      format;                //формат пикселей
  bool             generate_mips_enable;  //разрешена ли генерация mip-уровней
  size_t           access_flags;          //флаги доступа к буферу
  size_t           bind_flags;            //флаги биндинга буфера
  UsageMode        usage_mode;            //режим использования буфера
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureData
{
  const void*   data;  //текстурные данные
  const size_t* sizes; //размеры мип-уровней (если равен 0, то расчитывается автоматически)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class ITexture : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (TextureDesc&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с данными
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer) = 0;
    virtual void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer) = 0;
};

}

}

#endif
