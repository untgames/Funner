#ifndef MEDIA_RMS_SERVER_HEADER
#define MEDIA_RMS_SERVER_HEADER

#include <media/rms/binding.h>
#include <media/rms/event_batcher.h>
#include <media/rms/group.h>

namespace media
{

namespace rms
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomServer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void PrefetchResource (const char* resource_name) = 0;
    virtual void LoadResource     (const char* resource_name) = 0;
    virtual void UnloadResource   (const char* resource_name) = 0;

  protected:
    virtual ~ICustomServer () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerGroup
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ServerGroup  (const char* name);
    ServerGroup  (const ServerGroup&);
    ~ServerGroup ();

    ServerGroup& operator = (const ServerGroup&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach (ICustomServer&);
    void Detach (ICustomServer&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Filters      () const;
    void        SetFilters   (const char* filters_string);    
    void        AddFilters   (const char* filters_string);
    void        ResetFilters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Binding CreateBinding (const Group&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCacheState (bool state);
    bool CacheState    () const;
    void EnableCache   () { SetCacheState (true); }
    void DisableCache  () { SetCacheState (false); }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushUnusedResources ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ServerGroup&);
    
  private:
    struct Impl;  
  
    ServerGroup (Impl*);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (ServerGroup&, ServerGroup&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ������� � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ServerGroupAttachment: private ICustomServer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ServerGroupAttachment  (const char* name, ICustomServer& server);
    ~ServerGroupAttachment ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ICustomServer&   Server      () const;
    rms::ServerGroup ServerGroup () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void SetNotificationsState (bool state);
    bool NotificationsState    () const;
    void EnableNotifications   () { SetNotificationsState (true); }
    void DisableNotifications  () { SetNotificationsState (false); }

  private:
    ServerGroupAttachment (const ServerGroupAttachment&); //no impl
    ServerGroupAttachment& operator = (const ServerGroupAttachment&); //no impl

    void PrefetchResource (const char* resource_name);
    void LoadResource     (const char* resource_name);
    void UnloadResource   (const char* resource_name);

  private:
    rms::ServerGroup group;
    ICustomServer*   server;
    EventBatcher     event_batcher;
    bool             notifications_state;
};

}

}

#endif
