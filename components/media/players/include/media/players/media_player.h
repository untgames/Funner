#ifndef MEDIALIB_PLAYER_MEDIA_PLAYER_HEADER
#define MEDIALIB_PLAYER_MEDIA_PLAYER_HEADER

#include <stl/auto_ptr.h>

#include <xtl/dynamic_cast_root.h>
#include <xtl/functional_fwd>

namespace media
{

namespace players
{

//forward declarations
class Playlist;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние проигрывателя медиа-потока
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerState
{  
  MediaPlayerState_Stopped, //остановлен
  MediaPlayerState_Playing, //проигрывается
  MediaPlayerState_Paused,  //приостановлен (остановлен без сброса позиции)
  
  MediaPlayerState_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режимы проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerRepeatMode
{
  MediaPlayerRepeatMode_Off,  //повторение проигрывания потоков выключено
  MediaPlayerRepeatMode_Last, //повторяется последний выбранный поток в списке проигрывания
  MediaPlayerRepeatMode_All,  //повторяется весь список проигрывания

  MediaPlayerRepeatMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События проигрывателя медиа-потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MediaPlayerEvent
{
  MediaPlayerEvent_OnChangeName,       //изменено имя
  MediaPlayerEvent_OnChangeTarget,     //изменена цель проигрывания
  MediaPlayerEvent_OnChangePlaylist,   //изменён список проигрывания
  MediaPlayerEvent_OnChangeTrack,      //переключен трек
  MediaPlayerEvent_OnChangePlayback,   //изменены настройки проигрывания (play / pause / stop / position)
  MediaPlayerEvent_OnChangeVolume,     //изменены настройки громкости
  MediaPlayerEvent_OnChangeRepeatMode, //изменён режим повторения песен
  
  MediaPlayerEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрыватель медиа-потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
class MediaPlayer : public xtl::dynamic_cast_root
{
  public:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayer  ();
    MediaPlayer  (const char* target_name);
    MediaPlayer  (const char* target_name, const char* stream_name);
    MediaPlayer  (const char* target_name, const players::Playlist& list);
    ~MediaPlayer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текущая цель проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetTarget (const char* target_name);    
    const char* Target    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие / закрытие потока (список проигрывание клонируется)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Open  (const char* stream_name);
    void Open  (const players::Playlist&);
    void Close ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение списка проигрывания (если воспроизведение активно, оно не останавливается;
///если имя текущего трека не соответствует имени трека в новом списке проигрывания - номер
///следующего трека сбрасывается в 0, номер текущего трека в -1)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdatePlaylist (const players::Playlist&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    const players::Playlist& Playlist () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество треков
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int TracksCount () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим повторения источников
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                  SetRepeatMode (MediaPlayerRepeatMode mode);
    MediaPlayerRepeatMode RepeatMode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Номер проигрываемого трека / имя потока / его длительность
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetTrack         (unsigned int track);       //установка текущего трека
    void         SetNextPlayTrack (unsigned int track);       //установка следующего трека
    int          Track            () const;                   //текущий трек
    unsigned int NextPlayTrack    () const;                   //следующий трек
    const char*  Source           (unsigned int track) const; //имя трека
    const char*  Source           () const;                   //имя прогрываемого трека
    float        Duration         (unsigned int tack) const;  //длительность трека
    float        Duration         () const;                   //длительность проигрываемого трека
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переключение треков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void NextTrack ();
    void PrevTrack ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление проигрыванием
///////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayerState State () const; //состояние проигрывания
    void             Play  ();       //начать / продолжить проигрывание
    void             Pause ();       //приостановить проигрывание
    void             Stop  ();       //остановить проигрывание

    bool IsPlaying () const { return State () == MediaPlayerState_Playing; } //проигрывается ли поток
    bool IsPaused  () const { return State () == MediaPlayerState_Paused; }  //приостановлен ли поток
    bool IsStopped () const { return State () == MediaPlayerState_Stopped; } //остановлен ли поток

///////////////////////////////////////////////////////////////////////////////////////////////////
///Позиция проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetPosition (float position);
    float Position    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление громкостью
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetVolume (float volume); //in range [0..1]
    float Volume    () const;

    void SetMute (bool state); //включен или отключен ли звук
    bool IsMuted () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещения о событиях плеера
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (MediaPlayer&, MediaPlayerEvent)> EventHandler;

    xtl::connection RegisterEventHandler (MediaPlayerEvent event, const EventHandler& handler) const;
    
  private:
    MediaPlayer  (const MediaPlayer&); //no impl
    MediaPlayer& operator = (const MediaPlayer&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;    
};

}

}

#endif
