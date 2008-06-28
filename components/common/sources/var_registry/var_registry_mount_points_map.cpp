#include "shared.h"

using namespace common;
using namespace xtl;

/*
   Деструктор
*/

MountPointsMap::~MountPointsMap ()
{
  UnmountAll ();
}

/*
   Монтирование реестров
*/

void MountPointsMap::Mount (const char* branch_name, ICustomVarRegistry* registry)
{
  stl::string parent_branch (branch_name);
  
  while (!parent_branch.empty ())
  {
    MountMap::iterator iter = mount_points_map.find (parent_branch.c_str ());

    if (iter != mount_points_map.end ())
      throw format_operation_exception ("common::MountPointsMap::Mount", "Branch '%s' has already mounted");

    parent_branch.resize (get_prefix_end (parent_branch.c_str ()) - parent_branch.c_str ());
  }

  MountPointPtr new_mount_point (new MountPoint (branch_name, registry), false);

  mount_points_map.insert_pair (branch_name, new_mount_point);

  for (ListenersList::iterator iter = listeners_list.begin (), end = listeners_list.end (); iter != end; ++iter)
    (*iter)->OnMount (new_mount_point.get ());
}

void MountPointsMap::Unmount (const char* branch_name)
{
  MountMap::iterator mount_point_iter = mount_points_map.find (branch_name);

  if (mount_point_iter == mount_points_map.end ())
    return;

  for (ListenersList::iterator iter = listeners_list.begin (), end = listeners_list.end (); iter != end; ++iter)
    (*iter)->OnUnmount (mount_point_iter->second.get ());

  mount_points_map.erase (mount_point_iter);
}

void MountPointsMap::UnmountAll ()
{
  for (MountMap::iterator map_iter = mount_points_map.begin (), map_end = mount_points_map.end (); map_iter != map_end; ++map_iter)
    for (ListenersList::iterator list_iter = listeners_list.begin (), list_end = listeners_list.end (); list_iter != list_end; ++list_iter)
      (*list_iter)->OnUnmount (map_iter->second.get ());

  mount_points_map.clear ();
}

/*
   Поиск точки монтирования
*/

MountPoint* MountPointsMap::FindMountPoint (const char* full_var_name)
{
  MountMap::iterator ret_value;

  stl::string search_mount_point (full_var_name, get_prefix_end (full_var_name));

  while (!search_mount_point.empty ())
  {
    ret_value = mount_points_map.find (search_mount_point.c_str ());

    if (ret_value != mount_points_map.end ())
      return ret_value->second.get ();

    search_mount_point.resize (get_prefix_end (search_mount_point.c_str ()) - search_mount_point.c_str ());
  }

  return 0;
}

/*
   Подписка на события карты
*/

void MountPointsMap::RegisterListener (IListener* listener)
{
  listeners_list.push_back (listener);

  for (MountMap::iterator iter = mount_points_map.begin (), end = mount_points_map.end (); iter != end; ++iter)
    listener->OnMount (iter->second.get ());
}

void MountPointsMap::UnregisterListener (IListener* listener)
{
  listeners_list.remove (listener);
}

/*
   Получение глобальной карты точек монтирования
*/

MountPointsMap::Pointer MountPointsMap::GetGlobalMap ()
{
  struct MountPointsMapWrapper
  {
    MountPointsMapWrapper () : instance (new MountPointsMap) {}

    MountPointsMap::Pointer instance;
  };

  typedef Singleton<MountPointsMapWrapper> MountPointsMapSingleton;

  return MountPointsMapSingleton::Instance ().instance;
}
