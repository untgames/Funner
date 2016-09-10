#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   Default platform
*/

/*
   Login (returns true, if platform-specific login flow was started)
*/

void DefaultPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  callback (false, OperationStatus_Failure, "", "");
}

void DefaultPlatform::CancelLogin ()
{
}

void DefaultPlatform::LogOut ()
{
}

/*
   Check if Facebook app is installed on device
*/

bool DefaultPlatform::IsFacebookAppInstalled ()
{
  return false;
}

/*
   Send app install event to Facebook
*/

void DefaultPlatform::PublishAppInstallEvent (const char* app_id)
{
}

/*
   Handle component start, used for initializing facebook SDK before user logins
*/

void DefaultPlatform::OnComponentStart ()
{
}
