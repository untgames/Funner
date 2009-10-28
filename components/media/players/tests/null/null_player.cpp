#include <cstdio>

#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/players.h>

using namespace media::players;

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

int main ()
{
  printf ("Results of null_player_test:\n");

  try
  {
    MediaPlayer player ("null", "stream1");
    
    MediaPlayer::EventHandler handler = &media_player_event_handler;
    
    for (int i=0; i<MediaPlayerEvent_Num; i++)
      player.RegisterEventHandler ((MediaPlayerEvent)i, handler);

    player.SetName ("null player");
    
    printf ("Playing...\n");
    
    player.Play ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
