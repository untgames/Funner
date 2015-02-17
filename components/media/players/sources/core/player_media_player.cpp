#include "shared.h"

using namespace media::players;

namespace
{

/*
    Константы
*/

const char* COMPONENT_MASK = "media.players.stream.*"; //маска компонентов проигрывания медиа-потоков
const char* LOG_NAME       = "media::players::MediaPlayer";

///Класс устанавливающий значение переменной в true в конструкторе и в false в деструкторе
class PlayStateLockScope
{
  public:
    PlayStateLockScope (bool& in_lock_variable)
      : lock_variable (in_lock_variable)
    {
      lock_variable = true;
    }

    ~PlayStateLockScope ()
    {
      lock_variable = false;
    }

  private:
    bool& lock_variable;
};

}

/*
    Описание реализации проигрывателя медиа-потоков
*/

typedef xtl::shared_ptr<IStreamPlayer>                     StreamPtr;
typedef stl::vector<StreamPtr>                             StreamArray;
typedef xtl::signal<void (MediaPlayer&, MediaPlayerEvent)> MediaPlayerSignal;
typedef stl::vector<common::Action>                        ActionArray;

struct MediaPlayer::Impl : public common::Lockable
{
  MediaPlayer&                            owner;                          //владелец
  stl::string                             name;                           //имя проигрывателя
  stl::string                             target_name;                    //имя цели проигрывания
  media::players::Playlist                list;                           //список воспроизведения
  StreamArray                             streams;                        //проигрыватели потоков
  StreamPtr                               current_stream;                 //текущий проигрываемый поток
  StreamPlayerManager::StreamEventHandler callback_handler;               //обработчик изменения состояния проигрываемых потоков
  int                                     current_track;                  //текущий трек
  unsigned int                            next_track;                     //следующий трек
  MediaPlayerState                        current_track_state;            //состояние текущего трека
  bool                                    is_muted;                       //выключен ли звук
  bool                                    play_state_lock;                //предотвращение двойного вызова функции изменения состояния проигрывателя (play / pause / stop)
  float                                   volume;                         //текущая громкость
  MediaPlayerRepeatMode                   repeat_mode;                    //режим повтора проигрывания треков
  MediaPlayerSignal                       signals [MediaPlayerEvent_Num]; //обработчики событий проигрывателя
  ActionArray                             notify_actions;                 //действия оповещения о событиях
  
///Конструктор
  Impl (MediaPlayer& in_owner)
    : owner (in_owner)
    , callback_handler (xtl::bind (&Impl::OnStreamEvent, this, _1, _2))
    , current_track (-1)
    , next_track (0)
    , current_track_state (MediaPlayerState_Stopped)
    , is_muted (false)
    , play_state_lock (false)
    , volume (1.0f)
    , repeat_mode (MediaPlayerRepeatMode_Off)
  {
  }  
  
  ~Impl ()
  {
    for (ActionArray::iterator iter = notify_actions.begin (), end = notify_actions.end (); iter != end; ++iter)
      iter->Cancel ();
  }

  ///Update looping state for current playing track
  void UpdateCurrentTrackLooping ()
  {
    if (!current_stream || (unsigned int)current_track >= streams.size ())
      return;

    current_stream->SetLooping (current_track == streams.size () - 1 && repeat_mode == MediaPlayerRepeatMode_Last && current_stream == streams [current_track]);
  }

