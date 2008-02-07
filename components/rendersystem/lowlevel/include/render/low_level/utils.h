#ifndef RENDER_LOW_LEVEL_UTILITIES_HEADER
#define RENDER_LOW_LEVEL_UTILITIES_HEADER

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строковых описаний элементов перечислений
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_name (CompareMode);
const char* get_name (UsageMode);
const char* get_name (BindFlag);
const char* get_name (AccessFlag);
const char* get_name (PixelFormat);
const char* get_name (TextureDimension);
const char* get_name (VertexAttributeSemantic);
const char* get_name (InputDataType);
const char* get_name (InputDataFormat);
const char* get_name (LightType);
const char* get_name (LightAttenuation);
const char* get_name (TexMinFilter);
const char* get_name (TexMagFilter);
const char* get_name (TexcoordWrap);
const char* get_name (TexcoordSource);
const char* get_name (MaterialMap);
const char* get_name (FillMode);
const char* get_name (CullMode);
const char* get_name (StencilOperation);
const char* get_name (FaceMode);
const char* get_name (BlendOperation);
const char* get_name (BlendArgument);
const char* get_name (ColorWriteFlag);
const char* get_name (SwapMethod);
const char* get_name (ShaderMode);
const char* get_name (ClearFlag);
const char* get_name (PrimitiveType);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о формате пикселей
///////////////////////////////////////////////////////////////////////////////////////////////////

//получение размеров несжатого текселя и размера изображения в указанном формате
size_t get_texel_size (PixelFormat format);
size_t get_image_size (size_t width, PixelFormat format);
size_t get_image_size (size_t width, size_t height, PixelFormat format);
size_t get_image_size (size_t width, size_t height, size_t depth, PixelFormat format);

//получение размеров распакованного текселя и размера изображения после распаковки
size_t get_uncompressed_texel_size (PixelFormat format);
size_t get_uncompressed_image_size (size_t width, PixelFormat format);
size_t get_uncompressed_image_size (size_t width, size_t height, PixelFormat format);
size_t get_uncompressed_image_size (size_t width, size_t height, size_t depth, PixelFormat format);

bool is_compressed    (PixelFormat); //является ли формат сжатым
bool is_uncompressed  (PixelFormat); //является ли формат несжатым
bool is_depth_stencil (PixelFormat); //проверка на форматы буфера глубина-трафарет
bool is_color         (PixelFormat); //является ли формат "цветовым"

PixelFormat get_uncompressed_format (PixelFormat); //возвращает распакованный эквивалент переданного формата

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение количества mip-уровней
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t get_mips_count (size_t size);
size_t get_mips_count (size_t width, size_t height);
size_t get_mips_count (size_t width, size_t height, size_t depth);

}

}

#endif
