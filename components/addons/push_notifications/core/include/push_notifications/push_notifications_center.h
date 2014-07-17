#ifndef PUSH_NOTIFICATIONS_CENTER_HEADER
#define PUSH_NOTIFICATIONS_CENTER_HEADER

namespace common
{

//forward declaration
class PropertyMap;

}

namespace push_notifications
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PushNotificationsCenter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PushNotificationsCenter  (const char* center_name);
    PushNotificationsCenter  (const PushNotificationsCenter& source);
    ~PushNotificationsCenter ();

    PushNotificationsCenter& operator = (const PushNotificationsCenter& source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (bool success, const char* status, const char* token)> RegisterCallback;

    void RegisterForNotifications   (const RegisterCallback& callback, const common::PropertyMap& properties = common::PropertyMap ());
    void UnregisterForNotifications ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char*)> NotificationsHandler;
    
    xtl::connection RegisterNotificationsHandler (const NotificationsHandler&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PushNotificationsCenter&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PushNotificationsCenter&, PushNotificationsCenter&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPushNotificationsCenter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IPushNotificationsCenter () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Description () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void RegisterForNotifications   (const PushNotificationsCenter::RegisterCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void UnregisterForNotifications () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::connection RegisterNotificationsHandler (const PushNotificationsCenter::NotificationsHandler&) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PushNotificationsCentersManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ������� ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<IPushNotificationsCenter* (const char* center_name)> CreateCenterHandler;

    static void RegisterCenter       (const char* id, const char* center_name_mask, const CreateCenterHandler& handler);
    static void UnregisterCenter     (const char* id);
    static void UnregisterAllCenters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������ ���-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsCenterRegistered (const char* center_name);
};

}

#endif