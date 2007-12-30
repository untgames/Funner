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
  RaiseNotImplemented ("render::low_level::get_name(BindFlag)");
  return "";  
}

/*const char* get_name (BufferBindFlag param)
{
  switch (param)
  {
    case BufferBindFlag_VertexBuffer: return "VertexBuffer";
    case BufferBindFlag_IndexBuffer:  return "IndexBuffer";
    case BufferBindFlag_Texture:      return "Texture";
    case BufferBindFlag_RenderTarget: return "RenderTarget";
    case BufferBindFlag_DepthStencil: return "DepthStencil";
    
    case BufferBindFlag_VertexBuffer | BufferBindFlag_IndexBuffer:  return "VertexBuffer | IndexBuffer";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_Texture:      return "VertexBuffer | Texture";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_RenderTarget: return "VertexBuffer | RenderTarget";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_DepthStencil: return "VertexBuffer | DepthStencil";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_Texture:       return "IndexBuffer | Texture";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_RenderTarget:  return "IndexBuffer | RenderTarget";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_DepthStencil:  return "IndexBuffer | DepthStencil";
    case BufferBindFlag_Texture | BufferBindFlag_RenderTarget:      return "Texture | RenderTarget";
    case BufferBindFlag_Texture | BufferBindFlag_DepthStencil:      return "Texture | DepthStencil";

    case BufferBindFlag_VertexBuffer | BufferBindFlag_IndexBuffer | BufferBindFlag_Texture:       return "VertexBuffer | IndexBuffer | Texture";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_IndexBuffer | BufferBindFlag_RenderTarget:  return "VertexBuffer | IndexBuffer | RenderTarget";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_IndexBuffer | BufferBindFlag_DepthStencil:  return "VertexBuffer | IndexBuffer | DepthStencil";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_Texture | BufferBindFlag_RenderTarget:      return "VertexBuffer | Texture | RenderTarget";
    case BufferBindFlag_VertexBuffer | BufferBindFlag_Texture | BufferBindFlag_DepthStencil:      return "VertexBuffer | Texture | DepthStencil";    
    
    case BufferBindFlag_IndexBuffer | BufferBindFlag_VertexBuffer | BufferBindFlag_IndexBuffer:  return "VertexBuffer | IndexBuffer";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_VertexBuffer | BufferBindFlag_Texture:      return "VertexBuffer | Texture";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_VertexBuffer | BufferBindFlag_RenderTarget: return "VertexBuffer | RenderTarget";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_VertexBuffer | BufferBindFlag_DepthStencil: return "VertexBuffer | DepthStencil";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_Texture | BufferBindFlag_RenderTarget:      return "Texture | RenderTarget";
    case BufferBindFlag_IndexBuffer | BufferBindFlag_Texture | BufferBindFlag_DepthStencil:      return "Texture | DepthStencil";    
  }
} */

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
