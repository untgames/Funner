#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

BlendState::BlendState (const ContextManager& in_context_manager, const BlendDesc& in_desc)
  : context_manager (in_context_manager), display_list (0)
{
  SetDesc (in_desc);
}

BlendState::~BlendState ()
{
  try
  {
    if (display_list)
    {
      context_manager.MakeContextCurrent ();

      glDeleteLists (display_list, 1);
    }
  }
  catch (std::exception& exception)
  {
    context_manager.LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Установка / изменение дескриптора
*/

void BlendState::SetDesc (const BlendDesc& in_desc)
{
    //преобразование данных дескриптора
    
  GLenum blend_mode;
  GLenum src_arg, dst_arg;
  
  switch (in_desc.blend_operation)
  {
    case BlendOperation_Add:                blend_mode = GL_FUNC_ADD_EXT;      break;
    case BlendOperation_Subtraction:        blend_mode = GL_FUNC_SUBTRACT_EXT; break;
    case BlendOperation_ReverseSubtraction: blend_mode = GL_FUNC_REVERSE_SUBTRACT_EXT; break;
    case BlendOperation_Min:                blend_mode = GL_MIN_EXT; break;
    case BlendOperation_Max:                blend_mode = GL_MAX_EXT; break;
    default:
      RaiseInvalidArgument ("render::low_level::opengl::BlendState::SetDesc", "desc.blend_operation", desc.blend_operation);
      break;
  }
  
  switch (in_desc.blend_source_argument)
  {
    case BlendArgument_Zero:                    src_arg = GL_ZERO; break;
    case BlendArgument_One:                     src_arg = GL_ONE; break;
    case BlendArgument_SourceAlpha:             src_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceAlpha:      src_arg = GL_ONE_MINUS_SRC_ALPHA; break;
    case BlendArgument_DestinationColor:        src_arg = GL_DST_COLOR; break;
    case BlendArgument_DestinationAlpha:        src_arg = GL_DST_ALPHA; break;    
    case BlendArgument_InverseDestinationColor: src_arg = GL_ONE_MINUS_DST_COLOR; break;    
    case BlendArgument_InverseDestinationAlpha: src_arg = GL_ONE_MINUS_DST_ALPHA; break;
    default:
      RaiseInvalidArgument ("render::low_level::opengl::BlendState::SetDesc", "desc.blend_source_argument", desc.blend_source_argument);
      break;
  }

  switch (in_desc.blend_destination_argument)
  {
    case BlendArgument_Zero:                    dst_arg = GL_ZERO; break;
    case BlendArgument_One:                     dst_arg = GL_ONE; break;
    case BlendArgument_SourceColor:             dst_arg = GL_SRC_COLOR; break;
    case BlendArgument_SourceAlpha:             dst_arg = GL_SRC_ALPHA; break;
    case BlendArgument_InverseSourceColor:      dst_arg = GL_ONE_MINUS_SRC_COLOR; break;
    case BlendArgument_InverseSourceAlpha:      dst_arg = GL_ONE_MINUS_SRC_ALPHA; break;    
    case BlendArgument_DestinationAlpha:        dst_arg = GL_DST_ALPHA; break;
    case BlendArgument_InverseDestinationAlpha: dst_arg = GL_ONE_MINUS_DST_ALPHA; break;
    default:
      RaiseInvalidArgument ("render::low_level::opengl::BlendState::SetDesc", "desc.blend_destination_argument", desc.blend_destination_argument);
      break;    
  }
  
    //сохранение дескриптора
  
  desc = in_desc;  
  
    //выбор контекста
  
  context_manager.MakeContextCurrent ();  
  
    //запись команд в контексте OpenGL

  if (!display_list)
  {
    display_list = glGenLists (1);      
    
    if (!display_list)
      context_manager.RaiseError ("render::low_level::opengl::BlendState::SetDesc");
  }
  
  glNewList (display_list, GL_COMPILE);

  if (desc.blend_enable) glEnable  (GL_BLEND);
  else                   glDisable (GL_BLEND);

  if (GLEW_EXT_blend_minmax)
  {
    switch (blend_mode)
    {
      case GL_FUNC_ADD_EXT:
      case GL_MIN_EXT:
      case GL_MAX_EXT:
        glBlendEquationEXT (blend_mode);
        break;
      case GL_FUNC_SUBTRACT_EXT:
      case GL_FUNC_REVERSE_SUBTRACT_EXT:
        if (GLEW_EXT_blend_subtract) glBlendEquationEXT (blend_mode);
        else
        {
          context_manager.LogPrintf ("Extension GL_EXT_blend_subtract not supported. Set blend_operation=BlendOperation_Add");
          glBlendEquationEXT (GL_FUNC_ADD_EXT); //поведение по умолчанию - игнорирование отстутствия расширения

          desc.blend_operation = BlendOperation_Add;
        }

        break;
    }
  }
  else
  {
    desc.blend_operation = BlendOperation_Add;

    context_manager.LogPrintf ("Extension GL_EXT_blend_minmax not supported. Set blend_operation=BlendOperation_Add");
  }
  
  glBlendFunc (src_arg, dst_arg);
  
  size_t mask = in_desc.color_write_mask;

  glColorMask ((mask & ColorWriteFlag_Red) != 0, (mask & ColorWriteFlag_Green) != 0,
               (mask & ColorWriteFlag_Blue) != 0, (mask & ColorWriteFlag_Alpha) != 0);                 

  glEndList ();  
  
  context_manager.CheckErrors ("render::low_level::opengl::BlendState::SetDesc");
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
  if (!display_list)
    RaiseInvalidOperation ("render::low_level::opengl::BlendState::Bind", "Empty state (null display list)");

  context_manager.MakeContextCurrent ();

  glCallList (display_list);
}
