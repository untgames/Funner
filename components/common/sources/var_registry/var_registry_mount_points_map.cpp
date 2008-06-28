#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

//отсечение суффикса
void remove_suffix (stl::string& s)
{
  size_t pos = s.rfind ('.');
  
  s.erase (pos != stl::string::npos ? pos : 0);
}

}

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
  static const char* METHOD_NAME = "common::MountPointsMap::Mount";

  if (!branch_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "branch_name");

  if (!registry)
    throw xtl::make_null_argument_exception (METHOD_NAME, "registry");
    
    //проверка конфликтов в именах точек монтирования

  for (stl::string parent_branch=branch_name; !parent_branch.empty (); )
  {
    MountMap::iterator iter = mount_points_map.find (parent_branch.c_str ());

    if (iter != mount_points_map.end ())
      throw format_operation_exception ("common::MountPointsMap::Mount",
        "Can't mount branch '%s', because it conflicts with already mounted branch '%s'",
        branch_name, parent_branch.c_str ());
        
      //отсечение суффикса

    remove_suffix (parent_branch);
  }
  
    //добавление новой точки монтирования

  MountPointPtr new_mount_point (new MountPoint (branch_name, registry), false);

  mount_points_map.insert_pair (branch_name, new_mount_point);
  
    //оповещение о появлении новой точки монтирования

  for (ListenersList::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
  {
    IListener& listener = **iter;
    
    try
    {
      listener.OnMount (new_mount_point.get ());
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
}

void MountPointsMap::OnUnmount (MountPoint* mount_point)
{
  for (ListenersList::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
  {
    IListener& listener = **iter;
    
    try
    {
      listener.OnUnmount (mount_point);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
}

void MountPointsMap::Unmount (const char* branch_name)
{
  if (!branch_name)
    return;

    //поиск точки монтирования с указанным именем

  MountMap::iterator iter = mount_points_map.find (branch_name);      

  if (iter == mount_points_map.end ())
    return;
    
  MountPoint* mount_point = iter->second.get ();
    
    //оповещение об удалении точки монтирования
  
  OnUnmount (mount_point);

   //удаление точки монтирования

  mount_points_map.erase (iter);
}

void MountPointsMap::UnmountAll ()
{
    //оповещение об удалении всех точек монтирования

  for (MountMap::iterator map_iter = mount_points_map.begin (), map_end = mount_points_map.end (); map_iter != map_end; ++map_iter)
    OnUnmount (map_iter->second.get ());
    
    //удаление всех точек монтирования

  mount_points_map.clear ();
}

/*
   Поиск точки монтирования  
     жадный поиск точки монтирования с последовательным отсечением суффиксов (пример для a.b.c.d)
       1) search: a.b.c.d var_sub_name = ''
       2) search: a.b.c   var_sub_name = 'd'
       3) search: a.b     var_sub_name = 'c.d'
       4) search: a       var_sub_name = 'b.c.d'
       5) return 0
*/

MountPoint* MountPointsMap::FindMountPoint (const char* full_var_name, stl::string& var_sub_name)
{
  for (stl::string mount_point_name = full_var_name; !mount_point_name.empty (); )
  {
    MountMap::iterator iter = mount_points_map.find (mount_point_name.c_str ());

    if (iter != mount_points_map.end ())
    {
      var_sub_name = full_var_name + mount_point_name.size () + 1;

      return iter->second.get ();
    }
    
      //отсечение суффикса
      
    remove_suffix (mount_point_name);
  }

  return 0;
}

MountPoint* MountPointsMap::FindMountPoint (const char* branch_name, const char* var_name, stl::string& var_sub_name)
{
  stl::string full_name (branch_name);
  
  full_name += '.';
  full_name += var_name;
  
  return FindMountPoint (full_name.c_str (), var_sub_name);
}

/*
   Подписка на события карты
*/

void MountPointsMap::RegisterListener (IListener* listener)
{
  if (!listener)
    return;

  listeners.push_back (listener);

  for (MountMap::iterator iter = mount_points_map.begin (), end = mount_points_map.end (); iter != end; ++iter)
    listener->OnMount (iter->second.get ());
}

void MountPointsMap::UnregisterListener (IListener* listener)
{
  listeners.remove (listener);
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
