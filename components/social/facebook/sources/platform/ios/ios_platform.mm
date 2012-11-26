#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   iOs платформа
*/

/*
   Логин (возвращает true, если началось выполнение специфичного для данной платформы логина)
*/

void IOsPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
}

void IOsPlatform::CancelLogin ()
{
}
