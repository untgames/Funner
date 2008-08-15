#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

FppShaderManager::FppShaderManager (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
}

/*
    Количество поддерживаемых профилей
*/

size_t FppShaderManager::GetProfilesCount ()
{
  return 1;
}

/*
    Имена поддерживаемых профилей
*/

const char* FppShaderManager::GetProfile (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::low_level::opengl::FppShaderManager::GetProfile", "index", index, 1);
    
  return "fpp";
}

/*
    Создание шейдера
*/

IShader* FppShaderManager::CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  return new FppProgram (GetContextManager (), shader_desc, error_log);
}

/*
    Создание программы
*/

ICompiledProgram* FppShaderManager::CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction&)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FppShaderManager::CreateProgram";

    //проверка корректности аргументов

  if (!shaders)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shaders");

  if (shaders_count > 1)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Multiple shaders not supported (shaders_count=%u)", shaders_count);

  FppProgram* program = cast_object<FppProgram> (*this, shaders [0], METHOD_NAME, "shaders[0]");

  if (!program)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shaders[0]");
    
    //создание не требуется поскольку программа совпадает с шейдером

  program->AddRef ();

  return program;
}

/*
    Создание менеджера шейдеров
*/

namespace render
{

namespace low_level
{

namespace opengl
{

IShaderManager* create_fpp_shader_manager (const ContextManager& context_manager)
{
  return new FppShaderManager (context_manager);
}

}

}

}
