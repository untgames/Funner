#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

/*
    Прокомментировать назначение классов!!!! (leny)
*/

struct EventHandlerRouter
{
  public:
    EventHandlerRouter (const char* in_var_mask, const VarRegistryImpl::EventHandler& in_handler) : var_mask (in_var_mask), handler (in_handler) {}

    void operator () (const char* var_name) const
    {
      if (!wcmatch (var_name, var_mask.c_str ()))
        return;

      try
      {
        handler (var_name);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

  private:
    VarRegistryImpl::EventHandler handler;
    stl::string                   var_mask;
};

struct NullVarRegistry: public VarRegistryImpl
{
  NullVarRegistry () : VarRegistryImpl (MountPointsMap::Pointer (new MountPointsMap), "") {}
};

} 

/*
   Конструкторы / деструкторы
*/

VarRegistryImpl::VarRegistryImpl (const MountPointsMap::Pointer& in_mount_points_map, const char* in_branch_name) 
  : branch_name (in_branch_name), mount_points_map (in_mount_points_map)
{
  mount_points_map->RegisterListener (this);
}

VarRegistryImpl::~VarRegistryImpl ()
{
  mount_points_map->UnregisterListener (this);
}

/*
   Получение/установка данных
*/

const char* VarRegistryImpl::GetValue (const char* var_name)
{
  static const char* METHOD_NAME = "common::VarRegistryImpl::GetValue";
  
  if (!var_name)
    throw make_null_argument_exception (METHOD_NAME, "var_name");

  stl::string full_var_name;
  MountPoint  *mount_point = FindMountPoint (var_name, full_var_name);

  if (!mount_point)
    throw make_argument_exception (METHOD_NAME, "var_name", var_name, "Registry not mounted");

  return mount_point->Registry ()->GetValue (full_var_name.c_str () + strlen (mount_point->Name ()) + 1);
}

void VarRegistryImpl::SetValue (const char* var_name, const char* value)
{
  static const char* METHOD_NAME = "common::VarRegistryImpl::SetValue";
  
  if (!var_name)
    throw make_null_argument_exception (METHOD_NAME, "var_name");

  if (!value)
    throw make_null_argument_exception (METHOD_NAME, "value");

  stl::string full_var_name;

  MountPoint *mount_point = FindMountPoint (var_name, full_var_name);

  if (!mount_point)
    throw make_argument_exception (METHOD_NAME, "var_name", var_name, "Registry not mounted");

  return mount_point->Registry ()->SetValue (full_var_name.c_str () + strlen (mount_point->Name ()) + 1, value);
}

/*
   Проверка наличия переменной
*/

bool VarRegistryImpl::HasVariable (const char* var_name)
{
  if (!var_name)
    return false;
    
  try
  {
    stl::string full_var_name;

    MountPoint *mount_point = FindMountPoint (var_name, full_var_name);

    if (!mount_point)
      return false;

    return mount_point->Registry ()->HasVariable (full_var_name.c_str () + strlen (mount_point->Name ()) + 1);
  }
  catch (...)
  {
    return false;
  }
}

/*
   Получение имени
*/

const char* VarRegistryImpl::BranchName ()
{
  return branch_name.c_str ();
}

/*
   Обход всех переменных / переменных по маске
*/

void VarRegistryImpl::EnumerateVars (const EnumHandler& handler)
{
  struct EnumerationWrapper
  {
    public:
      EnumerationWrapper (MountPointsList::iterator begin, MountPointsList::iterator in_last, const EnumHandler& in_handler, size_t in_branch_name_size)
        : iter (begin), last (in_last), handler (in_handler), branch_name_size (in_branch_name_size)
      {
        EventHandler wrapper = *this;

        for (; iter != last; ++iter)
        {
          if (*((*iter)->mount_point->Name () + branch_name_size))
            current_prefix = (*iter)->mount_point->Name () + branch_name_size + 1;
          else
            current_prefix = (*iter)->mount_point->Name () + branch_name_size;

          (*iter)->mount_point->Registry ()->EnumerateVars (wrapper);
        }
      
      }

      void operator () (const char* var_name)
      {
        handler (format ("%s%s", current_prefix.c_str (), var_name).c_str ());
      }

    private:
      MountPointsList::iterator iter, last;
      EnumHandler               handler;
      stl::string               current_prefix;
      size_t                    branch_name_size;
  };

  EnumerationWrapper (mount_points_list.begin (), mount_points_list.end (), handler, branch_name.length ());
}

void VarRegistryImpl::EnumerateVars (const char* mask, const EnumHandler& handler)
{
    //дублирование кода!!!!
  struct EnumerationWrapper
  {
    public:
      EnumerationWrapper (MountPointsList::iterator begin, MountPointsList::iterator in_last, const EnumHandler& in_handler, size_t in_branch_name_size, const char* in_var_name_mask)
        : iter (begin), last (in_last), handler (in_handler), branch_name_size (in_branch_name_size), var_name_mask (in_var_name_mask)
      {
        EventHandler wrapper = *this;

        for (; iter != last; ++iter)
        {
          if (*((*iter)->mount_point->Name () + branch_name_size))
            current_prefix = (*iter)->mount_point->Name () + branch_name_size + 1;
          else
            current_prefix = (*iter)->mount_point->Name () + branch_name_size;

          (*iter)->mount_point->Registry ()->EnumerateVars (wrapper);
        }
      
      }

      void operator () (const char* var_name)
      {
        stl::string relative_var_name = format ("%s%s", current_prefix.c_str (), var_name);

        if (wcmatch (relative_var_name.c_str (), var_name_mask))
          handler (relative_var_name.c_str ());
      }

    private:
      MountPointsList::iterator iter, last;
      EnumHandler               handler;
      stl::string               current_prefix;
      const char*               var_name_mask;
      size_t                    branch_name_size;
  };

  EnumerationWrapper (mount_points_list.begin (), mount_points_list.end (), handler, branch_name.length (), mask);
}

/*
   Подписка на добавление/изменение/удаление переменных
*/

xtl::connection VarRegistryImpl::RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler)
{
  static const char* METHOD_NAME = "common::VarRegistryImpl::RegisterEventHandler";
  
  if (!var_name_mask)
    throw make_null_argument_exception (METHOD_NAME, "var_name_mask");

  switch (event)
  {
    case VarRegistryEvent_OnCreateVar:
    case VarRegistryEvent_OnDeleteVar:
    case VarRegistryEvent_OnChangeVar:
      break;
    default:
      throw make_argument_exception (METHOD_NAME, "event", event);
  }

  return signals [event].connect (EventHandlerRouter (var_name_mask, handler));
}

