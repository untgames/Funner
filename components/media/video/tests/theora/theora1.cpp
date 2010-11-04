#include <cstdio>
#include <exception>

#include <stl/string>

#include <common/hash.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/video.h>
#include <media/image.h>

using namespace media;

const char* TEST_FILES [] = {
 "data/320x240.ogv",
 "data/320x240.ogx",
 "data/320x240.skeleton+cmml.ogv",
 "data/322x242_not-divisible-by-sixteen-framesize.ogx",
 "data/chained_streams.ogx",
 "data/ducks_take_off_444_720p25.ogx",
 "data/mobile_itu601_i_422.ogx",
 "data/multi2.ogx",
 "data/offset_test.ogv",
 "data/pixel_aspect_ratio.ogx",
 "data/sign_irene_cif-3qi-b.ogx",
 "data/stockholm-vfr.ogx",
 "data/videotestsrc-720x576-16-15.ogx",
};

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

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

int main ()
{
  printf ("Results of theora1_test:\n");

  common::LogFilter log_filter ("*", &log_handler);

  for (size_t i = 0, count = sizeof (TEST_FILES) / sizeof (TEST_FILES [0]); i < count; i++)
  {
    printf ("Opening file '%s'\n", TEST_FILES [i]);

    try
    {
      VideoStream stream (TEST_FILES [i]);

      dump (stream);

      Image image (stream.Width (), stream.Height (), 1, PixelFormat_RGBA8);

      size_t image_size = image.Width () * image.Height () * get_bytes_per_pixel (image.Format ());

      for (size_t j = 0; j < stream.FramesCount (); j++)
      {
        stream.Decode (j, image);

        printf ("Frame %03u hash is %08x\n", j, common::crc32 (image.Bitmap (0), image_size));
      }
    }
    catch (std::exception& e)
    {
      printf ("exception: %s\n", e.what ());
    }
  }

  return 0;
}
