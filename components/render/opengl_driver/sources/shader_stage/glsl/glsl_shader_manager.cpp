#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

const char*  GLSL_BASE_PROFILE = "glsl";
const char*  GLSL_PROFILES[] = {"glsl.ps", "glsl.vs"};
const size_t GLSL_PROFILES_COUNT = sizeof GLSL_PROFILES / sizeof *GLSL_PROFILES;

}

/*
   Конструктор / Деструктор
*/

GlslShaderManager::GlslShaderManager (const ContextManager& manager)
  : ContextObject (manager)
{
}

GlslShaderManager::~GlslShaderManager ()
{
}

/*
   Количество поддерживаемых профилей
*/

size_t GlslShaderManager::GetProfilesCount ()
{
  return 1;
}

/*
   Имена поддерживаемых профилей
*/

const char* GlslShaderManager::GetProfile (size_t index)
{
  if (index)
    RaiseOutOfRange ("render::low_level::opengl::GlslShaderManager::GetProfile", "index", index, 0u, 1u);

  return GLSL_BASE_PROFILE;
}

/*
   Создание шейдера
*/

Shader* GlslShaderManager::CreateShader (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  return new GlslShader (GetContextManager (), shader_desc, error_log);
}

Program* GlslShaderManager::CreateProgram (size_t shaders_count, ShaderPtr* shaders, const LogFunction& error_log)
{
  if (!shaders)
    RaiseNullArgument ("render::low_level::opengl::GlslShaderManager::CreateProgram", "shaders");

  return new GlslProgram (GetContextManager (), shaders_count, shaders, error_log);
}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
   Создание шейдер менеджеров
*/

ShaderManager* create_glsl_shader_manager (const ContextManager& manager)
{
  return new GlslShaderManager (manager);
}

GlslShaderType get_shader_type (const char* profile)
{
  if (!strcmp (profile, GLSL_PROFILES[0]))
    return GlslShaderType_Fragment;
  else if (!strcmp (profile, GLSL_PROFILES[1]))
    return GlslShaderType_Vertex;
  else
    RaiseInvalidArgument ("render::low_level::opengl::get_shader_type", "profile", profile);

  return GlslShaderType_Fragment;
}

}

}

}
