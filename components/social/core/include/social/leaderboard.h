#ifndef SOCIAL_LEADERBOARD_HEADER
#define SOCIAL_LEADERBOARD_HEADER

#include <xtl/functional_fwd>

#include <social/collection.h>
#include <social/common.h>
#include <social/score.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace social
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
class Leaderboard
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор / копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    Leaderboard ();
    Leaderboard (const void* handle);
    Leaderboard (const Leaderboard&);
    ~Leaderboard ();

    Leaderboard& operator = (const Leaderboard&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                Id            () const;
    void                       SetId         (const char* id);
    const char*                Title         () const;
    void                       SetTitle      (const char* title);
    const Score&               UserScore     () const;
    void                       SetUserScore  (const Score& score);
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рекорды
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ScoreCollection& Scores () const;
          ScoreCollection& Scores ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение низкоуровневого дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle () const;

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таблиц рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
class ILeaderboardManager: public virtual ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблицы рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const common::StringArray&, OperationStatus status, const char* error)>     LoadLeaderboardsIdsCallback;
    typedef xtl::function<void (const Leaderboard& leaderboard, OperationStatus status, const char* error)> LoadLeaderboardCallback;

    virtual void LoadLeaderboardsIds (const LoadLeaderboardsIdsCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LoadLeaderboard     (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LoadLeaderboard     (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (OperationStatus status, const char* error)> ReportScoreCallback;

    virtual void ReportScore (const Score& score, const ReportScoreCallback& callback, const common::PropertyMap& properties) = 0;
};

}

#endif
