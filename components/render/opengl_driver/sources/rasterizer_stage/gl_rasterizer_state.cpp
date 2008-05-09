#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор
*/

RasterizerState::RasterizerState (const ContextManager& manager, const RasterizerDesc& in_desc)
  : ContextObject (manager),
    desc_hash (0),
    display_list (0)
{
  SetDesc (in_desc);
}

RasterizerState::~RasterizerState ()
{
  if (!display_list)
    return;

  try
  {
      //удаление списка

    MakeContextCurrent ();

    glDeleteLists (display_list, 1);

      //проверка ошибок

    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::RasterizerState::~RasterizerState");
    
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
   Биндинг состояния
*/

void RasterizerState::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerState::Bind";
  
    //проверка необходимости биндинга (кэширование состояния)
    
  size_t& current_desc_hash = GetContextDataTable (Stage_Rasterizer)[RasterizerStageCache_RasterizerStateHash];
  
  if (current_desc_hash == desc_hash)
    return;

    //проверка корректности состояния  

  if (!display_list)
    RaiseInvalidOperation (METHOD_NAME, "Empty state (null display list)");

    //установка состояния в контекст OpenGL

  MakeContextCurrent ();  

  glCallList (display_list);

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //установка кэш-переменной

  current_desc_hash = desc_hash;
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
    RaiseNotSupported (METHOD_NAME, "Multisampling not supported (GL_ARB_multisample extension not supported)");
    
    //выбор текущего контекста

  MakeContextCurrent ();

    //создание нового списка

  if (!display_list)
  {
    display_list = glGenLists (1);

    if (!display_list)
      RaiseError (METHOD_NAME);
  }
  
    //преобразование дескриптора
    
  GLenum gl_fill_mode;

  switch (in_desc.fill_mode)
  {
    case FillMode_Wireframe: gl_fill_mode = GL_LINE; break;
    case FillMode_Solid:     gl_fill_mode = GL_FILL; break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.fill_mode", in_desc.fill_mode);
      break;
  }
  
  switch (in_desc.cull_mode)
  {
    case CullMode_None:
    case CullMode_Front:
    case CullMode_Back:
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.cull_mode", in_desc.cull_mode);
      break;
  }

    //запись команд в контекст OpenGL

  glNewList (display_list, GL_COMPILE);

  glPolygonMode (GL_FRONT_AND_BACK, gl_fill_mode);

  switch (in_desc.cull_mode)
  {
    case CullMode_None:
      glDisable (GL_CULL_FACE);
      break;
    case CullMode_Front:
      glEnable   (GL_CULL_FACE);
      glCullFace (GL_FRONT);
      break;
    case CullMode_Back:
      glEnable   (GL_CULL_FACE);
      glCullFace (GL_BACK);
      break;
  }

  if (in_desc.front_counter_clockwise) glFrontFace (GL_CW);
  else                                 glFrontFace (GL_CCW);  

  if (in_desc.depth_bias)
  {
    glEnable (GL_POLYGON_OFFSET_FILL);
    glEnable (GL_POLYGON_OFFSET_LINE);
    glEnable (GL_POLYGON_OFFSET_POINT);

    glPolygonOffset (0.f, (float)in_desc.depth_bias);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_FILL);
    glDisable (GL_POLYGON_OFFSET_LINE);
    glDisable (GL_POLYGON_OFFSET_POINT);
  }

  if (in_desc.scissor_enable) glEnable  (GL_SCISSOR_TEST);
  else                        glDisable (GL_SCISSOR_TEST);

  if (in_desc.antialiased_line_enable && !in_desc.multisample_enable)
  {
    glEnable (GL_POINT_SMOOTH);
    glEnable (GL_POLYGON_SMOOTH);
    glEnable (GL_LINE_SMOOTH);
  }
  else
  {
    glDisable (GL_POINT_SMOOTH);
    glDisable (GL_LINE_SMOOTH);
    glDisable (GL_POLYGON_SMOOTH);
  }

  if (caps.has_arb_multisample && in_desc.multisample_enable)
    glEnable (GL_MULTISAMPLE_ARB);

  glEndList ();

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //сохранение параметров

  desc      = in_desc;
  desc_hash = crc32 (&desc, sizeof desc);
}
