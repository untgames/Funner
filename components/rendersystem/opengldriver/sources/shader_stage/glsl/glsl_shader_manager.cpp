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

Shader* GlslShaderManager::CreateShader (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslShaderManager::CreateShader";

  if (!shader_descs)
    RaiseNullArgument (METHOD_NAME, "shader_descs");

  bool used_profiles[GLSL_PROFILES_COUNT] = {0, 0};

  for (size_t i = 0; i < shaders_count; i++)
  {
    if (!strcmp (shader_descs[i].profile, GLSL_PROFILES[0]))
    {
      if (used_profiles[0])
        RaiseInvalidArgument (METHOD_NAME, "shader_descs[i].profile", shader_descs[i].profile, "Can be no more than one shader for each profile");
      used_profiles[0] = true;
    }
    else if (!strcmp (shader_descs[i].profile, GLSL_PROFILES[1]))
    {
      if (used_profiles[1])
        RaiseInvalidArgument (METHOD_NAME, "shader_descs[i].profile", shader_descs[i].profile, "Can be no more than one shader for each profile");
      used_profiles[1] = true;
    }
    else
      RaiseInvalidArgument (METHOD_NAME, "shader_descs[i].profile", shader_descs[i].profile, "Profile not supported");
  }

  return new GlslShader (GetContextManager (), shaders_count, shader_descs, error_log);
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
