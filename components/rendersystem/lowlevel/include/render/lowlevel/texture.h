#ifndef RENDER_LOW_LEVEL_TEXTURE_HEADER
#define RENDER_LOW_LEVEL_TEXTURE_HEADER

#include <render/lowlevel/buffer.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///‘орматы пикселей
///  (дл€ DXT форматов методы SetData/GetData работают со сжатыми данными)
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_RGB8,  //red:8, green:8, blue:8
  PixelFormat_RGBA8, //red:8, green:8, blue:8, alpha:8
  PixelFormat_L8,    //luminance:8
  PixelFormat_A8,    //alpha:8
  PixelFormat_LA8,   //luminance:8, alpha:8
  PixelFormat_DXT1,  //red, green, blue, alpha - compressed
  PixelFormat_DXT3,  //red, green, blue, alpha - compressed  
  PixelFormat_DXT5,  //red, green, blue, alpha - compressed
  PixelFormat_D16,   //depth:16
  PixelFormat_D24,   //depth:24
  PixelFormat_D32,   //depth:32
  PixelFormat_S8,    //stencil:8

  PixelFormat_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///–азмерность текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureDimension
{
  TextureDimension_1D,      //одномерна€ текстура
  TextureDimension_2D,      //двумерна€ текстура
  TextureDimension_3D,      //трЄхмерна€ текстура
  TextureDimension_Cubemap, //кубическа€ текстура
  
  TextureDimension_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///ќписание текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureDesc
{
  TextureDimension dimension;             //размерность текстуры
  size_t           width;                 //ширина текстуры
  size_t           height;                //высота текстуры
  size_t           layers;                //количество слоЄв (дл€ кубических текстур), глубина (дл€ трЄхмерных текстур)
  PixelFormat      format;                //формат пикселей
  bool             generate_mips_enable;  //разрешена ли генераци€ mip-уровней
    bool             render_target;         //разрешено использование текстуры в качестве целевого буфера отрисовки //???
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///“екстура
///////////////////////////////////////////////////////////////////////////////////////////////////
class ITexture: virtual public IBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕолучение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (TextureDesc&) = 0;
};

}

}

#endif
