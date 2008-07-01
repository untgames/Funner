#include "shared.h"

using namespace common;
using namespace xtl;

/*
   Созданеие/удаление ссылки
*/

void VarRegistrySystem::Link (const char* link_name, const char* source)
{
  try
  {
    MountPointsMap::GetGlobalMap ()->Link (link_name, source);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::VarRegistrySystem::Link");
    throw;
  }
}

void VarRegistrySystem::Unlink (const char* link_name)
{
  MountPointsMap::GetGlobalMap ()->Unlink (link_name);
}

/*
   Присоединение/отсоединение реестра
*/

void VarRegistrySystem::Mount (const char* branch_name, ICustomVarRegistry* registry)
{
  try
  {
    MountPointsMap::GetGlobalMap ()->Mount (branch_name, registry);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::VarRegistrySystem::Mount");
    throw;
  }
}

void VarRegistrySystem::Unmount (const char* branch_name, ICustomVarRegistry* registry)
{
  MountPointsMap::GetGlobalMap ()->Unmount (branch_name, registry);
}

void VarRegistrySystem::Unmount (const char* branch_name)
{
  MountPointsMap::GetGlobalMap ()->Unmount (branch_name);
}

void VarRegistrySystem::UnmountAll (ICustomVarRegistry* registry)
{
  MountPointsMap::GetGlobalMap ()->UnmountAll (registry);
}

void VarRegistrySystem::UnmountAll ()
{
  MountPointsMap::GetGlobalMap ()->UnmountAll ();
}
