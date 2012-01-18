#ifndef SOCIAL_GAME_KIT_SHARED_HEADER
#define SOCIAL_GAME_KIT_SHARED_HEADER

#import <UIKit/UIKit.h>

#import <GameKit/GKAchievement.h>
#import <GameKit/GKAchievementDescription.h>
#import <GameKit/GKAchievementViewController.h>
#import <GameKit/GKLeaderboardViewController.h>
#import <GameKit/GKLocalPlayer.h>
#import <GameKit/GKScore.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/image.h>

#include <social/session.h>

namespace social
{

namespace game_kit
{

const char* OK_STATUS = "Ok";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Game Kit ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class GameKitSessionImpl: public IAchievementManager, public ILeaderboardManager, public IUserManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    GameKitSessionImpl  ();
    ~GameKitSessionImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� Game Kit API
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsApiAvailable ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� � PropertyMap ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void CheckUnknownProperties (const char* source, const common::PropertyMap& properties,
                                        size_t known_properties_count, const char** known_properties_names);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowWindow (const char* window_name, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsUserLoggedIn ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    User& CurrentUser ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadLeaderboards (const LoadLeaderboardsCallback& callback, const common::PropertyMap& properties);
    void LoadLeaderboard  (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties);
    void LoadLeaderboard  (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties);
    void LoadFriends    (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties);

  private:
    GameKitSessionImpl (const GameKitSessionImpl& source);              //no impl
    GameKitSessionImpl& operator = (const GameKitSessionImpl& source);  //no impl

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         FillUser        (GKPlayer* player, User& user);
    void         FillLeaderboard (GKLeaderboard* ns_leaderboard, Leaderboard& leaderboard);
    void         FillScore       (GKScore* ns_score, Score& score);
    media::Image ConvertImage    (UIImage* image);

  private:
    common::Log log;
    User        current_user;
    bool        system_version_5_0_available;
};

class UtilityImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    UtilityImpl ();
    ~UtilityImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         FillUser        (GKPlayer* player, User& user);
    void         FillLeaderboard (GKLeaderboard* ns_leaderboard, Leaderboard& leaderboard, bool loaded_for_user);
    void         FillScore       (GKScore* ns_score, Score& score);
    void         FillAchievement (GKAchievement* ns_achievement, GKAchievementDescription* ns_description, Achievement& achievement);
    media::Image ConvertImage    (UIImage* image);

  private:
    UtilityImpl (const UtilityImpl&);             //no impl
    UtilityImpl& operator = (const UtilityImpl&); //no impl

  private:
    NSDateFormatter* date_formatter;
    bool             system_version_5_0_available;
};

typedef common::Singleton <UtilityImpl> Utility;

void release_ns_object (const void* handle);

}

}

@interface AchievementHandle : NSObject
{
  @private
    GKAchievement*            achievement;
    GKAchievementDescription* description;
}

@property (retain) GKAchievement*            achievement;
@property (retain) GKAchievementDescription* description;

@end

#endif
