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
    Leaderboard (const Leaderboard&);
    ~Leaderboard ();

    Leaderboard& operator = (const Leaderboard&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Id    () const;
    void        SetId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Название
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Title    () const;
    void        SetTitle (const char* title);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очки залогиненного пользователя
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Score& UserScore    () const;
    void         SetUserScore (const Score& score);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Другие свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рекорды
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ScoreList& Scores    () const;
          ScoreList& Scores    ();
    void             SetScores (const ScoreList& scores);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle    () const;
    void        SetHandle (const void* handle, const ReleaseHandleFunction& release_function = ReleaseHandleFunction ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Leaderboard&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Leaderboard&, Leaderboard&);

typedef Collection<Leaderboard> LeaderboardList;

typedef xtl::function<void (const LeaderboardList&, OperationStatus status, const char* error)>         LoadLeaderboardsCallback;
typedef xtl::function<void (const Leaderboard& leaderboard, OperationStatus status, const char* error)> LoadLeaderboardCallback;
typedef xtl::function<void (OperationStatus status, const char* error)>                                 SendScoreCallback;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таблиц рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
class ILeaderboardManager: public virtual ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблицы рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void LoadLeaderboards (const LoadLeaderboardsCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LoadLeaderboard  (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LoadLeaderboard  (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties) = 0;
};

}

#endif
