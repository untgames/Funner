#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Описание реализации менеджера шейдеров
*/

struct ShaderManager::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

ShaderManager::ShaderManager (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

ShaderManager::~ShaderManager ()
{
}

/*
    Создание шейдеров
*/

IProgramParametersLayout* ShaderManager::CreateProgramParametersLayout (const ProgramParametersLayoutDesc&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IProgram* ShaderManager::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
    
/*
    Получение строки поддерживаемых контекстом профилей шейдеров
*/

const char* ShaderManager::GetShaderProfilesString () const
{
  return "hlsl.vs hlsl.ps hlsl";
}
