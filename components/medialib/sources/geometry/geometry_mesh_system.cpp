#include "shared.h"

using namespace medialib::geometry;

/*
    MeshSystemImpl
*/

MeshSystemImpl::MeshSystemImpl ()
{
}

/*
    Врапперы вызовов MeshSystemImpl    
*/

namespace medialib
{

namespace geometry
{

void MeshSystem::RegisterLoader (const char* extension, const LoadHandler& loader)
{
  MeshSystemSingleton::Instance ().RegisterLoader (extension, loader);
}

void MeshSystem::RegisterSaver (const char* extension, const SaveHandler& saver)
{
  MeshSystemSingleton::Instance ().RegisterSaver (extension, saver);
}

void MeshSystem::UnregisterLoader (const char* extension)
{
  MeshSystemSingleton::Instance ().UnregisterLoader (extension);
}

void MeshSystem::UnregisterSaver (const char* extension)
{
  MeshSystemSingleton::Instance ().UnregisterSaver (extension);
}

void MeshSystem::UnregisterAll ()
{
  MeshSystemSingleton::Instance ().UnregisterAll ();
}

}

}
