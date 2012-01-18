#include "shared.h"

using namespace social;
using namespace social::game_kit;

@implementation AchievementHandle

@synthesize achievement, description;

-(void)dealloc
{
  self.achievement = nil;
  self.description = nil;

  [super dealloc];
}

@end

namespace
{

void on_achievements_loaded (const char* source, NSArray *ns_achievements, NSArray *ns_descriptions, NSError *achievements_error, NSError *descriptions_error, const common::Log& log, const LoadAchievementsCallback& callback)
{
  try
  {
    if (achievements_error)
    {
      const char* error_string = [[achievements_error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (AchievementList (), OperationStatus_Failure, error_string);
      return;
    }

    if (descriptions_error)
    {
      const char* error_string = [[descriptions_error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (AchievementList (), OperationStatus_Failure, error_string);
      return;
    }

    AchievementList achievements;

    achievements.Reserve ([ns_achievements count]);

    for (GKAchievement* ns_achievement in ns_achievements)
    {
      GKAchievementDescription* ns_description = nil;

      for (GKAchievementDescription* current_description in ns_descriptions)
      {
        if ([current_description.identifier isEqualToString:ns_achievement.identifier])
        {
          ns_description = current_description;
          break;
        }
      }

      if (!ns_description)
      {
        const char* achievement_id = [ns_achievement.identifier UTF8String];

        log.Printf ("%s error 'can't load description for achievement '%s''", source, achievement_id);
        callback (AchievementList (), OperationStatus_Failure, "can't load descriptions for all achievements");
        return;
      }

      Achievement achievement;

      Utility::Instance ()->FillAchievement (ns_achievement, ns_description, achievement);

      achievements.Add (achievement);
    }

    callback (achievements, OperationStatus_Success, OK_STATUS);
  }
  catch (xtl::exception& e)
  {
    log.Printf ("Exception in %s callback: '%s'", source, e.what ());
  }
  catch (...)
  {
    log.Printf ("Unknown exception in %s callback", source);
  }
}

void on_achievement_image_loaded (const char* source, UIImage* picture, NSError *error, const common::Log& log, const LoadUserPictureCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (media::Image (), OperationStatus_Failure, error_string);
      return;
    }

    if (!picture)
    {
      static const char* ERROR = "requested picture not loaded";

      log.Printf ("%s error '%s'", source, ERROR);
      callback (media::Image (), OperationStatus_Failure, ERROR);
      return;
    }

    callback (Utility::Instance ()->ConvertImage (picture), OperationStatus_Success, OK_STATUS);
  }
  catch (xtl::exception& e)
  {
    log.Printf ("Exception in %s callback: '%s'", source, e.what ());
  }
  catch (...)
  {
    log.Printf ("Unknown exception in %s callback", source);
  }
}

}

/*
   Достижения
*/

void GameKitSessionImpl::LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadAchievements";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *achievements_error)
  {
    [GKAchievementDescription loadAchievementDescriptionsWithCompletionHandler:^(NSArray *descriptions, NSError *descriptions_error)
    {
      on_achievements_loaded (METHOD_NAME, achievements, descriptions, achievements_error, descriptions_error, log, callback);
    }];
  }];
}

/*
   Иконка
*/

void GameKitSessionImpl::LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadAchievementPicture";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!achievement.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load picture for achievement without setted handle");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  AchievementHandle* handle = (AchievementHandle*)achievement.Handle ();

  if (handle.description.image)
    on_achievement_image_loaded (METHOD_NAME, handle.description.image, nil, log, callback);
  else
    [handle.description loadImageWithCompletionHandler:^(UIImage *image, NSError *error)
    {
      on_achievement_image_loaded (METHOD_NAME, image, error, log, callback);
    }];
}

/*
   Публикация
*/

void GameKitSessionImpl::SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties)
{

}
