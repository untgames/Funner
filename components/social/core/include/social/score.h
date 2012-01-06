#ifndef SOCIAL_SCORE_HEADER
#define SOCIAL_SCORE_HEADER

#include <social/session_manager.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace social
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очки
///////////////////////////////////////////////////////////////////////////////////////////////////
  //non virtual, with internal reference counter
class Score
{
  public:
    typedef xtl::com_ptr<Score> Pointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Pointer Create (const char* leaderboard) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char*                PlayerId         () = 0;
    virtual void                       SetPlayerId      (const char* player) = 0;
    virtual const char*                LeaderboardId    () = 0;
    virtual void                       SetLeaderboardId (const char* leaderboard) = 0;
    virtual const char*                UserData         () = 0;
    virtual void                       SetUserData      (const char* user_data) = 0;
    virtual double                     Value            () = 0;
    virtual void                       SetValue         (double value) = 0;
    virtual const char*                FormattedValue   () = 0;
    virtual size_t                     Rank             () = 0;
    virtual const common::PropertyMap& Properties    () = 0;
    virtual void                       SetProperties (const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (OperationStatus status, const char* error)> ReportCallback;

    virtual void Report (const ReportCallback& callback) = 0; //move to ScoresManager/LeaderboardManager

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
//    virtual void AddRef  () = 0;
//    virtual void Release () = 0;

  protected:
    virtual ~Score () {}
};

}

#endif
