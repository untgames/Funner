#include "shared.h"

#include <media/players/media_player.h>
#include <media/players/playlist.h>

using namespace media::players;
using namespace script;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* MEDIA_PLAYER_LIBRARY                    = "Media.MediaPlayer";
const char* MEDIA_STATIC_PLAYER_STATE_LIBRARY       = "Media.MediaPlayerState";
const char* MEDIA_STATIC_PLAYER_REPEAT_MODE_LIBRARY = "Media.MediaPlayerRepeatMode";
const char* MEDIA_STATIC_PLAYER_EVENT_LIBRARY       = "Media.MediaPlayerEvent";
const char* MEDIA_PLAYLIST_LIBRARY                  = "Media.Playlist";
const char* BINDER_NAME                             = "MediaPlayers";
const char* COMPONENT_NAME                          = "script.binds.MediaPlayers";

typedef xtl::shared_ptr<MediaPlayer>           MediaPlayerPtr;
typedef xtl::function<void (MediaPlayerEvent)> MediaPlayerEventHandler;

//Создание плейлиста
Playlist create_playlist ()
{
  return Playlist ();
}

//Регистрация библиотеки работы с плейлистами
void bind_playlist_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (MEDIA_PLAYLIST_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_playlist));

    //регистрация операций

  lib.Register ("get_Capacity", make_invoker (&Playlist::Capacity));
  lib.Register ("get_Empty",    make_invoker (&Playlist::IsEmpty));
  lib.Register ("get_Id",       make_invoker (&Playlist::Id));
  lib.Register ("get_Size",     make_invoker (&Playlist::Size));

  lib.Register ("AddSource",    make_invoker (&Playlist::AddSource));
  lib.Register ("Clear",        make_invoker (&Playlist::Clear));
  lib.Register ("Clone",        make_invoker (&Playlist::Clone));
  lib.Register ("Item",         make_invoker (&Playlist::Item));
  lib.Register ("RemoveSource", make_invoker (make_invoker (implicit_cast<void (Playlist::*) (const char*)> (&Playlist::RemoveSource)),
                                              make_invoker (implicit_cast<void (Playlist::*) (size_t)> (&Playlist::RemoveSource))));
  lib.Register ("Reserve",      make_invoker (&Playlist::Reserve));

  environment.RegisterType<Playlist> (MEDIA_PLAYLIST_LIBRARY);
}

//Регистрация статических переменных
void bind_static_media_player_library (Environment& environment)
{
  InvokerRegistry& player_state_lib       = environment.CreateLibrary (MEDIA_STATIC_PLAYER_STATE_LIBRARY);
  InvokerRegistry& player_repeat_mode_lib = environment.CreateLibrary (MEDIA_STATIC_PLAYER_REPEAT_MODE_LIBRARY);
  InvokerRegistry& player_event_lib       = environment.CreateLibrary (MEDIA_STATIC_PLAYER_EVENT_LIBRARY);

  player_state_lib.Register       ("get_Stopped",            make_const (MediaPlayerState_Stopped));
  player_state_lib.Register       ("get_Playing",            make_const (MediaPlayerState_Playing));
  player_state_lib.Register       ("get_Paused",             make_const (MediaPlayerState_Paused));
  player_repeat_mode_lib.Register ("get_Off",                make_const (MediaPlayerRepeatMode_Off));
  player_repeat_mode_lib.Register ("get_Last",               make_const (MediaPlayerRepeatMode_Last));
  player_repeat_mode_lib.Register ("get_All",                make_const (MediaPlayerRepeatMode_All));
  player_event_lib.Register       ("get_OnChangeName",       make_const (MediaPlayerEvent_OnChangeName));
  player_event_lib.Register       ("get_OnChangeTarget",     make_const (MediaPlayerEvent_OnChangeTarget));
  player_event_lib.Register       ("get_OnChangePlaylist",   make_const (MediaPlayerEvent_OnChangePlaylist));
  player_event_lib.Register       ("get_OnChangeTrack",      make_const (MediaPlayerEvent_OnChangeTrack));
  player_event_lib.Register       ("get_OnChangePlayback",   make_const (MediaPlayerEvent_OnChangePlayback));
  player_event_lib.Register       ("get_OnChangeVolume",     make_const (MediaPlayerEvent_OnChangeVolume));
  player_event_lib.Register       ("get_OnChangeRepeatMode", make_const (MediaPlayerEvent_OnChangeRepeatMode));
}

//Создание проигрывателя
MediaPlayerPtr create_player ()
{
  return MediaPlayerPtr (new MediaPlayer);
}

MediaPlayerPtr create_player_with_target (const char* target_name)
{
  return MediaPlayerPtr (new MediaPlayer (target_name));
}

