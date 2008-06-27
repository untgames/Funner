#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

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
   Реализация VarRegistry
*/

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
  stl::string full_var_name;
  MountPoint  *mount_point = FindMountPoint (var_name, full_var_name);

  if (!mount_point)
    throw make_argument_exception ("common::VarRegistry::GetValue", "var_name", var_name);

  return mount_point->Registry ()->GetValue (full_var_name.c_str () + strlen (mount_point->Name ()) + 1);
}

void VarRegistryImpl::SetValue (const char* var_name, const char* value)
{
  stl::string full_var_name;
  MountPoint *mount_point = FindMountPoint (var_name, full_var_name);

  if (!mount_point)
    throw make_argument_exception ("common::VarRegistry::SetValue", "var_name", var_name);

  return mount_point->Registry ()->SetValue (full_var_name.c_str () + strlen (mount_point->Name ()) + 1, value);
}

/*
   Проверка наличия переменной
*/

bool VarRegistryImpl::HasVariable (const char* var_name)
{
  stl::string full_var_name;
  MountPoint *mount_point = FindMountPoint (var_name, full_var_name);

  if (!mount_point)
    throw make_argument_exception ("common::VarRegistry::HasVariable", "var_name", var_name);

  return mount_point->Registry ()->HasVariable (full_var_name.c_str () + strlen (mount_point->Name ()) + 1);
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
  return signals[event].connect (EventHandlerRouter (var_name_mask, handler));
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
  stl::string full_var_name = format ("%s.%s", mount_point_name, var_name);

  signals[event] (full_var_name.c_str () + branch_name.length () + 1);
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
   Обёртка над реестром переменных
*/

/*
   Конструкторы / деструкторы
*/

VarRegistry::VarRegistry ()
  : impl (VarRegistryImpl::GetNullBranch ())
  {}

VarRegistry::VarRegistry (const char* branch_name)
{
  if (!branch_name)
    throw make_null_argument_exception ("common::VarRegistry::VarRegistry", "branch_name");

  impl = VarRegistryImpl::CreateBranch (branch_name);
}

VarRegistry::VarRegistry (const VarRegistry& source)
  : impl (source.impl)
{
}

VarRegistry::~VarRegistry ()
{
  release (impl);
}
    
/*
   Копирование
*/

VarRegistry& VarRegistry::operator = (const VarRegistry& source)
{
  if (this == &source)
    return *this;

  impl = source.impl;

  return *this;
}

/*
   Получение/установка данных
*/

const char* VarRegistry::GetValue (const char* var_name) const
{
  if (!var_name)
    throw make_null_argument_exception ("common::VarRegistry::GetValue", "var_name");

  return impl->GetValue (var_name);
}

void VarRegistry::SetValue (const char* var_name, const char* value)
{
  static const char* METHOD_NAME = "common::VarRegistry::SetValue";

  if (!var_name)
    throw make_null_argument_exception (METHOD_NAME, "var_name");

  if (!value)
    throw make_null_argument_exception (METHOD_NAME, "value");

  impl->SetValue (var_name, value);
}

/*
   Проверка наличия переменной
*/

bool VarRegistry::HasVariable (const char* var_name) const
{
  if (!var_name)
    throw make_null_argument_exception ("common::VarRegistry::HasVariable", "var_name");

  return impl->HasVariable (var_name);
}

/*
   Получение имени
*/

const char* VarRegistry::BranchName () const
{
  return impl->BranchName ();
}           

/*
   Проверка установлен ли реестр
*/

bool VarRegistry::IsOpened () const
{
  return impl != VarRegistryImpl::GetNullBranch ();
}

/*
   Подключение/отключение реестра
*/

void VarRegistry::Open (const char* branch_name)
{
  VarRegistry (branch_name).Swap (*this);
}

void VarRegistry::Close ()
{
  VarRegistry ().Swap (*this);
}

/*
   Обход всех переменных / переменных по маске
*/

void VarRegistry::EnumerateVars (const EnumHandler& handler)
{
  impl->EnumerateVars (handler);
}

void VarRegistry::EnumerateVars (const char* var_name_mask, const EnumHandler& handler)
{
  if (!var_name_mask)
    throw make_null_argument_exception ("common::VarRegistry::EnumerateVars", "var_name_mask");

  impl->EnumerateVars (var_name_mask, handler);
}

/*
   Подписка на добавление/изменение/удаление переменных
*/

xtl::connection VarRegistry::RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler) const
{
  if (!var_name_mask)
    throw make_null_argument_exception ("common::VarRegistry::RegisterEventHandler", "var_name_mask");

  return impl->RegisterEventHandler (var_name_mask, event, handler);
}

/*
   Обмен
*/

void VarRegistry::Swap (VarRegistry& source)
{
  stl::swap (impl, source.impl);
}

namespace common
{

/*
   Обмен
*/

void swap (VarRegistry& source1, VarRegistry& source2)
{
  source1.Swap (source2);
}

}
