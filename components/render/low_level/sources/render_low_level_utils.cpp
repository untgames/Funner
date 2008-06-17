#include <math.h>
#include <render/low_level/utils.h>
#include <xtl/common_exceptions.h>

namespace render
{

namespace low_level
{

const char* get_name (CompareMode param)
{
  switch (param)
  {
    case CompareMode_AlwaysFail:   return "CompareMode_AlwaysFail";
    case CompareMode_AlwaysPass:   return "CompareMode_AlwaysPass";
    case CompareMode_Equal:        return "CompareMode_Equal";
    case CompareMode_NotEqual:     return "CompareMode_NotEqual";
    case CompareMode_Less:         return "CompareMode_Less";
    case CompareMode_LessEqual:    return "CompareMode_LessEqual";
    case CompareMode_Greater:      return "CompareMode_Greater";
    case CompareMode_GreaterEqual: return "CompareMode_GreaterEqual";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(CompareMode)", "param", param);
      return "";
  }
}

const char* get_name (UsageMode param)
{
  switch (param)
  {
    case UsageMode_Default: return "UsageMode_Default";
    case UsageMode_Static:  return "UsageMode_Static";
    case UsageMode_Dynamic: return "UsageMode_Dynamic";
    case UsageMode_Stream:  return "UsageMode_Stream";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(UsageMode)", "param", param);
      return "";
  }
}

//сгенерировано автоматически, не изменять руками (см. утилиту get_name_bind_flags_gen.cpp)
const char* get_name (BindFlag param)
{
  switch (param)
  {
    case 0: return "0";
    case BindFlag_VertexBuffer: return "BindFlag_VertexBuffer";
    case BindFlag_IndexBuffer: return "BindFlag_IndexBuffer";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer";
    case BindFlag_ConstantBuffer: return "BindFlag_ConstantBuffer";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer";
    case BindFlag_Texture: return "BindFlag_Texture";
    case BindFlag_VertexBuffer | BindFlag_Texture: return "BindFlag_VertexBuffer | BindFlag_Texture";
    case BindFlag_IndexBuffer | BindFlag_Texture: return "BindFlag_IndexBuffer | BindFlag_Texture";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture";
    case BindFlag_ConstantBuffer | BindFlag_Texture: return "BindFlag_ConstantBuffer | BindFlag_Texture";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture";
    case BindFlag_RenderTarget: return "BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_RenderTarget: return "BindFlag_IndexBuffer | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget";
    case BindFlag_ConstantBuffer | BindFlag_RenderTarget: return "BindFlag_ConstantBuffer | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget";
    case BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget";
    case BindFlag_DepthStencil: return "BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_DepthStencil";
    case BindFlag_ConstantBuffer | BindFlag_DepthStencil: return "BindFlag_ConstantBuffer | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_DepthStencil";
    case BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_DepthStencil";
    case BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil: return "BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(BindFlag)", "param", param);
      return "";
  }
}

const char* get_name (AccessFlag param)
{
  switch (param)
  {
    case 0:                                  return "no_access";
    case AccessFlag_Read:                    return "read";
    case AccessFlag_Write:                   return "write";
    case AccessFlag_Read | AccessFlag_Write: return "read | write";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(AccessFlag)", "param", param);
      return "";
  }
}

const char* get_name (PixelFormat param)
{
  switch (param)
  {
    case PixelFormat_RGB8:  return "PixelFormat_RGB8";
    case PixelFormat_RGBA8: return "PixelFormat_RGBA8";
    case PixelFormat_L8:    return "PixelFormat_L8";
    case PixelFormat_A8:    return "PixelFormat_A8";
    case PixelFormat_LA8:   return "PixelFormat_LA8";
    case PixelFormat_DXT1:  return "PixelFormat_DXT1";
    case PixelFormat_DXT3:  return "PixelFormat_DXT3";
    case PixelFormat_DXT5:  return "PixelFormat_DXT5";
    case PixelFormat_D16:   return "PixelFormat_D16";
    case PixelFormat_D24X8: return "PixelFormat_D24X8";
    case PixelFormat_D24S8: return "PixelFormat_D24S8";
    case PixelFormat_S8:    return "PixelFormat_S8";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(PixelFormat)", "param", param);
      return "";
  }
}

const char* get_name (TextureDimension param)
{
  switch (param)
  {
    case TextureDimension_1D:      return "TextureDimension_1D";
    case TextureDimension_2D:      return "TextureDimension_2D";
    case TextureDimension_3D:      return "TextureDimension_3D";
    case TextureDimension_Cubemap: return "TextureDimension_Cubemap";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(TextureDimension)", "param", param);
      return "";
  }
}

const char* get_name (StencilOperation param)
{
  switch (param)
  {
    case StencilOperation_Keep:       return "StencilOperation_Keep";
    case StencilOperation_Zero:       return "StencilOperation_Zero";
    case StencilOperation_Replace:    return "StencilOperation_Replace";
    case StencilOperation_Increment:  return "StencilOperation_Increment";
    case StencilOperation_Decrement:  return "StencilOperation_Decrement";
    case StencilOperation_Invert:     return "StencilOperation_Invert";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(StencilOperation)", "param", param);
      return "";
  }
}

const char* get_name (BlendOperation param)
{
  switch (param)
  {
    case BlendOperation_Add:                return "BlendOperation_Add";
    case BlendOperation_Subtraction:        return "BlendOperation_Subtraction";
    case BlendOperation_ReverseSubtraction: return "BlendOperation_ReverseSubtraction";
    case BlendOperation_Min:                return "BlendOperation_Min";
    case BlendOperation_Max:                return "BlendOperation_Max";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(BlendOperation)", "param", param);
      return "";
  }
}

const char* get_name (BlendArgument param)
{
  switch (param)
  {
    case BlendArgument_Zero:                    return "BlendArgument_Zero";
    case BlendArgument_One:                     return "BlendArgument_One";
    case BlendArgument_SourceColor:             return "BlendArgument_SourceColor";
    case BlendArgument_SourceAlpha:             return "BlendArgument_SourceAlpha";
    case BlendArgument_InverseSourceColor:      return "BlendArgument_InverseSourceColor";
    case BlendArgument_InverseSourceAlpha:      return "BlendArgument_InverseSourceAlpha";
    case BlendArgument_DestinationColor:        return "BlendArgument_DestinationColor";
    case BlendArgument_DestinationAlpha:        return "BlendArgument_DestinationAlpha";
    case BlendArgument_InverseDestinationColor: return "BlendArgument_InverseDestinationColor";
    case BlendArgument_InverseDestinationAlpha: return "BlendArgument_InverseDestinationAlpha";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(BlendArgument)", "param", param);
      return "";
  }  
}

const char* get_name (ColorWriteFlag param)
{
  switch (param)
  {
    case 0:                                                                 return "empty";
    case ColorWriteFlag_Red:                                                return "red";
    case ColorWriteFlag_Green:                                              return "green";
    case ColorWriteFlag_Blue:                                               return "blue";
    case ColorWriteFlag_Alpha:                                              return "alpha";
    case ColorWriteFlag_Red | ColorWriteFlag_Green:                         return "red | green";
    case ColorWriteFlag_Red | ColorWriteFlag_Blue:                          return "red | blue";
    case ColorWriteFlag_Red | ColorWriteFlag_Alpha:                         return "red | alpha";
    case ColorWriteFlag_Green | ColorWriteFlag_Blue:                        return "green | blue";
    case ColorWriteFlag_Green | ColorWriteFlag_Alpha:                       return "green | alpha";
    case ColorWriteFlag_Blue | ColorWriteFlag_Alpha:                        return "blue | alpha";
    case ColorWriteFlag_Red | ColorWriteFlag_Green | ColorWriteFlag_Blue:   return "red | green | blue";
    case ColorWriteFlag_Red | ColorWriteFlag_Green | ColorWriteFlag_Alpha:  return "red | green | alpha";
    case ColorWriteFlag_Red | ColorWriteFlag_Blue | ColorWriteFlag_Alpha:   return "red | blue | alpha";        
    case ColorWriteFlag_Green | ColorWriteFlag_Blue | ColorWriteFlag_Alpha: return "green | blue | alpha";    
    case ColorWriteFlag_Red | ColorWriteFlag_Green | ColorWriteFlag_Blue | ColorWriteFlag_Alpha:  return "red | green | blue | alpha";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(ColorWriteFlag)", "param", param);
      return "";    
  }
}

const char* get_name (InputDataType param)
{
  switch (param)
  {
    case InputDataType_Byte:    return "InputDataType_Byte";
    case InputDataType_UByte:   return "InputDataType_UByte";
    case InputDataType_Short:   return "InputDataType_Short";
    case InputDataType_UShort:  return "InputDataType_UShort";
    case InputDataType_Int:     return "InputDataType_Int";
    case InputDataType_UInt:    return "InputDataType_UInt";
    case InputDataType_Float:   return "InputDataType_Float";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(InputDataType)", "param", param);
      return "";    
  }
}

const char* get_name (InputDataFormat param)
{
  switch (param)
  {
    case InputDataFormat_Vector1: return "InputDataFormat_Vector1";
    case InputDataFormat_Vector2: return "InputDataFormat_Vector2";
    case InputDataFormat_Vector3: return "InputDataFormat_Vector3";
    case InputDataFormat_Vector4: return "InputDataFormat_Vector4";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(InputDataFormat)", "param", param);
      return "";    
  }
}

const char* get_name (VertexAttributeSemantic param)
{
  switch (param)
  {
    case VertexAttributeSemantic_Position:  return "VertexAttributeSemantic_Position";
    case VertexAttributeSemantic_Normal:    return "VertexAttributeSemantic_Normal";
    case VertexAttributeSemantic_Color:     return "VertexAttributeSemantic_Color";
    case VertexAttributeSemantic_TexCoord0: return "VertexAttributeSemantic_TexCoord0";
    case VertexAttributeSemantic_TexCoord1: return "VertexAttributeSemantic_TexCoord1";
    case VertexAttributeSemantic_TexCoord2: return "VertexAttributeSemantic_TexCoord2";
    case VertexAttributeSemantic_TexCoord3: return "VertexAttributeSemantic_TexCoord3";
    case VertexAttributeSemantic_TexCoord4: return "VertexAttributeSemantic_TexCoord4";
    case VertexAttributeSemantic_TexCoord5: return "VertexAttributeSemantic_TexCoord5";
    case VertexAttributeSemantic_TexCoord6: return "VertexAttributeSemantic_TexCoord6";
    case VertexAttributeSemantic_TexCoord7: return "VertexAttributeSemantic_TexCoord7";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(VertexAttributeSemantic)", "param", param);
      return "";    
  }
}

const char* get_name (TexMinFilter param)
{
  switch (param)
  {
    case TexMinFilter_Default:         return "TexMinFilter_Default";
    case TexMinFilter_Point:           return "TexMinFilter_Point";
    case TexMinFilter_Linear:          return "TexMinFilter_Linear";
    case TexMinFilter_PointMipPoint:   return "TexMinFilter_PointMipPoint";
    case TexMinFilter_LinearMipPoint:  return "TexMinFilter_LinearMipPoint";
    case TexMinFilter_PointMipLinear:  return "TexMinFilter_PointMipLinear";
    case TexMinFilter_LinearMipLinear: return "TexMinFilter_LinearMipLinear";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(TexMinFilter)", "param", param);
      return "";    
  }
}

const char* get_name (TexMagFilter param)
{
  switch (param)
  {
    case TexMagFilter_Default: return "TexMagFilter_Default";
    case TexMagFilter_Point:   return "TexMagFilter_Point";
    case TexMagFilter_Linear:  return "TexMagFilter_Linear";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(TexMagFilter)", "param", param);
      return "";    
  }
}

const char* get_name (TexcoordWrap param)
{
  switch (param)
  {
    case TexcoordWrap_Repeat:        return "TexcoordWrap_Repeat";
    case TexcoordWrap_Mirror:        return "TexcoordWrap_Mirror";
    case TexcoordWrap_Clamp:         return "TexcoordWrap_Clamp";
    case TexcoordWrap_ClampToBorder: return "TexcoordWrap_ClampToBorder";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(TexcoordWrap)", "param", param);
      return "";    
  }
}

const char* get_name (ProgramParameterType param)
{
  switch (param)
  {
    case ProgramParameterType_Int:        return "ProgramParameterType_Int";
    case ProgramParameterType_Float:      return "ProgramParameterType_Float";
    case ProgramParameterType_Int2:       return "ProgramParameterType_Int2";
    case ProgramParameterType_Float2:     return "ProgramParameterType_Float2";
    case ProgramParameterType_Int3:       return "ProgramParameterType_Int3";
    case ProgramParameterType_Float3:     return "ProgramParameterType_Float3";
    case ProgramParameterType_Int4:       return "ProgramParameterType_Int4";
    case ProgramParameterType_Float4:     return "ProgramParameterType_Float4";
    case ProgramParameterType_Float2x2:   return "ProgramParameterType_Float2x2";
    case ProgramParameterType_Float3x3:   return "ProgramParameterType_Float3x3";
    case ProgramParameterType_Float4x4:   return "ProgramParameterType_Float4x4";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(ProgramParameterType)", "param", param);
      return "";
  }
}

const char* get_name (FillMode param)
{
  switch (param)
  {
    case FillMode_Wireframe: return "FillMode_Wireframe";
    case FillMode_Solid:     return "FillMode_Solid";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(FillMode)", "param", param);
      return "";    
  }
}

const char* get_name (CullMode param)
{
  switch (param)
  {
    case CullMode_None:  return "CullMode_None";
    case CullMode_Front: return "CullMode_Front";
    case CullMode_Back:  return "CullMode_Back";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(CullMode)", "param", param);
      return "";    
  }
}

const char* get_name (PrimitiveType param)
{
  switch (param)
  {
    case PrimitiveType_PointList:      return "PrimitiveType_PointList";
    case PrimitiveType_LineList:       return "PrimitiveType_LineList";
    case PrimitiveType_LineStrip:      return "PrimitiveType_LineStrip";
    case PrimitiveType_TriangleList:   return "PrimitiveType_TriangleList";
    case PrimitiveType_TriangleStrip:  return "PrimitiveType_TriangleStrip";
    case PrimitiveType_TriangleFan:    return "PrimitiveType_TriangleFan";
    default:
      throw xtl::make_argument_exception ("render::low_level::get_name(PrimitiveType)", "param", param);
      return "";
  }
}

/*
    Получение параметров формата пикселей
*/

//получение размеров несжатого текселя
size_t get_texel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    return 1;
    case PixelFormat_LA8:
    case PixelFormat_D16:   return 2;
    case PixelFormat_RGB8:  return 3;
    case PixelFormat_RGBA8:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return 4;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      throw xtl::make_argument_exception ("render::low_level::get_texel_size", "format", get_name (format), "No texel size semantic in compressed pixel format");
      return 0;
    default:
      throw xtl::make_argument_exception ("render::low_level::get_texel_size", "format");
      return 0;
  }
}

