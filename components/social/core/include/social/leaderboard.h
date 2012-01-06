#ifndef SOCIAL_LEADERBOARD_HEADER
#define SOCIAL_LEADERBOARD_HEADER

#include <social/session_manager.h>
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
  //non virtual
class Leaderboard
{
  public:
    typedef xtl::com_ptr<Leaderboard> Pointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char*                Id            () = 0;
    virtual const char*                Title         () = 0;
    virtual Score::Pointer             UserScore     () = 0;
    virtual size_t                     Size          () = 0;
    virtual const Score&               Score         (size_t index) = 0;
    virtual const common::PropertyMap& Properties    () = 0;
    virtual void                       SetProperties (const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IUser () {}
};

}

#endif
