#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Конструктор / деструктор
*/

ModelManagerImpl::ModelManagerImpl ()
{
  RegisterLoader ("dae", &Model::DefaultDaeLoader);
}

/*
    Регистрация пользовательских функций загрузки / сохранения
*/

void ModelManager::RegisterLoader (const char* extension, const LoadHandler& loader)
{
  ModelManagerSingleton::Instance ().RegisterLoader (extension, loader);
}

void ModelManager::RegisterSaver (const char* extension, const SaveHandler& saver)
{
  ModelManagerSingleton::Instance ().RegisterSaver (extension, saver);
}

void ModelManager::UnregisterLoader (const char* extension)
{
  ModelManagerSingleton::Instance ().UnregisterLoader (extension);
}

void ModelManager::UnregisterSaver (const char* extension)
{
  ModelManagerSingleton::Instance ().UnregisterSaver (extension);
}

void ModelManager::UnregisterAll ()
{
  ModelManagerSingleton::Instance ().UnregisterAll ();
}