MediaPlayerPtr create_player_with_target_and_stream (const char* target_name, const char* stream_name)
{
  return MediaPlayerPtr (new MediaPlayer (target_name, stream_name));
}

MediaPlayerPtr create_player_with_target_and_playlist (const char* target_name, const Playlist& playlist)
{
  return MediaPlayerPtr (new MediaPlayer (target_name, playlist));
}

xtl::connection player_register_event_handler (MediaPlayer& player, MediaPlayerEvent event, const MediaPlayerEventHandler& handler)
{
  return player.RegisterEventHandler (event, xtl::bind (handler, _2));
}

//Регистрация библиотеки работы с проигрывателями
void bind_media_player_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (MEDIA_PLAYER_LIBRARY);

    //регистрация статических переменных
  bind_static_media_player_library (environment);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (&create_player_with_target_and_playlist),
                                        make_invoker (&create_player_with_target_and_stream),
                                        make_invoker (&create_player_with_target),
                                        make_invoker (&create_player)));

  //регистрация операций

  lib.Register ("get_Duration",    make_invoker (implicit_cast<float (MediaPlayer::*) () const> (&MediaPlayer::Duration)));
  lib.Register ("get_IsPaused",    make_invoker (&MediaPlayer::IsPaused));
  lib.Register ("get_IsPlaying",   make_invoker (&MediaPlayer::IsPlaying));
  lib.Register ("get_IsStopped",   make_invoker (&MediaPlayer::IsStopped));
  lib.Register ("get_Mute",        make_invoker (&MediaPlayer::IsMuted));
  lib.Register ("get_Name",        make_invoker (&MediaPlayer::Name));
  lib.Register ("get_Playlist",    make_invoker (&MediaPlayer::Playlist));
  lib.Register ("get_Position",    make_invoker (&MediaPlayer::Position));
  lib.Register ("get_RepeatMode",  make_invoker (&MediaPlayer::RepeatMode));
  lib.Register ("get_Source",      make_invoker (implicit_cast<const char* (MediaPlayer::*) () const> (&MediaPlayer::Source)));
  lib.Register ("get_State",       make_invoker (&MediaPlayer::State));
  lib.Register ("get_Target",      make_invoker (&MediaPlayer::Target));
  lib.Register ("get_Track",       make_invoker (&MediaPlayer::Track));
  lib.Register ("get_TracksCount", make_invoker (&MediaPlayer::TracksCount));
  lib.Register ("get_Volume",      make_invoker (&MediaPlayer::Volume));
  lib.Register ("set_Mute",        make_invoker (&MediaPlayer::SetMute));
  lib.Register ("set_Name",        make_invoker (&MediaPlayer::SetName));
  lib.Register ("set_Position",    make_invoker (&MediaPlayer::SetPosition));
  lib.Register ("set_RepeatMode",  make_invoker (&MediaPlayer::SetRepeatMode));
  lib.Register ("set_Target",      make_invoker (&MediaPlayer::SetTarget));
  lib.Register ("set_Track",       make_invoker (&MediaPlayer::SetTrack));
  lib.Register ("set_Volume",      make_invoker (&MediaPlayer::SetVolume));

  lib.Register ("Close",     make_invoker (&MediaPlayer::Close));
  lib.Register ("Duration",  make_invoker (implicit_cast<float (MediaPlayer::*) (size_t) const> (&MediaPlayer::Duration)));
  lib.Register ("NextTrack", make_invoker (&MediaPlayer::NextTrack));
  lib.Register ("Open",      make_invoker (make_invoker (implicit_cast<void (MediaPlayer::*) (const char*)> (&MediaPlayer::Open)),
                                           make_invoker (implicit_cast<void (MediaPlayer::*) (const Playlist&)> (&MediaPlayer::Open))));
  lib.Register ("Pause",     make_invoker (&MediaPlayer::Pause));
  lib.Register ("Play",      make_invoker (&MediaPlayer::Play));
  lib.Register ("PrevTrack", make_invoker (&MediaPlayer::PrevTrack));
  lib.Register ("Source",    make_invoker (implicit_cast<const char* (MediaPlayer::*) (size_t) const> (&MediaPlayer::Source)));
  lib.Register ("Stop",      make_invoker (&MediaPlayer::Stop));

  lib.Register ("CreateEventHandler",   make_callback_invoker<MediaPlayerEventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (&player_register_event_handler));

  environment.RegisterType<MediaPlayer> (MEDIA_PLAYER_LIBRARY);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_playlist_library     (environment);
      bind_media_player_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> MediaPlayersScriptBind (COMPONENT_NAME);

}

}
