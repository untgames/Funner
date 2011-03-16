#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера шейдинга
*/

struct ShadingManager::Impl
{
  ProgramProxyManager program_proxy_manager; //менеджер прокси программ
};

/*
    Конструктор / деструктор
*/

ShadingManager::ShadingManager ()
  : impl (new Impl)
{
}

ShadingManager::~ShadingManager ()
{
}

/*
    Получение прокси
*/

ProgramProxy ShadingManager::GetProgramProxy (const char* name)
{
  return impl->program_proxy_manager.GetProxy (name);
}

/*
    Поиск загруженной программы
*/

LowLevelProgramPtr ShadingManager::FindProgram (const char* name)
{
  return impl->program_proxy_manager.FindResource (name);
}

/*
    Установка программы по умолчанию
*/

void ShadingManager::SetDefaultProgram (const LowLevelProgramPtr& program)
{
  impl->program_proxy_manager.SetDefaultResource (program);
}

LowLevelProgramPtr ShadingManager::DefaultProgram ()
{
  return impl->program_proxy_manager.DefaultResource ();
}
