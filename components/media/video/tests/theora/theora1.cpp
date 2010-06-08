#include <cstdio>
#include <exception>

#include <stl/string>

#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/video.h>
#include <media/image.h>

using namespace media;

const char* RESULTS_DIR = "results";

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

  if (!common::FileSystem::IsDir (RESULTS_DIR))
    common::FileSystem::Mkdir (RESULTS_DIR);

  for (size_t i = 0, count = sizeof (TEST_FILES) / sizeof (TEST_FILES [0]); i < count; i++)
  {
    printf ("Opening file '%s'\n", TEST_FILES [i]);

    try
    {
      VideoStream stream (TEST_FILES [i]);

      dump (stream);

      Image image (stream.Width (), stream.Height (), 1, PixelFormat_RGBA8);

      stream.Decode (0, image);

      stl::string image_name = common::format ("%s/%s.png", RESULTS_DIR, common::notdir (common::basename (TEST_FILES [i])).c_str ());

      image.Save (image_name.c_str ());
    }
    catch (std::exception& e)
    {
      printf ("exception: %s\n", e.what ());
    }
  }

  return 0;
}
