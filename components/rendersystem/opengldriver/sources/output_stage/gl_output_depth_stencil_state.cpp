#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;


namespace
{

struct DepthStencilExtensions
{
  bool has_ext_stencil_two_side;
  bool has_ati_separate_stencil;
  
  DepthStencilExtensions (const ContextManager& manager)
  {
    static Extension EXT_stencil_two_side = "GL_EXT_stencil_two_side",
                     ATI_separate_stencil = "GL_ATI_separate_stencil",
                     Version_2_0          = "GL_VERSION_2_0";

    has_ext_stencil_two_side = manager.IsSupported (EXT_stencil_two_side);
    has_ati_separate_stencil = manager.IsSupported (ATI_separate_stencil) || manager.IsSupported (Version_2_0);
  }
};

}

/*
    Конструктор / деструктор
*/

DepthStencilState::DepthStencilState (const ContextManager& manager, const DepthStencilDesc& in_desc)
  : ContextObject (manager),
    desc_hash (0),
    display_list (0)
{
  SetDesc (in_desc);
}

DepthStencilState::~DepthStencilState ()
{
  try
  {
    if (display_list)
    {
      MakeContextCurrent ();

      glDeleteLists (display_list, 1);

      CheckErrors ("");
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DepthStencilState::~DepthStencilState");
    
    LogPrintf ("%s", exception.Message ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
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

/*
    Преобразование констант
*/

GLenum get_gl_compare_mode (CompareMode mode, const char* source, const char* param)
{
  switch (mode)
  {
    case CompareMode_AlwaysFail:   return GL_NEVER;
    case CompareMode_AlwaysPass:   return GL_ALWAYS;
    case CompareMode_Equal:        return GL_EQUAL;
    case CompareMode_NotEqual:     return GL_NOTEQUAL;
    case CompareMode_Less:         return GL_LESS;
    case CompareMode_LessEqual:    return GL_LEQUAL;
    case CompareMode_Greater:      return GL_GREATER;
    case CompareMode_GreaterEqual: return GL_GEQUAL;
    default:
      RaiseInvalidArgument (source, param, mode);
      return 0;
  }
}

GLenum get_gl_stencil_operation (StencilOperation operation, const char* source, const char* param)
{
  switch (operation)
  {
    case StencilOperation_Keep:      return GL_KEEP;
    case StencilOperation_Zero:      return GL_ZERO;
    case StencilOperation_Replace:   return GL_REPLACE;
    case StencilOperation_Increment: return GL_INCR;
    case StencilOperation_Decrement: return GL_DECR;
    case StencilOperation_Invert:    return GL_INVERT;
    default:
      RaiseInvalidArgument (source, param, operation);
      return 0;
  }
}

}

void DepthStencilState::SetDesc (const DepthStencilDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilState::SetDesc";
  
    //выбор контекста
    
  MakeContextCurrent ();
  
    //получение информации о доступных расширениях    
    
  DepthStencilExtensions extensions (GetContextManager ());
  
  bool has_two_side_stencil = extensions.has_ext_stencil_two_side || extensions.has_ati_separate_stencil;
  
    //преобразование данных дескриптора
    
  GLenum gl_depth_compare_mode = get_gl_compare_mode (in_desc.depth_compare_mode, METHOD_NAME, "desc.depth_compare_mode");
  bool   need_two_side_stencil = false;
  
  const StencilDesc &front_stencil = in_desc.stencil_desc [FaceMode_Front],
                    &back_stencil  = in_desc.stencil_desc [FaceMode_Back];

  if (front_stencil.stencil_compare_mode != back_stencil.stencil_compare_mode)
    need_two_side_stencil = true;

  if (front_stencil.stencil_fail_operation != back_stencil.stencil_fail_operation)
    need_two_side_stencil = true;
    
  if (front_stencil.depth_fail_operation != back_stencil.depth_fail_operation)
    need_two_side_stencil = true;
    
  if (front_stencil.stencil_pass_operation != back_stencil.stencil_pass_operation)
    need_two_side_stencil = true;

  GLenum gl_stencil_func [2] = {
    get_gl_compare_mode (front_stencil.stencil_compare_mode, METHOD_NAME, "desc.stencil_desc [FaceMode_Front].stencil_compare_mode"),
    get_gl_compare_mode (back_stencil.stencil_compare_mode, METHOD_NAME, "desc.stencil_desc [FaceMode_Back].stencil_compare_mode")
  };
  
  enum StencilOperationType
  {
    StencilOperationType_Fail,
    StencilOperationType_ZFail,
    StencilOperationType_ZPass,
    
    StencilOperationType_Num
  };
  
  GLenum gl_stencil_operation [2][StencilOperationType_Num] = {
    {
      get_gl_stencil_operation (front_stencil.stencil_fail_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Front].stencil_fail_operation"),
      get_gl_stencil_operation (front_stencil.depth_fail_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Front].depth_fail_operation"),
      get_gl_stencil_operation (front_stencil.stencil_pass_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Front].stencil_pass_operation")
    },
    {
      get_gl_stencil_operation (back_stencil.stencil_fail_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Back].stencil_fail_operation"),
      get_gl_stencil_operation (back_stencil.depth_fail_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Back].depth_fail_operation"),
      get_gl_stencil_operation (back_stencil.stencil_pass_operation, METHOD_NAME, "desc.stencil_desc [FaceMode_Back].stencil_pass_operation")
    }
  };
  
    //проверка наличия расширений
  
  if (in_desc.stencil_test_enable && need_two_side_stencil && !has_two_side_stencil)
    RaiseNotSupported (METHOD_NAME, "Unsupported configuration: desc.stencil_desc [FaceMode_Front] != desc.stencil_desc [FaceMode_Back] "
      "(GL_EXT_stencil_two_side and GL_ATI_separate_stencil not supported)");  
         
    //запись команд в контексте OpenGL

  if (!display_list)
  {
    display_list = glGenLists (1);      
    
    if (!display_list)
      RaiseError (METHOD_NAME);
  }
  
  glNewList (display_list, GL_COMPILE);
  
  if (in_desc.depth_test_enable)
  {
    glEnable    (GL_DEPTH_TEST);
    glDepthFunc (gl_depth_compare_mode);
  }
  else
  {
    glDisable (GL_DEPTH_TEST);
  }

  glDepthMask (in_desc.depth_write_enable);

  if (in_desc.stencil_test_enable)
  {
    glEnable (GL_STENCIL_TEST);
    
    if (need_two_side_stencil)
    {
      if (extensions.has_ati_separate_stencil)
      {
        if (glStencilOpSeparate)
        {
          glStencilOpSeparate (GL_FRONT, gl_stencil_operation [FaceMode_Front][0], gl_stencil_operation [FaceMode_Front][1],
                               gl_stencil_operation [FaceMode_Front][2]);
          glStencilOpSeparate (GL_BACK, gl_stencil_operation [FaceMode_Back][0], gl_stencil_operation [FaceMode_Back][1],
                               gl_stencil_operation [FaceMode_Back][2]);
        }
        else if (glStencilOpSeparateATI)
        {
          glStencilOpSeparateATI (GL_FRONT, gl_stencil_operation [FaceMode_Front][0], gl_stencil_operation [FaceMode_Front][1],
                                  gl_stencil_operation [FaceMode_Front][2]);
          glStencilOpSeparateATI (GL_BACK, gl_stencil_operation [FaceMode_Back][0], gl_stencil_operation [FaceMode_Back][1],
                                  gl_stencil_operation [FaceMode_Back][2]);
        }
      }
      else if (extensions.has_ext_stencil_two_side)
      {
        glEnable               (GL_STENCIL_TEST_TWO_SIDE_EXT);        
        glActiveStencilFaceEXT (GL_FRONT);
        glStencilOp            (gl_stencil_operation [0][0], gl_stencil_operation [0][1], gl_stencil_operation [0][2]);
        glActiveStencilFaceEXT (GL_BACK);
        glStencilOp            (gl_stencil_operation [1][0], gl_stencil_operation [1][1], gl_stencil_operation [1][2]);
      }
    }
    else
    {
      if (extensions.has_ext_stencil_two_side)
        glDisable (GL_STENCIL_TEST_TWO_SIDE_EXT);

      glStencilOp (gl_stencil_operation [0][0], gl_stencil_operation [0][1], gl_stencil_operation [0][2]);
    }
  }
  else
  {
    glDisable (GL_STENCIL_TEST);
  }

  glStencilMask (in_desc.stencil_write_mask);

  glEndList ();

    //проверка ошибок

  CheckErrors (METHOD_NAME);
  
    //сохранение дескриптора

  desc      = in_desc;
  desc_hash = crc32 (&desc, sizeof desc);

  for (int i=0; i<FaceMode_Num; i++)
    this->gl_stencil_func [i] = gl_stencil_func [i];

  this->need_two_side_stencil = need_two_side_stencil;  
}

void DepthStencilState::GetDesc (DepthStencilDesc& out_desc)
{
  out_desc = desc;
}

/*
    Установка состояния в контекст OpenGL
*/

void DepthStencilState::Bind (size_t reference)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilState::Bind";
  
    //проверка необходимости биндинга (кэширование состояния)

  ContextDataTable& state_cache        = GetContextDataTable (Stage_Output);
  size_t            &current_desc_hash = state_cache [OutputStageCache_DepthStencilStateHash],
                    &current_reference = state_cache [OutputStageCache_StencilReference];

  if (desc_hash == current_desc_hash && reference == current_reference)
    return;  

    //проверка корректности состояния

  if (!display_list)
    RaiseInvalidOperation (METHOD_NAME, "Empty state (null display list)");
    
    //установка текущего контекста

  MakeContextCurrent ();
  
    //установка состояния в контекст OpenGL

  if (desc_hash != current_desc_hash)
    glCallList (display_list);

  if (desc.stencil_test_enable)
  {
    if (need_two_side_stencil)
    {
        //получение информации о доступных расширениях
        
      DepthStencilExtensions extensions (GetContextManager ());

        //настройка функции трафарета

      if (extensions.has_ati_separate_stencil)
      {
        if (glStencilFuncSeparate)
        {
          glStencilFuncSeparate (GL_FRONT, gl_stencil_func [0], reference, desc.stencil_read_mask);
          glStencilFuncSeparate (GL_BACK, gl_stencil_func [1], reference, desc.stencil_read_mask);
        }
        else if (glStencilFuncSeparateATI)
        {
          glStencilFuncSeparateATI (GL_FRONT, gl_stencil_func [0], reference, desc.stencil_read_mask);
          glStencilFuncSeparateATI (GL_BACK, gl_stencil_func [1], reference, desc.stencil_read_mask);
        }
      }
      else if (extensions.has_ext_stencil_two_side)
      {
        glActiveStencilFaceEXT (GL_FRONT);
        glStencilFunc          (gl_stencil_func [0], reference, desc.stencil_read_mask);
        glActiveStencilFaceEXT (GL_BACK);
        glStencilFunc          (gl_stencil_func [1], reference, desc.stencil_read_mask);
      }
    }
    else
    {
      glStencilFunc (gl_stencil_func [0], reference, desc.stencil_read_mask);
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //установка кэш-переменных

  current_desc_hash = desc_hash;
  current_reference = reference;  
}
