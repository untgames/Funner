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

}

}

#endif
