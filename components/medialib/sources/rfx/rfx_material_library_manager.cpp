#include "shared.h"

using namespace media::rfx;

/*
    MaterialLibraryManagerImpl
*/

MaterialLibraryManagerImpl::MaterialLibraryManagerImpl ()
{
}

/*
    MaterialLibraryManager
*/

void MaterialLibraryManager::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  MaterialLibraryManagerSingleton::Instance ().RegisterLoader (extension, handler);
}

void MaterialLibraryManager::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  MaterialLibraryManagerSingleton::Instance ().RegisterSaver (extension, handler);
}

void MaterialLibraryManager::UnregisterLoader (const char* extension)
{
  MaterialLibraryManagerSingleton::Instance ().UnregisterLoader (extension);
}

void MaterialLibraryManager::UnregisterSaver (const char* extension)
{
  MaterialLibraryManagerSingleton::Instance ().UnregisterSaver (extension);
}

void MaterialLibraryManager::UnregisterAll ()
{
  MaterialLibraryManagerSingleton::Instance ().UnregisterAll ();
}
