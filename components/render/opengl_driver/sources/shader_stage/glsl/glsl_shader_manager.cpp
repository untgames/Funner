#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    Описание профиля
*/

struct Profile
{
  const char* name;        //имя профиля
  GLenum      shader_type; //тип шейдеров
};

//доступные профили
Profile profiles [] = {
  {"glsl.ps", GL_FRAGMENT_SHADER},
  {"glsl.vs", GL_VERTEX_SHADER}
};

//количество профилей
const size_t PROFILES_COUNT = sizeof (profiles) / sizeof (*profiles);

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
  return PROFILES_COUNT;
}

/*
    Имена поддерживаемых профилей
*/

const char* GlslShaderManager::GetProfile (size_t index)
{
  if (index >= PROFILES_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::GlslShaderManager::GetProfile", "index", index, PROFILES_COUNT);

  return profiles [index].name;
}

/*
    Создание шейдера
*/

IShader* GlslShaderManager::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{    
  static const char* METHOD_NAME = "render::low_level::opengl::GlslShaderManager::CreateShader";

    //проверка корректности аргументов
    
  if (!desc.profile)
    throw xtl::make_null_argument_exception (METHOD_NAME, "desc.profile");
    
    //определение типа шейдера

  GLenum type = 0;
  size_t i;

  for (i=0; i<PROFILES_COUNT; i++)
    if (!strcmp (desc.profile, profiles [i].name))
    {
      type = profiles [i].shader_type;
      break;
    }

  if (i == PROFILES_COUNT)
    throw xtl::make_argument_exception (METHOD_NAME, "desc.profile", desc.profile);

    //создание шейдера

  return new GlslShader (GetContextManager (), type, desc, error_log);
}

ICompiledProgram* GlslShaderManager::CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction& error_log)
{
  if (!shaders)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::GlslShaderManager::CreateProgram", "shaders");

  return new GlslProgram (GetContextManager (), shaders_count, shaders, error_log);
}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    Создание менеджера GLSL шейдера
*/

IShaderManager* create_glsl_shader_manager (const ContextManager& manager)
{
  return new GlslShaderManager (manager);
}

}

}

}

