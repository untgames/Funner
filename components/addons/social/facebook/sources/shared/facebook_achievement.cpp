#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   Достижения
*/

void FacebookSessionImpl::LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadAchievements", "Achievments not supported for facebook session");
}

/*
   Иконка
*/

void FacebookSessionImpl::LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadAchievementPicture", "Achievments not supported for facebook session");
}

/*
   Публикация
*/

void FacebookSessionImpl::SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::SendAchievement", "Achievments not supported for facebook session");
}
