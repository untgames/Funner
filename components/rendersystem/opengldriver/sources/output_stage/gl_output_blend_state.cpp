#include "shared.h"

//добавить GL_NV_blend_squaring (gl 1.4)
//добавить blend-constant-color (gl 1.4)

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

BlendState::BlendState (const ContextManager& context_manager, const BlendDesc& in_desc)
  : ContextObject (context_manager),
    display_list (0)
{
  SetDesc (in_desc);
}

BlendState::~BlendState ()
{
  try
  {
    if (display_list)
    {
      MakeContextCurrent ();

      glDeleteLists (display_list, 1);
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::BlendState::~BlendState");
    
    LogPrintf ("%s\n", exception.Message ());
  }  
  catch (std::exception& exception)
  {
    LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Установка / изменение дескриптора
*/

namespace
{

struct BlendExtensions
{
  bool has_ext_blend_func_separate;     //GL_EXT_blend_func_separate
  bool has_ext_blend_equation_separate; //GL_EXT_blend_equation_separate
  bool has_ext_blend_minmax;            //GL_EXT_blend_minmax
  bool has_ext_blend_subtract;          //GL_EXT_blend_subtract
};

void check_blend_operation (BlendOperation operation, const BlendExtensions& ext, const char* method, const char* param)
{
  switch (operation)
  {
    case BlendOperation_Add:
      break;
    case BlendOperation_Subtraction:
    case BlendOperation_ReverseSubtraction:
      if (!ext.has_ext_blend_subtract)
        RaiseNotSupported (method, "Unsupported blend operation %s=%s (GL_EXT_blend_subtract not supported)", param, get_name (operation));

      break;
    case BlendOperation_Min:
    case BlendOperation_Max:
      if (!ext.has_ext_blend_minmax)
        RaiseNotSupported (method, "Unsupported blend operation %s=%s (GL_EXT_blend_minmax not supported)", param, get_name (operation));

      break;      
    default:
      break;
  }
}

GLenum get_blend_equation (BlendOperation operation, const char* method, const char* param)
{
  switch (operation)
  {
    case BlendOperation_Add:                return GL_FUNC_ADD_EXT;
    case BlendOperation_Subtraction:        return GL_FUNC_SUBTRACT_EXT;
    case BlendOperation_ReverseSubtraction: return GL_FUNC_REVERSE_SUBTRACT_EXT;
    case BlendOperation_Min:                return GL_MIN_EXT;
    case BlendOperation_Max:                return GL_MAX_EXT;
    default:
      RaiseInvalidArgument (method, param, operation);
      return 0;
  }
}

}

void BlendState::SetDesc (const BlendDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::BlendState::SetDesc";
  
    //выбор контекста

  MakeContextCurrent ();
  
    //определение поддержки расширений OpenGL
    
  BlendExtensions ext;
    
  ext.has_ext_blend_func_separate     = IsSupported (GlExtension_EXT_blend_func_separate) || IsSupported (GlExtension_Version_1_4);
  ext.has_ext_blend_equation_separate = IsSupported (GlExtension_EXT_blend_equation_separate) || IsSupported (GlExtension_Version_2_0);
  ext.has_ext_blend_minmax            = IsSupported (GlExtension_EXT_blend_minmax) || IsSupported (GlExtension_Version_1_2);
  ext.has_ext_blend_subtract          = IsSupported (GlExtension_EXT_blend_subtract) || IsSupported (GlExtension_Version_1_2);

    //преобразование данных дескриптора
    
  GLenum color_blend_equation = get_blend_equation (in_desc.blend_color_operation, METHOD_NAME, "desc.blend_color_operation"),
         alpha_blend_equation = get_blend_equation (in_desc.blend_alpha_operation, METHOD_NAME, "desc.blend_alpha_operation");

  GLenum src_color_arg, dst_color_arg;
  GLenum src_alpha_arg, dst_alpha_arg;
  
  switch (in_desc.blend_color_source_argument)
  {
    case BlendArgument_Zero:                    src_color_arg = GL_ZERO; break;
    case BlendArgument_One:                     src_color_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             src_color_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      src_color_arg = GL_ONE_MINUS_SRC_ALPHA; break;
    case BlendArgument_DestinationColor:        src_color_arg = GL_DST_COLOR; break;
    case BlendArgument_DestinationAlpha:        src_color_arg = GL_DST_ALPHA; break;    
    case BlendArgument_InverseDestinationColor: src_color_arg = GL_ONE_MINUS_DST_COLOR; break;    
    case BlendArgument_InverseDestinationAlpha: src_color_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_color_source_argument", get_name (in_desc.blend_color_source_argument));
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_color_source_argument", in_desc.blend_color_source_argument);
      break;
  }

  switch (in_desc.blend_color_destination_argument)
  {
    case BlendArgument_Zero:                    dst_color_arg = GL_ZERO; break;
    case BlendArgument_One:                     dst_color_arg = GL_ONE; break;
    case BlendArgument_SourceColor:             dst_color_arg = GL_SRC_COLOR; break;
    case BlendArgument_SourceAlpha:             dst_color_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceColor:      dst_color_arg = GL_ONE_MINUS_SRC_COLOR; break;
    case BlendArgument_InverseSourceAlpha:      dst_color_arg = GL_ONE_MINUS_SRC_ALPHA; break;    
    case BlendArgument_DestinationAlpha:        dst_color_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: dst_color_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_color_destination_argument", get_name (in_desc.blend_color_destination_argument));
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_color_destination_argument", in_desc.blend_color_destination_argument);
      break;    
  }

  switch (in_desc.blend_alpha_source_argument)
  {
    case BlendArgument_Zero:                    src_alpha_arg = GL_ZERO; break;
    case BlendArgument_One:                     src_alpha_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             src_color_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      src_alpha_arg = GL_ONE_MINUS_SRC_ALPHA; break;
    case BlendArgument_DestinationAlpha:        src_alpha_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: src_alpha_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_alpha_source_argument", get_name (in_desc.blend_alpha_source_argument));
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_alpha_source_argument", in_desc.blend_alpha_source_argument);
      break;
  }  
  
  switch (in_desc.blend_alpha_destination_argument)
  {
    case BlendArgument_Zero:                    dst_alpha_arg = GL_ZERO; break;
    case BlendArgument_One:                     dst_alpha_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             dst_alpha_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      dst_alpha_arg = GL_ONE_MINUS_SRC_ALPHA; break;    
    case BlendArgument_DestinationAlpha:        dst_alpha_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: dst_alpha_arg = GL_ONE_MINUS_DST_ALPHA; break;
    case BlendArgument_SourceColor:
    case BlendArgument_InverseSourceColor:
    case BlendArgument_DestinationColor:
    case BlendArgument_InverseDestinationColor:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_alpha_destination_argument", get_name (in_desc.blend_alpha_destination_argument));
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.blend_alpha_destination_argument", in_desc.blend_alpha_destination_argument);
      break;    
  }  
  
    //проверка поддержки расширений
    
  if (in_desc.blend_enable && (in_desc.color_write_mask & ColorWriteFlag_All))
  {  
    if (color_blend_equation != alpha_blend_equation && !ext.has_ext_blend_equation_separate)
      RaiseNotSupported (METHOD_NAME, "Unsupported configuration: desc.blend_color_operation=%s mismatch desc.blend_alpha_operation=%s"
        " (GL_EXT_blend_equation_separate not supported)", get_name (in_desc.blend_color_operation), get_name (in_desc.blend_alpha_operation));
    
    if (!ext.has_ext_blend_func_separate)
    {
      if (src_color_arg != src_alpha_arg)
        RaiseNotSupported (METHOD_NAME, "Unsupported configuration: desc.blend_color_source_argument=%s mismatch desc.blend_alpha_source_argument=%s"
        " (GL_EXT_blend_func_separate not supported)", get_name (in_desc.blend_color_source_argument), get_name (in_desc.blend_alpha_source_argument));

      if (dst_color_arg != dst_alpha_arg)
        RaiseNotSupported (METHOD_NAME, "Unsupported configuration: desc.blend_color_destination_argument=%s mismatch desc.blend_alpha_destination_argument=%s"
        " (GL_EXT_blend_func_separate not supported)", get_name (in_desc.blend_color_destination_argument), get_name (in_desc.blend_alpha_destination_argument));
    }

    check_blend_operation (in_desc.blend_color_operation, ext, METHOD_NAME, "desc.blend_color_operation");
    check_blend_operation (in_desc.blend_alpha_operation, ext, METHOD_NAME, "desc.blend_alpha_operation");
  }

    //сохранение дескриптора

  desc = in_desc;

    //запись команд в контексте OpenGL

  if (!display_list)
  {
    display_list = glGenLists (1);      
    
    if (!display_list)
      RaiseError (METHOD_NAME);
  }

  glNewList (display_list, GL_COMPILE);
  
  size_t mask = in_desc.color_write_mask;

  glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
               (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);
               
  if (desc.blend_enable && (mask & ColorWriteFlag_All))
  {
    glEnable (GL_BLEND);

    if      (glBlendEquationSeparate)    glBlendEquationSeparate    (color_blend_equation, alpha_blend_equation);
    else if (glBlendEquationSeparateEXT) glBlendEquationSeparateEXT (color_blend_equation, alpha_blend_equation);      
    else if (glBlendEquation)            glBlendEquation            (color_blend_equation);
    else if (glBlendEquationEXT)         glBlendEquationEXT         (color_blend_equation);

    if      (glBlendFuncSeparate)    glBlendFuncSeparate    (src_color_arg, dst_color_arg, src_alpha_arg, dst_alpha_arg);
    else if (glBlendFuncSeparateEXT) glBlendFuncSeparateEXT (src_color_arg, dst_color_arg, src_alpha_arg, dst_alpha_arg);
    else                             glBlendFunc            (src_color_arg, dst_color_arg);      
  }
  else
  {
    glDisable (GL_BLEND);
  }

  glEndList ();  
  
    //проверка ошибок
  
  CheckErrors (METHOD_NAME);
}

void BlendState::GetDesc (BlendDesc& out_desc)
{
  out_desc = desc;
}

/*
    Установка состояния в контекст OpenGL
*/

void BlendState::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::BlendState::Bind";

  if (!display_list)
    RaiseInvalidOperation (METHOD_NAME, "Empty state (null display list)");

  MakeContextCurrent ();

  glCallList (display_list);
  
  CheckErrors (METHOD_NAME);
}
