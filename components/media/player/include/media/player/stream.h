#ifndef MEDIALIB_PLAYER_STREAM_HEADER
#define MEDIALIB_PLAYER_STREAM_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace player
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс реализации проигрывателей медиа-потока
///////////////////////////////////////////////////////////////////////////////////////////////////
class IStreamPlayer
{
  public:  
    virtual ~IStreamPlayer () {}

    virtual float Duration    () = 0;                 //длительность потока
    virtual void  Play        () = 0;                 //начать проигрывание
    virtual void  Pause       () = 0;                 //приостановить проигрывание
    virtual void  Stop        () = 0;                 //остановить проигрывание
    virtual void  SetPosition (float position) = 0;   //установить позицию проигрывания
    virtual float Position    () = 0;                 //получить позицию проигрывания    
    virtual void  SetLooping  (bool state) = 0;       //установка режима циклического проигрывания    
    virtual void  SetVolume   (float volume) = 0;     //установить громкость
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер проигрывателей медиа-потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
class StreamPlayerManager
{
  public:
    typedef xtl::function<void (StreamPlayerState)> StateChangeCallback;  
    typedef xtl::function<IStreamPlayer* (const char* target_name, const char* source_name, const StateChangeCallback& callback)> PlayerCreater;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация проигрывателей медиа-потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPlayer       (const char* target, const PlayerCreater& creater);
    static void UnregisterPlayer     (const char* target);
    static void UnregisterAllPlayers ();
};

}

}

#endif