//получение размера изображения
size_t get_image_size (size_t width, size_t height, size_t depth, PixelFormat format)
{
  static const size_t DXT_BLOCK_SIZE = 16;

  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:
    case PixelFormat_LA8:
    case PixelFormat_D16:
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return width * height * depth * get_texel_size (format);
    case PixelFormat_DXT1:  return width * height * depth * 8 / DXT_BLOCK_SIZE;
    case PixelFormat_DXT3:  return width * height * depth * 16 / DXT_BLOCK_SIZE;
    case PixelFormat_DXT5:  return width * height * depth * 16 / DXT_BLOCK_SIZE;
      throw xtl::make_argument_exception ("render::low_level::get_texel_size", "format", get_name (format), "No texel size semantic in compressed pixel format");
      return 0;
    default:
      throw xtl::make_argument_exception ("render::low_level::get_texel_size", "format");
      return 0;    
  }
}

size_t get_image_size (size_t width, size_t height, PixelFormat format)
{
  return get_image_size (width, height, 1, format);
}

size_t get_image_size (size_t width, PixelFormat format)
{
  return get_image_size (width, 1, 1, format);
}

//проверка является ли формат сжатым
bool is_compressed (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return true;
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return false;  
    default:
      throw xtl::make_argument_exception ("render::low_level::is_compressed", "format", format);
      return false;
  }
}

