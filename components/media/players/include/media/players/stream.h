#ifndef MEDIALIB_PLAYER_STREAM_HEADER
#define MEDIALIB_PLAYER_STREAM_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace players
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///События потока
///////////////////////////////////////////////////////////////////////////////////////////////////
enum StreamEvent
{
  StreamEvent_OnPlay,         //поток начал проигрываться
  StreamEvent_OnPause,        //проигрывание потока приостановлено
  StreamEvent_OnManualStop,   //проигрывание потока остановлено вручную
  StreamEvent_OnAutoStop,     //проигрывание потока остановлено автоматически по окончании его проигрывания
  
  StreamEvent_Num
};

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
    typedef xtl::function<void (IStreamPlayer*, StreamEvent)> StreamEventHandler;
    typedef xtl::function<IStreamPlayer* (const char* target_name, const char* source_name, const StreamEventHandler& handler)> PlayerCreator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация проигрывателей медиа-потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPlayer       (const char* target, const PlayerCreator& creator);
    static void UnregisterPlayer     (const char* target);
    static void UnregisterAllPlayers ();
};

}

}

#endif
