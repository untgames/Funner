#include "shared.h"

using namespace scene_graph;

/*
    Регистрация сериализаторов
*/

void SceneSerializationManager::RegisterLoader (const char* file_type, const SceneLoader& loader)
{
  SceneSerializationManagerImpl::RegisterLoader (file_type, loader);
}

void SceneSerializationManager::RegisterSaver (const char* file_type, const SceneSaver& saver)
{
  SceneSerializationManagerImpl::RegisterSaver (file_type, saver);
}

void SceneSerializationManager::UnregisterLoader (const char* file_type)
{
  SceneSerializationManagerImpl::UnregisterLoader (file_type);
}

void SceneSerializationManager::UnregisterSaver (const char* file_type)
{
  SceneSerializationManagerImpl::UnregisterSaver (file_type);
}

void SceneSerializationManager::UnregisterAllLoaders ()
{
  SceneSerializationManagerImpl::UnregisterAllLoaders ();
}

void SceneSerializationManager::UnregisterAllSavers ()
{
  SceneSerializationManagerImpl::UnregisterAllSavers ();
}
