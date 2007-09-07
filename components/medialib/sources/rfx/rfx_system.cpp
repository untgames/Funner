#include "shared.h"

using namespace media::rfx;

/*
    MaterialSystemImpl
*/

MaterialSystemImpl::MaterialSystemImpl ()
{
}

/*
    MaterialSystem
*/

void MaterialSystem::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  MaterialSystemSingleton::Instance ().RegisterLoader (extension, handler);
}

void MaterialSystem::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  MaterialSystemSingleton::Instance ().RegisterSaver (extension, handler);
}

void MaterialSystem::UnregisterLoader (const char* extension)
{
  MaterialSystemSingleton::Instance ().UnregisterLoader (extension);
}

void MaterialSystem::UnregisterSaver (const char* extension)
{
  MaterialSystemSingleton::Instance ().UnregisterSaver (extension);
}

void MaterialSystem::UnregisterAll ()
{
  MaterialSystemSingleton::Instance ().UnregisterAll ();
}
