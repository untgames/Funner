#include "shared.h"

using namespace push_notifications;

/*
   Push notifications centers manager implementation
*/

namespace
{

const char* COMPONENTS_MASK = "push_notifications.centers.*"; //push notifications centers components names wildcard

struct PushNotificationsCenterDesc
{
  stl::string                                          center_name_mask;
  PushNotificationsCentersManager::CreateCenterHandler handler;

  PushNotificationsCenterDesc (const char* in_mask, const PushNotificationsCentersManager::CreateCenterHandler& in_handler)
    : center_name_mask (in_mask)
    , handler (in_handler)
    {}
};

}

struct PushNotificationsCentersManagerImpl::Impl
{
  typedef stl::hash_map<stl::hash_key<const char*>, PushNotificationsCenterDesc> PushNotificationsCenterDescMap;

  PushNotificationsCenterDescMap center_creators;

  ///Registration of push notifications centers creators
  void RegisterCenter (const char* id, const char* center_name_mask, const PushNotificationsCentersManager::CreateCenterHandler& handler)
  {
    static const char* METHOD_NAME = "push_notifications::PushNotificationsCentersManager::RegisterCenter";

    if (!id)
      throw xtl::make_null_argument_exception (METHOD_NAME, "id");

    if (!center_name_mask)
      throw xtl::make_null_argument_exception (METHOD_NAME, "center_name_mask");

    PushNotificationsCenterDescMap::iterator iter = center_creators.find (id);

    if (iter != center_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Store with id '%s' already registered", id);

    center_creators.insert_pair (id, PushNotificationsCenterDesc (center_name_mask, handler));
  }

  void UnregisterCenter (const char* id)
  {
    if (!id)
      throw xtl::make_null_argument_exception ("push_notifications::PushNotificationsCentersManager::UnregisterStore", "id");

    center_creators.erase (id);
  }

  void UnregisterAllCenters ()
  {
    center_creators.clear ();
  }

  ///Search for push notifications center
  PushNotificationsCenterDescMap::iterator FindCenterDesc (const char* center_name)
  {
    for (PushNotificationsCenterDescMap::iterator iter = center_creators.begin (), end = center_creators.end (); iter != end; ++iter)
    {
      if (common::wcmatch (center_name, iter->second.center_name_mask.c_str ()))
        return iter;
    }

    return center_creators.end ();
  }

  ///Check push notifications center existence
  bool IsCenterRegistered (const char* center_name)
  {
    if (!center_name)
      throw xtl::make_null_argument_exception ("push_notifications::PushNotificationsCentersManager::IsStoreRegistered", "center_name");

    LoadDefaultCenters ();

    return FindCenterDesc (center_name) != center_creators.end ();
  }

  ///Create push notifications center
  IPushNotificationsCenter* CreateCenter (const char* center_name)
  {
    static const char* METHOD_NAME = "push_notifications::PushNotificationsCentersManagerImpl::CreateCenter";

    if (!center_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "center_name");

    LoadDefaultCenters ();

    PushNotificationsCenterDescMap::iterator iter = FindCenterDesc (center_name);

    if (iter == center_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Push notifications center '%s' not registered", center_name);

    return iter->second.handler (center_name);
  }

  ///Load default push notifications centers
  void LoadDefaultCenters ()
  {
    static common::ComponentLoader loader (COMPONENTS_MASK);
  }
};

/*
   Constructor / destructor
*/

PushNotificationsCentersManagerImpl::PushNotificationsCentersManagerImpl ()
  : impl (new Impl)
  {}

PushNotificationsCentersManagerImpl::~PushNotificationsCentersManagerImpl ()
{
  delete impl;
}

/*
   Registration of push notifications centers creators
*/

void PushNotificationsCentersManagerImpl::RegisterCenter (const char* id, const char* center_name_mask, const PushNotificationsCentersManager::CreateCenterHandler& handler)
{
  impl->RegisterCenter (id, center_name_mask, handler);
}

void PushNotificationsCentersManagerImpl::UnregisterCenter (const char* id)
{
  impl->UnregisterCenter (id);
}

void PushNotificationsCentersManagerImpl::UnregisterAllCenters ()
{
  impl->UnregisterAllCenters ();
}

/*
   Check push notifications center existence
*/

bool PushNotificationsCentersManagerImpl::IsCenterRegistered (const char* center_name)
{
  return impl->IsCenterRegistered (center_name);
}

/*
   Create push notifications center
*/

IPushNotificationsCenter* PushNotificationsCentersManagerImpl::CreateCenter (const char* center_name)
{
  return impl->CreateCenter (center_name);
}

/*
   Registration of push notifications centers creators
*/

void PushNotificationsCentersManager::RegisterCenter (const char* id, const char* center_name_mask, const PushNotificationsCentersManager::CreateCenterHandler& handler)
{
  PushNotificationsCentersManagerSingleton::Instance ()->RegisterCenter (id, center_name_mask, handler);
}

void PushNotificationsCentersManager::UnregisterCenter (const char* id)
{
  PushNotificationsCentersManagerSingleton::Instance ()->UnregisterCenter (id);
}

void PushNotificationsCentersManager::UnregisterAllCenters ()
{
  PushNotificationsCentersManagerSingleton::Instance ()->UnregisterAllCenters ();
}

/*
   Check push notifications center existence
*/
    
bool PushNotificationsCentersManager::IsCenterRegistered (const char* center_name)
{
  return PushNotificationsCentersManagerSingleton::Instance ()->IsCenterRegistered (center_name);
}
