#include "shared.h"

const float SIMULATE_DT  = 0.2f;
const float SIMULATE_MAX = 6.0f;

class TestStreamPlayer: public IStreamPlayer
{
  static TestStreamPlayer* instance;
  public:
    TestStreamPlayer (const char* in_stream, const StreamPlayerManager::StreamEventHandler& in_handler)
      : position (0.0f)
      , handler (in_handler)
      , stream (in_stream)
    {      
    }
    
    ~TestStreamPlayer ()
    {      
    }
  
    float Duration ()
    {
      return 2.0f;
    }
    
    void Play () 
    {    
      printf ("TestStreamPlayer::Play(%s)\n", stream.c_str ());
      
      if (position >= Duration ())
        return;
        
      instance = this;
      
      handler (this, StreamEvent_OnPlay);
    }
    
    void Pause ()
    {
      printf ("TestStreamPlayer::Pause(%s)\n", stream.c_str ());
      
      handler (this, StreamEvent_OnPause);
    }
    
    void Stop ()
    {
      instance = 0;
    
      printf ("TestStreamPlayer::Stop(%s)\n", stream.c_str ());
      
      handler (this, StreamEvent_OnManualStop);
    }
    
    void SetPosition (float in_position)
    {
      printf ("TestStreamPlayer::SetPosition(%s, %.2f)\n", stream.c_str (), in_position);
      position = in_position;
    }
    
    float Position () 
    {
      return position;
    }
    
    void SetLooping (bool state)
    {
      printf ("TestStreamPlayer::SetLooping(%s, %s)\n", stream.c_str (), state ? "true" : "false");
      looping = state;
    }
    
    void SetVolume (float in_volume)
    {
      printf ("TestStreamPlayer::SetVolume(%s, %.2f)\n", stream.c_str (), in_volume);
      volume = in_volume;
    }

    void Simulate (float dt)
    {
      position += dt;
      
      if (position >= Duration ())
      {
        if (looping)
        {
          position = 0.0f;
        }
        else
        {
          position = Duration ();
          
          Stop ();

          handler (this, StreamEvent_OnAutoStop);
        }
      }
    }    
    
    static IStreamPlayer* Create (const char*, const char* stream, const StreamPlayerManager::StreamEventHandler& handler)
    {
      return new TestStreamPlayer (stream, handler);
    }
    
    static TestStreamPlayer& Instance ()
    {
      if (!instance)
        throw xtl::format_operation_exception ("TestStreamPlayer::Instance", "No TestStreamPlayer instance");
        
      return *instance;
    }
    
  private:
    float                                   position;
    float                                   volume;
    bool                                    looping;
    StreamPlayerManager::StreamEventHandler handler;
    stl::string                             stream;
};

TestStreamPlayer* TestStreamPlayer::instance = 0;

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
      printf ("media player '%s' change play list", player.Name ());
      dump ("", player.Playlist ());
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

void simulate (MediaPlayer& player, float time = SIMULATE_MAX)
{  
  player.Play ();
       
  while (player.State () != MediaPlayerState_Stopped && time > 0)
  {
    printf ("  track #%u stream '%s': %.2f of %.2f\n", player.Track (), player.Source (), player.Position (), player.Duration ());  
  
    TestStreamPlayer::Instance ().Simulate (SIMULATE_DT);            
    
    time -= SIMULATE_DT;    
  }
  
  player.Stop ();
}

int main ()
{
  printf ("Results of media_player1_test:\n");

  try
  {
    StreamPlayerManager::RegisterPlayer ("test", &TestStreamPlayer::Create);
    
    MediaPlayer player;
    
    MediaPlayer::EventHandler handler = &media_player_event_handler;
    
    for (int i=0; i<MediaPlayerEvent_Num; i++)
      player.RegisterEventHandler ((MediaPlayerEvent)i, handler);

    player.SetName ("player");
    player.SetTarget ("test");
    
    printf ("Open play list:\n");    
    
    Playlist list;
    
    list.AddSource ("source1");
    list.AddSource ("source2");       
    
    player.Open (list);
    
    printf ("Tracks count: %u\n", player.TracksCount ());
    
    printf ("Playing...\n");
    
    simulate (player);
    
    printf ("Playing with last looping...\n");
    
    player.SetRepeatMode (MediaPlayerRepeatMode_Last);
    
    simulate (player, 4.0f);
    
    printf ("Playing with all looping...\n");
    
    player.PrevTrack ();
    
    player.SetRepeatMode (MediaPlayerRepeatMode_All);
    
    simulate (player, 8.0f);
    
    printf ("Change mute\n");
    
    player.SetMute (true);
    
    printf ("volume=%.2f mute=%s\n", player.Volume (), player.IsMuted () ? "true" : "false");
    
    player.SetMute (false);
    
    printf ("volume=%.2f mute=%s\n", player.Volume (), player.IsMuted () ? "true" : "false");
    
    printf ("Dynamic change volume\n");
    
    player.NextTrack ();
    
    player.Play ();
    
    for (int i=0; i<5; i++)
      player.SetVolume (1.0f / 5 * i);

    printf ("before exit\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
