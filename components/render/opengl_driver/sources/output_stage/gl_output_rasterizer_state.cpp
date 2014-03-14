#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор
*/

RasterizerState::RasterizerState (const ContextManager& manager, const RasterizerDesc& in_desc)
  : ContextObject (manager),
    desc_hash (0)
{
  SetDesc (in_desc);
}

RasterizerState::~RasterizerState ()
{
}

/*
   Биндинг состояния
*/

void RasterizerState::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerState::Bind";
  
    //проверка необходимости биндинга (кэширование состояния)
    
  const size_t current_desc_hash      = GetContextCacheValue (CacheEntry_RasterizerStateHash),
               current_scissor_enable = GetContextCacheValue (CacheEntry_ScissorEnable);

  if (current_desc_hash == desc_hash && current_scissor_enable == size_t (desc.scissor_enable))
    return;

    //установка состояния в контекст OpenGL

  MakeContextCurrent ();  
  
    //выполнение команд

  executer->ExecuteCommands ();

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //установка кэш-переменных
    
  SetContextCacheValue (CacheEntry_RasterizerStateHash, desc_hash);
  SetContextCacheValue (CacheEntry_ScissorEnable,       desc.scissor_enable);
}

/*
    Получение дескриптора
*/

void RasterizerState::GetDesc (RasterizerDesc& out_desc)
{
  out_desc = desc;
}

/*
   Изменение дескриптора
*/

void RasterizerState::SetDesc (const RasterizerDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerState::SetDesc";
  
    //проверка наличия необходимых расширений
    
  const ContextCaps& caps = GetCaps ();

  if (in_desc.multisample_enable && !caps.has_arb_multisample)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Multisampling not supported (GL_ARB_multisample extension not supported)");
    
    //проверка корректности аргументов и преобразование дескриптора
    
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  GLenum gl_fill_mode;

  switch (in_desc.fill_mode)
  {
    case FillMode_Wireframe: gl_fill_mode = GL_LINE; break;
    case FillMode_Solid:     gl_fill_mode = GL_FILL; break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.fill_mode", in_desc.fill_mode);
  }
  
#else

  switch (in_desc.fill_mode)
  {
    case FillMode_Solid:
      break;
    case FillMode_Wireframe:
      throw xtl::format_not_supported_exception (METHOD_NAME, "desc.fill_mode=%s not supported", get_name (in_desc.fill_mode));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.fill_mode", in_desc.fill_mode);
  }
  
  if (in_desc.depth_bias)
    throw xtl::format_not_supported_exception (METHOD_NAME, "in_desc.depth_bias=%d not supported", in_desc.depth_bias);
  
#endif

  switch (in_desc.cull_mode)
  {
    case CullMode_None:
    case CullMode_Front:
    case CullMode_Back:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.cull_mode", in_desc.cull_mode);
  }

    //выбор текущего контекста

  MakeContextCurrent ();  

    //запись команд в контекст OpenGL
    
  CommandListBuilder cmd_list;
  
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  cmd_list.Add (glPolygonMode, GL_FRONT_AND_BACK, gl_fill_mode);
  
#endif

  switch (in_desc.cull_mode)
  {
    case CullMode_None:
      cmd_list.Add (glDisable, GL_CULL_FACE);
      break;
    case CullMode_Front:
      cmd_list.Add (glEnable, GL_CULL_FACE);
      cmd_list.Add (glCullFace, GL_FRONT);
      break;
    case CullMode_Back:
      cmd_list.Add (glEnable, GL_CULL_FACE);
      cmd_list.Add (glCullFace, GL_BACK);
      break;
    default:
      break;
  }

  cmd_list.Add (glFrontFace, in_desc.front_counter_clockwise ? GL_CW : GL_CCW);

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  if (in_desc.depth_bias)
  {
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_FILL);
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_LINE);
    cmd_list.Add (glEnable, GL_POLYGON_OFFSET_POINT);
    cmd_list.Add (glPolygonOffset, 0.f, (float)in_desc.depth_bias);
  }
  else
  {
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_FILL);
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_LINE);
    cmd_list.Add (glDisable, GL_POLYGON_OFFSET_POINT);
  }  

#endif

  if (in_desc.scissor_enable) cmd_list.Add (glEnable, GL_SCISSOR_TEST);
  else                        cmd_list.Add (glDisable, GL_SCISSOR_TEST);

  if (in_desc.antialiased_line_enable && !in_desc.multisample_enable)
  {
#ifndef OPENGL_ES2_SUPPORT
    cmd_list.Add (glEnable, GL_POINT_SMOOTH);
    cmd_list.Add (glEnable, GL_LINE_SMOOTH);
#endif
    
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
    cmd_list.Add (glEnable, GL_POLYGON_SMOOTH);
#endif
  }
  else
  {
#ifndef OPENGL_ES2_SUPPORT
    cmd_list.Add (glDisable, GL_POINT_SMOOTH);
    cmd_list.Add (glDisable, GL_LINE_SMOOTH);
#endif

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
    cmd_list.Add (glDisable, GL_POLYGON_SMOOTH);
#endif
  }

  if (caps.has_arb_multisample && in_desc.multisample_enable)
  {
#ifndef OPENGL_ES2_SUPPORT
#ifndef OPENGL_ES_SUPPORT
    cmd_list.Add (glEnable, GL_MULTISAMPLE_ARB);
#else
    cmd_list.Add (glEnable, GL_MULTISAMPLE);
#endif
#endif
  }
    
    //создание исполнителя команд

  ExecuterPtr new_executer = cmd_list.Finish ();

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //сохранение параметров

  desc      = in_desc;
  desc_hash = crc32 (&desc, sizeof desc);
  executer  = new_executer;
  
    //оповещение о необходимости ребиндинга уровня

  StageRebindNotify (Stage_Output);
}