  ///Создание проигрывателей потоков
  void UpdatePlaylist (const media::players::Playlist& in_play_list)
  {
    try
    {
      static common::ComponentLoader loader (COMPONENT_MASK);
      
      StreamArray new_streams;

      {  //new_streams should not be under lock, so there will not be deadlock in stream destructor while stream event is waiting for lock
        common::Lock lock (*this);

        media::players::Playlist new_play_list = in_play_list.Id () == list.Id () ? list : in_play_list.Clone ();

        new_streams.reserve (new_play_list.Size ());

        for (unsigned int i=0, count=new_play_list.Size (); i<count; i++)
        {
          StreamPtr stream (create_stream_player (target_name.c_str (), new_play_list.Item (i), callback_handler));

          if (!stream)
            throw xtl::format_operation_exception ("", "Internal error: null player returned for target '%s' stream '%s'",
                                                   target_name.c_str (), new_play_list.Item (i));

          new_streams.push_back (stream);
        }

        if ((unsigned int)current_track >= list.Size () || (unsigned int)current_track >= new_play_list.Size () || xtl::xstrcmp (list.Item (current_track), new_play_list.Item (current_track)))
        {
          if (new_play_list.IsEmpty ())
          {
            current_track = -1;
            next_track    = 0;
          }
          else
          {
            current_track = 0;
            next_track    = 0;
          }
        }

        streams.swap (new_streams);

        if (current_stream)
          current_stream->SetLooping (false);

        bool is_closed = list.IsEmpty ();

        list = new_play_list;

        if (!is_closed || !list.IsEmpty ())
          Notify (MediaPlayerEvent_OnChangePlaylist);
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::players::MediaPlayer::Impl::UpdatePlaylist(const Playlist&)");
      throw;
    }    
  }
  
  void UpdatePlaylist (const char* stream_name)
  {
    try
    {
      media::players::Playlist new_play_list;
      
      new_play_list.AddSource (stream_name);
      
      UpdatePlaylist (new_play_list);
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::players::MediaPlayer::Impl::UpdatePlaylist(const char*)");
      throw;
    }
  }
  
///Корректен ли текущий поток
  bool IsCurrentTrackValid ()
  {
    return (unsigned int)current_track < streams.size ();
  }

///Текущий медиа-поток
  IStreamPlayer* CurrentTrackStream ()
  {
    return IsCurrentTrackValid () ? streams [current_track].get () : 0;
  }

///Обработчик событий медиа-потока
  void OnStreamEvent (IStreamPlayer* stream_player, StreamEvent event)
  {
    common::Lock lock (*this);

    try
    {
        //обрабатываются только события текущего трека
    
      if (stream_player != current_stream.get () || !stream_player)
        return;
        
      switch (event)
      {
        case StreamEvent_OnPlay:
          current_track_state = MediaPlayerState_Playing;
          break;
        case StreamEvent_OnPause:
          current_stream.reset ();
          current_track_state = MediaPlayerState_Paused;
          break;
        case StreamEvent_OnManualStop:
          current_stream.reset ();
          current_track_state = MediaPlayerState_Stopped;
          break;
        case StreamEvent_OnAutoStop:
        {
          StreamPtr current_stream_holder = current_stream;

          current_stream.reset ();

          current_track_state = MediaPlayerState_Stopped;
          
          stream_player->SetPosition (0.0f);
          
          if (next_track >= streams.size ())
          {
              //весь список проигран
              
            switch (repeat_mode)
            {
              case MediaPlayerRepeatMode_Off:
                Notify (MediaPlayerEvent_OnChangePlayback);
                current_track = 0;
                next_track = 0;
                Notify (MediaPlayerEvent_OnChangeTrack);
                return;
              case MediaPlayerRepeatMode_Last:
                break;
              case MediaPlayerRepeatMode_All:
                current_track = 0;
                next_track = 1;
                Notify (MediaPlayerEvent_OnChangeTrack);
                break;
              default:
                break;
            }
          }
          else
          {
            current_track = next_track;
            next_track++;
            
            Notify (MediaPlayerEvent_OnChangeTrack);
          }
          
          owner.Play ();
          
          break;
        }
        default:
          break;
      }
    }
    catch (xtl::exception& e)
    {
      common::Log (LOG_NAME).Printf ("Exception at 'media::players::MediaPlayer::Impl::OnStreamEvent': '%s'", e.what ());
    }
    catch (...)
    {
      common::Log (LOG_NAME).Printf ("Unknown exception at 'media::players::MediaPlayer::Impl::OnStreamEvent'");
    }    
  }
  
///Оповещения о возникновении событий
  void Notify (MediaPlayerEvent event)
  {
    for (int i = (int)notify_actions.size () - 1; i >= 0; i--)
    {
      if (notify_actions [i].IsCompleted ())
        notify_actions.erase (notify_actions.begin () + i);
    }

    notify_actions.push_back (common::ActionQueue::PushAction (xtl::bind (&MediaPlayer::Impl::NotifyImpl, this, event), common::ActionThread_Main));
  }

  void NotifyImpl (MediaPlayerEvent event)
  {
    try
    {
      if (event < 0 || event >= MediaPlayerEvent_Num)
        return;

      signals [event](owner, event);
    }
    catch (xtl::exception& e)
    {
      common::Log (LOG_NAME).Printf ("Exception at 'media::players::MediaPlayer::Impl::NotifyImpl': '%s'", e.what ());
    }
    catch (...)
    {
      common::Log (LOG_NAME).Printf ("Unknown exception at 'media::players::MediaPlayer::Impl::NotifyImpl'");
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

MediaPlayer::MediaPlayer ()  
  : impl (new Impl (*this))
{  
}

MediaPlayer::MediaPlayer (const char* target_name)
  : impl (new Impl (*this))
{
  try
  {       
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");     

    impl->target_name = target_name;
  }
  catch (xtl::exception& e)
  {          
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*)");
    throw;
  }
}

MediaPlayer::MediaPlayer (const char* target_name, const char* stream_name)  
  : impl (new Impl (*this))
{
  try
  {
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");
      
    if (!stream_name)
      throw xtl::make_null_argument_exception ("", "stream_name");

    impl->target_name = target_name;
    
    impl->UpdatePlaylist (stream_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*, const char*)");
    throw;
  }
}

MediaPlayer::MediaPlayer (const char* target_name, const media::players::Playlist& list)
  : impl (new Impl (*this))
{
  try
  {       
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");

    impl->target_name = target_name;

    impl->UpdatePlaylist (list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::MediaPlayer(const char*, const Playlist&)");
    throw;
  }
}

MediaPlayer::~MediaPlayer ()
{
  common::Lock lock (*impl);

  Close ();
}

/*
    Имя
*/

void MediaPlayer::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::players::MediaPlayer::SetName", "name");
    
  impl->name = name;
  
  impl->Notify (MediaPlayerEvent_OnChangeName);
}

const char* MediaPlayer::Name () const
{
  return impl->name.c_str ();
}

/*
    Текущая цель проигрывания
*/

void MediaPlayer::SetTarget (const char* target_name)
{
  try
  {
    if (!target_name)
      throw xtl::make_null_argument_exception ("", "target_name");

    {
      common::Lock lock (*impl);

      //закрытие всех проигрываемых потоков

      Close ();

      //обновление цели

      impl->target_name.clear ();

      impl->target_name = target_name;

      //оповещение о смене цели

      impl->Notify (MediaPlayerEvent_OnChangeTarget);
    }
    
      //обновление потоков
      
    impl->UpdatePlaylist (impl->list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetTarget");
    throw;
  }
}

//имя цели проигрывания
const char* MediaPlayer::Target () const
{
  return impl->target_name.c_str ();
}

/*
   Открытие / закрытие потока (список проигрывание клонируется)
*/

void MediaPlayer::Open (const char* stream_name)
{
  try
  {
    if (!stream_name)
      throw xtl::make_null_argument_exception ("", "stream_name");

    {
      common::Lock lock (*impl);

      Close ();
    }
    
    impl->UpdatePlaylist (stream_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Open(const char*)");
    throw;
  }
}

void MediaPlayer::Open (const media::players::Playlist& list)
{
  try
  {
    {
      common::Lock lock (*impl);

      Close ();
    }
    
    impl->UpdatePlaylist (list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Open(const Playlist&)");
    throw;
  }
}

void MediaPlayer::Close ()
{
  try
  {
    bool is_closed;

    {
      common::Lock lock (*impl);

      is_closed = impl->streams.empty ();

      switch (impl->current_track_state)
      {
        case MediaPlayerState_Playing:
        case MediaPlayerState_Paused:

          try
          {
            Stop ();
          }
          catch (xtl::exception& e)
          {
            common::Log (LOG_NAME).Printf ("Exception at 'media::players::MediaPlayer::Close': '%s'", e.what ());
          }
          catch (...)
          {
            common::Log (LOG_NAME).Printf ("Unknown exception at 'media::players::MediaPlayer::Close'");
          }

          break;
        case MediaPlayerState_Stopped:
        default:
          break;
      }

      impl->current_track_state = MediaPlayerState_Stopped;
    }

    UpdatePlaylist (media::players::Playlist ());
    
    if (!is_closed)
      impl->Notify (MediaPlayerEvent_OnChangeTrack);
  }
  catch (xtl::exception& e)
  {
    common::Log (LOG_NAME).Printf ("Exception at 'media::players::MediaPlayer::Close': '%s'", e.what ());
  }
  catch (...)
  {
    common::Log (LOG_NAME).Printf ("Unknown exception at 'media::players::MediaPlayer::Close'");
  }
}

/*
   Изменение списка проигрывания (если воспроизведение активно, оно не останавливается;
   если имя текущего трека не соответствует имени трека в новом списке проигрывания - номер
   следующего трека сбрасывается в 0, номер текущего трека в -1)
*/

void MediaPlayer::UpdatePlaylist (const media::players::Playlist& new_playlist)
{
  impl->UpdatePlaylist (new_playlist);
}

/*
        Список проигрывания
*/

const media::players::Playlist& MediaPlayer::Playlist () const
{
  return impl->list;
}

/*
    Количество треков
*/

unsigned int MediaPlayer::TracksCount () const
{
  return impl->list.Size ();
}

/*
    Режим повторения источников
*/

void MediaPlayer::SetRepeatMode (MediaPlayerRepeatMode mode)
{
  static const char* METHOD_NAME = "media::players::MediaPlayer::SetRepeatMode";
  
  common::Lock lock (*impl);

  if (mode < 0 || mode >= MediaPlayerRepeatMode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);      

  impl->repeat_mode = mode;

  impl->UpdateCurrentTrackLooping ();

  impl->Notify (MediaPlayerEvent_OnChangeRepeatMode);
}

MediaPlayerRepeatMode MediaPlayer::RepeatMode () const
{
  return impl->repeat_mode;
}

/*
    Номер проигрываемого трека / имя потока / его длительность
*/

//установка текущего трека
void MediaPlayer::SetTrack (unsigned int track)
{
  common::Lock lock (*impl);

  try
  {
    if (track >= impl->streams.size ())
      throw xtl::make_range_exception ("", "track", track, impl->streams.size ());
      
    MediaPlayerState state = State ();

    Stop ();

    impl->current_track = track;
    impl->next_track    = track + 1;

    impl->Notify (MediaPlayerEvent_OnChangeTrack);

    switch (state)
    {
      case MediaPlayerState_Playing:
      case MediaPlayerState_Paused:
        Play ();
        break;
      case MediaPlayerState_Stopped:
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetTrack");
    throw;
  }
}

void MediaPlayer::SetNextPlayTrack (unsigned int track)
{
  common::Lock lock (*impl);

  try
  {
    if (track >= impl->streams.size ())
      throw xtl::make_range_exception ("", "track", track, impl->streams.size ());

    impl->next_track = track;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetNextTrack");
    throw;
  }
}

//текущий трек
int MediaPlayer::Track () const
{
  common::Lock lock (*impl);

  return impl->current_track;
}

unsigned int MediaPlayer::NextPlayTrack () const
{
  common::Lock lock (*impl);

  return impl->next_track;
}

//имя трека
const char* MediaPlayer::Source (unsigned int track) const
{
  try
  {
    return impl->list.Item (track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Source(unsigned int)");
    throw;
  }
}

//имя прогрываемого трека
const char* MediaPlayer::Source () const
{
  return Source (Track ());
}

//длительность трека
float MediaPlayer::Duration (unsigned int track) const
{  
  try
  {
    if (track >= impl->streams.size ())
      throw xtl::make_range_exception ("", "track", track, impl->streams.size ());

    return impl->streams [track]->Duration ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Duration");
    throw;
  }
}

//длительность проигрываемого трека
float MediaPlayer::Duration () const
{
  common::Lock lock (*impl);

  if (impl->current_stream)
    return impl->current_stream->Duration ();

  return 0.f;
}

/*
    Переключение треков
*/

void MediaPlayer::NextTrack ()
{
  common::Lock lock (*impl);

  try
  {
    unsigned int new_track = impl->current_track + 1;

    if (new_track == impl->list.Size ())
      new_track = 0;
      
    SetTrack (new_track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::MediaPlayer::NextTrack");
    throw;
  }
}

void MediaPlayer::PrevTrack ()
{
  common::Lock lock (*impl);

  try
  {
    unsigned int new_track = impl->current_track - 1;
  
    if (new_track >= impl->list.Size ())
      new_track = impl->list.Size () ? impl->list.Size () - 1 : 0;

    SetTrack (new_track);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::MediaPlayer::PrevTrack");
    throw;
  }
}

/*
    Управление проигрыванием
*/

//состояние проигрывания
MediaPlayerState MediaPlayer::State () const
{
  common::Lock lock (*impl);

  return impl->current_track_state;
}

//начать / продолжить проигрывание
void MediaPlayer::Play ()
{
  common::Lock lock (*impl);

  if (impl->play_state_lock)
    return;

  PlayStateLockScope lock_scope (impl->play_state_lock);

  try
  {
    if (!impl->IsCurrentTrackValid ())
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
        SetPosition (0.0f);
        break;
      case MediaPlayerState_Paused:
        impl->CurrentTrackStream ()->Play ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);
        break;
      case MediaPlayerState_Stopped:
      {
        if (impl->current_track == impl->next_track)
          impl->next_track++;

        impl->current_stream = impl->streams [impl->current_track];

        impl->current_stream->SetVolume (impl->volume);
        impl->UpdateCurrentTrackLooping ();
        
        impl->current_stream->Play ();

        impl->Notify (MediaPlayerEvent_OnChangePlayback);

        break;
      }
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Play");
    throw;
  }
}

//приостановить проигрывание
void MediaPlayer::Pause ()
{
  common::Lock lock (*impl);

  if (impl->play_state_lock)
    return;

  PlayStateLockScope lock_scope (impl->play_state_lock);

  try
  {
    if (!impl->current_stream)
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
        impl->current_stream->Pause ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);       
        break;
      case MediaPlayerState_Paused:
        Play ();
        break;
      case MediaPlayerState_Stopped:
        break;
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Pause");
    throw;
  }
}

//остановить проигрывание
void MediaPlayer::Stop ()
{
  common::Lock lock (*impl);

  if (impl->play_state_lock)
    return;

  PlayStateLockScope lock_scope (impl->play_state_lock);

  try
  {
    if (!impl->current_stream)
      return;
    
    switch (impl->current_track_state)
    {
      case MediaPlayerState_Playing:
      case MediaPlayerState_Paused:
        impl->current_stream->Stop ();
        impl->Notify (MediaPlayerEvent_OnChangePlayback);
        break;
      case MediaPlayerState_Stopped:
        break;
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::Stop");
    throw;
  }
}

/*
    Позиция проигрывания
*/

void MediaPlayer::SetPosition (float position)
{
  common::Lock lock (*impl);

  try
  {
    if (!impl->current_stream)
      return;
      
    float duration = impl->current_stream->Duration ();
    
    if (position > duration)
      throw xtl::make_range_exception ("", "position", position, 0.f, duration);

    impl->current_stream->SetPosition (position);

    impl->Notify (MediaPlayerEvent_OnChangePlayback);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetPosition");
    throw;
  }
}

float MediaPlayer::Position () const
{
  common::Lock lock (*impl);

  if (impl->current_stream)
    return impl->current_stream->Position ();

  return 0.f;
}

/*
    Управление громкостью
*/

//in range [0..1]
void MediaPlayer::SetVolume (float volume)
{
  common::Lock lock (*impl);

  try
  {
    if (volume == impl->volume)
      return;
      
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
      
    if (impl->current_stream)
      impl->current_stream->SetVolume (volume);
      
    impl->volume   = volume;
    impl->is_muted = false;
    
    impl->Notify (MediaPlayerEvent_OnChangeVolume);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetVolume");
    throw;
  }
}

float MediaPlayer::Volume () const
{
  return impl->is_muted ? 0.0f : impl->volume;
}

//включен или отключен ли звук
void MediaPlayer::SetMute (bool state)
{
  common::Lock lock (*impl);

  try
  {
    if (impl->is_muted == state)
      return;
      
    if (impl->current_stream)
    {
      if (state) impl->current_stream->SetVolume (0.0f);
      else       impl->current_stream->SetVolume (impl->volume);
    }

    impl->is_muted = state;
    
    impl->Notify (MediaPlayerEvent_OnChangeVolume);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::players::MediaPlayer::SetMute");
    throw;
  }
}

bool MediaPlayer::IsMuted () const
{
  return impl->is_muted;
}

/*
    Оповещения о событиях плеера
*/

xtl::connection MediaPlayer::RegisterEventHandler (MediaPlayerEvent event, const EventHandler& handler) const
{
  static const char* METHOD_NAME = "media::players::MediaPlayer::RegisterEventHandler";
  
  common::Lock lock (*impl);

  if (event < 0 || event >= MediaPlayerEvent_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "event", event);
    
  return impl->signals [event].connect (handler);
}
