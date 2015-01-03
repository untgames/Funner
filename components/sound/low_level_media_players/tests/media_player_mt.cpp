#include "shared.h"

using namespace media::players;

class MtStreamPlayer: public IStreamPlayer
{
  public:
    MtStreamPlayer (const char*, const StreamPlayerManager::StreamEventHandler& in_handler)
      : handler (in_handler)
      , need_stop_actions (false)
      , action_thread ("Action thread", xtl::bind (&MtStreamPlayer::DoActions, this))
    {      
    }
    
    ~MtStreamPlayer ()
    {
      need_stop_actions = true;

      action_thread.Join ();
    }

    int DoActions ()
    {
      for (;;)
      {
        if (need_stop_actions)
          return 0;

        handler (this, StreamEvent_OnAutoStop);
      }

      return 0;
    }

    float Duration ()
    {
      return 1.0f;
    }
    
    void Play () 
    {    
      handler (this, StreamEvent_OnPlay);
    }
    
    void Pause ()
    {
      handler (this, StreamEvent_OnPause);
    }
    
    void Stop ()
    {
      handler (this, StreamEvent_OnManualStop);
    }
    
    void SetPosition (float in_position)
    {
    }
    
    float Position () 
    {
      return 0.f;
    }
    
    void SetLooping (bool state)
    {
    }
    
    void SetVolume (float in_volume)
    {
    }

    static IStreamPlayer* Create (const char*, const char* stream, const StreamPlayerManager::StreamEventHandler& handler)
    {
      return new MtStreamPlayer (stream, handler);
    }
    
  private:
    StreamPlayerManager::StreamEventHandler handler;
    bool                                    need_stop_actions;
    syslib::Thread                          action_thread;
};

void media_player_event_handler (MediaPlayer& player, MediaPlayerEvent event)
{
  switch (event)
  {
    case MediaPlayerEvent_OnChangeName:
      printf ("media player '%s' change name\n", player.Name ());
      break;
    case MediaPlayerEvent_OnChangeTarget:
      printf ("media player '%s' change target to '%s'\n", player.Name (), player.Target ());
      break;
    case MediaPlayerEvent_OnChangePlaylist:
      printf ("media player '%s' change play list\n", player.Name ());
      break;
    case MediaPlayerEvent_OnChangeTrack:
      printf ("media player '%s' change track to %u\n", player.Name (), player.Track ());
      break;
    case MediaPlayerEvent_OnChangePlayback:
    {
      const char* state = "unknown";
      
      switch (player.State ())
      {
        case MediaPlayerState_Playing: state = "playing"; break;
        case MediaPlayerState_Paused:  state = "paused"; break;
        case MediaPlayerState_Stopped: state = "stopped"; break;
        default: break;
      }
      
      printf ("media player '%s' change playback: state=%s, track=%u, source=%s, duration=%.2f, position=%.2f\n",
        player.Name (), state, player.Track (), player.Source (), player.Duration (), player.Position ());
        
      break;
    }      
    case MediaPlayerEvent_OnChangeVolume:
      printf ("media player '%s' change volume: mute=%s, volume=%.2f\n",
        player.Name (), player.IsMuted () ? "true" : "false", player.Volume ());
      break;
    case MediaPlayerEvent_OnChangeRepeatMode:
    {
      const char* mode = "unknown";
      
      switch (player.RepeatMode ())
      {
        case MediaPlayerRepeatMode_Off:   mode = "off"; break;
        case MediaPlayerRepeatMode_Last:  mode = "last"; break;
        case MediaPlayerRepeatMode_All:   mode = "all"; break;
        default: break;
      }
      
      printf ("media player '%s' change repeat mode to '%s'\n",
        player.Name (), mode);
    
      break;
    }
    default:
      break;
  }
}

void idle (MediaPlayer& player)
{
  Playlist playlist;

  playlist.AddSource (common::format ("test_track%d", rand ()).c_str ());
  playlist.AddSource (common::format ("test_track%d", rand ()).c_str ());
  playlist.AddSource (common::format ("test_track%d", rand ()).c_str ());

  player.UpdatePlaylist (playlist);
}

int main ()
{
  printf ("Results of media_player_mt test:\n");

  try
  {
    StreamPlayerManager::RegisterPlayer ("test", &MtStreamPlayer::Create);
    
    MediaPlayer player;
    
    MediaPlayer::EventHandler handler = &media_player_event_handler;
    
    for (int i=0; i<MediaPlayerEvent_Num; i++)
      player.RegisterEventHandler ((MediaPlayerEvent)i, handler);

    player.SetName ("player");
    player.SetTarget ("test");
    
    Playlist playlist;

    playlist.AddSource ("test_track1");
    playlist.AddSource ("test_track2");
    playlist.AddSource ("test_track3");

    player.UpdatePlaylist (playlist);

    player.SetRepeatMode (MediaPlayerRepeatMode_All);

    player.Play ();

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (player)));

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
