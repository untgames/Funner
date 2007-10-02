#include "shared.h"

using namespace media::geometry;

/*
    MeshLibraryManagerImpl
*/

MeshLibraryManagerImpl::MeshLibraryManagerImpl ()
{
  RegisterSaver  ("xmesh", &xmesh_save_library);
  RegisterLoader ("xmesh", &xmesh_load_library);
}

/*
    Врапперы вызовов MeshLibraryManagerImpl    
*/

namespace media
{

namespace geometry
{

void MeshLibraryManager::RegisterLoader (const char* extension, const LoadHandler& loader)
{
  MeshLibraryManagerSingleton::Instance ().RegisterLoader (extension, loader);
}

void MeshLibraryManager::RegisterSaver (const char* extension, const SaveHandler& saver)
{
  MeshLibraryManagerSingleton::Instance ().RegisterSaver (extension, saver);
}

void MeshLibraryManager::UnregisterLoader (const char* extension)
{
  MeshLibraryManagerSingleton::Instance ().UnregisterLoader (extension);
}

void MeshLibraryManager::UnregisterSaver (const char* extension)
{
  MeshLibraryManagerSingleton::Instance ().UnregisterSaver (extension);
}

void MeshLibraryManager::UnregisterAll ()
{
  MeshLibraryManagerSingleton::Instance ().UnregisterAll ();
}

}

}
