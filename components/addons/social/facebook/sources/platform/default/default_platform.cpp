#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   Платформа по умолчанию
*/

/*
   Логин (возвращает true, если началось выполнение специфичного для данной платформы логина)
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
   Проверка, установлено ли приложение facebook
*/

bool DefaultPlatform::IsFacebookAppInstalled ()
{
  return false;
}

/*
   Публикация события установки приложения
*/

void DefaultPlatform::PublishAppInstallEvent (const char* app_id)
{
}
