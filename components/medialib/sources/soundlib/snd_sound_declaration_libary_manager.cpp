#include "shared.h"

using namespace media;

/*
    SoundDeclarationLibraryManagerImpl
*/

SoundDeclarationLibraryManagerImpl::SoundDeclarationLibraryManagerImpl ()
{
}

/*
    SoundDeclarationLibraryManager
*/

void SoundDeclarationLibraryManager::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  SoundDeclarationLibraryManagerSingleton::Instance ().RegisterLoader (extension, handler);
}

void SoundDeclarationLibraryManager::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  SoundDeclarationLibraryManagerSingleton::Instance ().RegisterSaver (extension, handler);
}

void SoundDeclarationLibraryManager::UnregisterLoader (const char* extension)
{
  SoundDeclarationLibraryManagerSingleton::Instance ().UnregisterLoader (extension);
}

void SoundDeclarationLibraryManager::UnregisterSaver (const char* extension)
{
  SoundDeclarationLibraryManagerSingleton::Instance ().UnregisterSaver (extension);
}

void SoundDeclarationLibraryManager::UnregisterAll ()
{
  SoundDeclarationLibraryManagerSingleton::Instance ().UnregisterAll ();
}