/*
   Оповещение о появлении/удалении точек монтирования
*/

void VarRegistryImpl::OnMount (MountPoint* new_mount_point)
{
  if (strstr (new_mount_point->Name (), branch_name.c_str ()) == new_mount_point->Name ())
  {
    mount_points_list.push_back (AssignedMountPointPtr (new AssignedMountPoint (new_mount_point, new_mount_point->RegisterEventHandler (xtl::bind (&VarRegistryImpl::Notify, this, _1, _2, _3)))));
  }
}

///???????????
class VarRegistryImpl::RemoveIfPredicate
{
  public:
    RemoveIfPredicate (MountPoint* in_mount_point) : mount_point (in_mount_point) {}

    bool operator () (const VarRegistryImpl::AssignedMountPointPtr& val)
    {
      return val->mount_point == mount_point;
    }

  private:
    MountPoint* mount_point;
};

void VarRegistryImpl::OnUnmount (MountPoint* deleted_mount_point)
{
  mount_points_list.remove_if (RemoveIfPredicate (deleted_mount_point));
}

/*
   Оповещение
*/

void VarRegistryImpl::Notify (const char* mount_point_name, const char* var_name, VarRegistryEvent event)
{
  try
  {
    stl::string full_var_name = format ("%s.%s", mount_point_name, var_name);

    signals[event] (full_var_name.c_str () + branch_name.length () + 1);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
   Нахождение точки монтирования по имени переменной
*/

MountPoint* VarRegistryImpl::FindMountPoint (const char* var_name, stl::string& full_var_name)
{
  full_var_name = format ("%s.%s", branch_name.c_str (), var_name); 

  return mount_points_map->FindMountPoint (full_var_name.c_str ());
}

/*
   Создание ветвей реестра
*/

VarRegistryImpl* VarRegistryImpl::GetNullBranch ()
{
  return common::Singleton<NullVarRegistry>::InstancePtr ();
}

VarRegistryImpl* VarRegistryImpl::CreateBranch (const char* branch_name)
{
  return new VarRegistryImpl (MountPointsMap::GetGlobalMap (), branch_name);
}
