#include <stl/hash_map>
#include <stl/list>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>
#include <xtl/string.h>
#include <xtl/trackable.h>

#include <common/file.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/var_registry.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MountPoint : public xtl::trackable, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MountPoint (const char* in_name, ICustomVarRegistry* in_registry);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const stl::string& Name () const { return name; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ICustomVarRegistry* Registry () const { return registry.get (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* mount_point_name, const char* var_name, VarRegistryEvent event)> EventHandler;    

    xtl::connection RegisterEventHandler (const EventHandler& handler);

  private:
    void Notify (const char* var_name, VarRegistryEvent event);

  private:
    typedef xtl::com_ptr<ICustomVarRegistry>                               CustomVarRegistryPtr;
    typedef xtl::signal<void (const char*, const char*, VarRegistryEvent)> MountPointSignal;

  private:
    stl::string          name;
    CustomVarRegistryPtr registry;
    MountPointSignal     signal;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MountPointsMap
{
  public:
    typedef xtl::shared_ptr<MountPointsMap> Pointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ~MountPointsMap ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Link   (const char* link_name, const char* source);
    void Unlink (const char* link_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Mount      (const char* branch_name, ICustomVarRegistry* registry);
    void Unmount    (const char* branch_name, ICustomVarRegistry* registry);
    void Unmount    (const char* branch_name);
    void UnmountAll (ICustomVarRegistry* registry);
    void UnmountAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MountPoint* FindMountPoint (const char* full_var_name, stl::string& var_sub_name);
    MountPoint* FindMountPoint (const char* branch_name, const char* var_name, stl::string& var_sub_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    class IListener
    {
      public:
        virtual void OnMount   (MountPoint*) = 0;
        virtual void OnUnmount (MountPoint*) = 0;
    };

    void RegisterListener   (IListener*);
    void UnregisterListener (IListener*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer GetGlobalMap ();
    
  private:
    void OnUnmount (MountPoint*);

  private:
    typedef xtl::intrusive_ptr<MountPoint>                           MountPointPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, MountPointPtr> MountMap;
    typedef stl::list<IListener*>                                    ListenersList;

  private:
    MountMap      mount_points_map;
    ListenersList listeners;
};

}
