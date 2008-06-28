#include "shared.h"

using namespace common;
using namespace xtl;

/*
   ѕрисоединение/отсоединение реестра
*/

void VarRegistrySystem::Mount (const char* branch_name, ICustomVarRegistry* registry)
{
  static const char* METHOD_NAME = "common::VarRegistrySystem::Mount";

  if (!branch_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "branch_name");

  if (!registry)
    throw xtl::make_null_argument_exception (METHOD_NAME, "registry");

  MountPointsMap::GetGlobalMap ()->Mount (branch_name, registry);
}

void VarRegistrySystem::Unmount (const char* branch_name)
{
  if (!branch_name)
    throw xtl::make_null_argument_exception ("common::VarRegistrySystem::Unmount", "branch_name");

  MountPointsMap::GetGlobalMap ()->Unmount (branch_name);
}

void VarRegistrySystem::UnmountAll ()
{
  MountPointsMap::GetGlobalMap ()->UnmountAll ();
}
