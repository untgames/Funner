#include <common/exception.h>

#include "shared.h"

using namespace medialib::collada;
using namespace common;

/*
    Конструктор / деструктор
*/

ModelSystemImpl::ModelSystemImpl ()
{
  RegisterLoader ("dae", &Model::DefaultDaeLoader);
}

/*
    Регистрация пользовательских функций загрузки / сохранения
*/

void ModelSystem::RegisterLoader (const char* extension, const LoadHandler& loader)
{
  ModelSystemSingleton::Instance ().RegisterLoader (extension, loader);
}

void ModelSystem::RegisterSaver (const char* extension, const SaveHandler& saver)
{
  ModelSystemSingleton::Instance ().RegisterSaver (extension, saver);
}

void ModelSystem::UnregisterLoader (const char* extension)
{
  ModelSystemSingleton::Instance ().UnregisterLoader (extension);
}

void ModelSystem::UnregisterSaver (const char* extension)
{
  ModelSystemSingleton::Instance ().UnregisterSaver (extension);
}

void ModelSystem::UnregisterAll ()
{
  ModelSystemSingleton::Instance ().UnregisterAll ();
}
