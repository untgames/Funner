#include "shared.h"

using namespace render::manager;

/*
    Описание реализации менеджера шейдинга
*/

struct ProgramManager::Impl
{
  ProgramProxyManager program_proxy_manager; //менеджер прокси программ
};

/*
    Конструктор / деструктор
*/

ProgramManager::ProgramManager ()
  : impl (new Impl)
{
}

ProgramManager::~ProgramManager ()
{
}

/*
    Получение прокси
*/

ProgramProxy ProgramManager::GetProgramProxy (const char* name)
{
  return impl->program_proxy_manager.GetProxy (name);
}

/*
    Поиск загруженной программы
*/

ProgramPtr ProgramManager::FindProgram (const char* name)
{
  return impl->program_proxy_manager.FindResource (name);
}

/*
    Установка программы по умолчанию
*/

void ProgramManager::SetDefaultProgram (const ProgramPtr& program)
{
  impl->program_proxy_manager.SetDefaultResource (program);
}

ProgramPtr ProgramManager::DefaultProgram ()
{
  return impl->program_proxy_manager.DefaultResource ();
}
