#include "shared.h"

void dump (const VideoStream& s)
{
  printf ("Stream '%s':\n", s.Name ());
  printf ("  frames_count:      %u\n", s.FramesCount ());
  printf ("  frames_per_second: %.4f\n", s.FramesPerSecond ());
  printf ("  width:             %u\n", s.Width ());
  printf ("  height:            %u\n", s.Height ());
  printf ("  frame_size:        %u\n", s.FrameSize ());
  printf ("  duration:          %.3f\n", s.Duration ());
}

int main ()
{
  printf ("Results of stream1_test:\n");
  
  try
  {
    VideoStream stream1;
    
    dump (stream1);
    
    VideoStream stream2 = stream1;
    
    stream2.Rename ("stream2");
    
    dump (stream2);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
