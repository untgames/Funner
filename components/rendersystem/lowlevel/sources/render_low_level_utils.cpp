#include <render/low_level/utils.h>
#include <common/exception.h>

using namespace common;

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
      RaiseInvalidArgument ("render::low_level::get_name(CompareMode)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(UsageMode)", "param", param);
      return "";
  }
}

const char* get_name (BindFlag param)
{
  switch (param)
  {
    case 0:
      return "0";
    case BindFlag_VertexBuffer:
      return "VertexBuffer";
    case BindFlag_IndexBuffer:
      return "IndexBuffer";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer:
      return "VertexBuffer | IndexBuffer";
    case BindFlag_Texture:
      return "Texture";
    case BindFlag_VertexBuffer | BindFlag_Texture:
      return "VertexBuffer | Texture";
    case BindFlag_IndexBuffer | BindFlag_Texture:
      return "IndexBuffer | Texture";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture:
      return "VertexBuffer | IndexBuffer | Texture";
    case BindFlag_RenderTarget:
      return "RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_RenderTarget:
      return "VertexBuffer | RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_RenderTarget:
      return "IndexBuffer | RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget:
      return "VertexBuffer | IndexBuffer | RenderTarget";
    case BindFlag_Texture | BindFlag_RenderTarget:
      return "Texture | RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget:
      return "VertexBuffer | Texture | RenderTarget";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget:
      return "IndexBuffer | Texture | RenderTarget";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget:
      return "VertexBuffer | IndexBuffer | Texture | RenderTarget";
    case BindFlag_DepthStencil:
      return "DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_DepthStencil:
      return "VertexBuffer | DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_DepthStencil:
      return "IndexBuffer | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_DepthStencil:
      return "VertexBuffer | IndexBuffer | DepthStencil";
    case BindFlag_Texture | BindFlag_DepthStencil:
      return "Texture | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_DepthStencil:
      return "VertexBuffer | Texture | DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil:
      return "IndexBuffer | Texture | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_DepthStencil:
      return "VertexBuffer | IndexBuffer | Texture | DepthStencil";
    case BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "RenderTarget | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "VertexBuffer | RenderTarget | DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "IndexBuffer | RenderTarget | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "VertexBuffer | IndexBuffer | RenderTarget | DepthStencil";
    case BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "Texture | RenderTarget | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "VertexBuffer | Texture | RenderTarget | DepthStencil";
    case BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "IndexBuffer | Texture | RenderTarget | DepthStencil";
    case BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil:
      return "VertexBuffer | IndexBuffer | Texture | RenderTarget | DepthStencil";
    default:
      RaiseInvalidArgument ("render::low_level::get_name(BindFlag)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(AccessFlag)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(PixelFormat)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(TextureDimension)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(StencilOperation)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(BlendOperation)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(BlendArgument)", "param", param);
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
      RaiseInvalidArgument ("render::low_level::get_name(ColorWriteFlag)", "param", param);
      return "";    
  }
}

}

}