//проверка является ли формат несжатым
bool is_uncompressed (PixelFormat format)
{
  return !is_compressed (format);
}

//проверка на форматы буфера глубина-трафарет
bool is_depth_stencil (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_S8:
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return true;   
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_L8:    
    case PixelFormat_A8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: return false;
    default:
      throw xtl::make_argument_exception ("render::low_level::is_depth_stencil", "format", format);
      return false;
  }
}

//проверка является ли формат "цветовым"
bool is_color (PixelFormat format)
{
  return !is_depth_stencil (format);
}

//возвращает распакованный эквивалент переданного формата
PixelFormat get_uncompressed_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    return format;
    case PixelFormat_DXT1:  return PixelFormat_RGB8;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return PixelFormat_RGBA8;
    default:
      throw xtl::make_argument_exception ("render::low_level::get_unpacked_format", "format", format);
      return (PixelFormat)0;
  }
}

//получение размеров распакованного текселя
size_t get_uncompressed_texel_size (PixelFormat format)
{
  return get_texel_size (get_uncompressed_format (format));
}

//получение размера изображения после распаковки
size_t get_uncompressed_image_size (size_t width, PixelFormat format)
{
  return get_image_size (width, get_uncompressed_format (format));
}

size_t get_uncompressed_image_size (size_t width, size_t height, PixelFormat format)
{
  return get_image_size (width, height, get_uncompressed_format (format));
}

size_t get_uncompressed_image_size (size_t width, size_t height, size_t depth, PixelFormat format)
{
  return get_image_size (width, height, depth, get_uncompressed_format (format));
}

/*
    Получение количества mip-уровней
*/

namespace
{

size_t get_max_size (size_t a, size_t b)
{
  return a > b ? a : b;
}

}

size_t get_mips_count (size_t size)
{
  return (size_t)(log ((float)size) / log (2.f)) + 1;
}

size_t get_mips_count (size_t width, size_t height)
{
  return get_mips_count (get_max_size (width, height));
}

size_t get_mips_count (size_t width, size_t height, size_t depth)
{
  return get_mips_count (get_max_size (width, get_max_size (height, depth)));
}

}

}
