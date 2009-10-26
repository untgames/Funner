#include <cstdio>
#include <exception>

#include <media/players/media_player.h>

int main ()
{
  printf ("Results of iphone_player_test:\n");

  try
  {
    media::players::MediaPlayer music_player ("music", "music.mp3"), video_player ("video", "video.mov");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
