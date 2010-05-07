#include "shared.h"

using namespace media::animation;

void event_handler (float time, const char* event)
{
  printf ("event '%s' executed in time %.2f\n", event, time);
}

int main ()
{
  printf ("Results of event_track_test:\n");
  
  try
  {
    EventTrack track;

    printf ("track capacity is %u\n", track.Capacity ());
    printf ("track size is %u\n", track.Size ());

    track.Reserve (1024);
    track.GetEvents (0, 5, event_handler);
    track.AddEvent (0, 1, "0-1 event (0)");
    track.AddEvent (1, 0, "1-0 event (1)");
    track.AddEvent (0, 0, "0-0 event (2)");
    track.AddEvent (2, 3, "2-3 event (3)");
    track.AddEvent (0, 5, "0-5 event (4)");
    track.GetEvents (0, 10, event_handler);

    printf ("track capacity is %u\n", track.Capacity ());
    printf ("track size is %u\n", track.Size ());

    printf ("Event 2 delay is %.2f, period is %.2f, event is '%s'\n", track.GetDelay (2), track.GetPeriod (2), track.GetEvent (2));

    track.Remove (3);
    track.SetEvent (2, "new event (2)");
    track.SetEvent (3, 1.5, 2.5, "1.5-2.5 new event (3)");
    track.GetEvents (0, 10, event_handler);

    printf ("track size is %u\n", track.Size ());

    track.Clear ();

    printf ("track size is %u\n", track.Size ());

    track.GetEvents (0, 10, event_handler);

    EventTrack copy_test (track), copy_test2;

    copy_test2 = copy_test;

    copy_test.Swap (copy_test2);
    swap (copy_test, copy_test2);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
