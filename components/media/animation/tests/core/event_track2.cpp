#include "shared.h"

using namespace media::animation;

void event_handler (float time, const char* event)
{
  printf ("event '%s' executed in time %.2f\n", event, time);
}

int main ()
{
  printf ("Results of event_track_2_test:\n");
  
  try
  {
    EventTrack track;

    track.AddEvent (0.1, 0.1, "event");
    track.GetEvents (0.f, 1.f, event_handler);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
