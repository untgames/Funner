#include <render/low_level/utils.h>
#include <common/exception.h>

using namespace common;

namespace render
{

namespace low_level
{

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
