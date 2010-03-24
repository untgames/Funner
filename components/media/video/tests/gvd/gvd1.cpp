#include <cstdio>
#include <exception>

#include <stl/string>

#include <common/file.h>
#include <common/strlib.h>

#include <media/video.h>
#include <media/image.h>

using namespace media;

const char* RESULTS_DIR = "results";

const char* TEST_FILES [] = {
 "data/test1.gvd",
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

void dump_hash (const char* file_name)
{
  common::FileHash file_hash;

  common::FileSystem::GetFileHash (file_name,file_hash);

  printf ("  file '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  printf ("Results of gvd1_test:\n");

  if (!common::FileSystem::IsDir (RESULTS_DIR))
    common::FileSystem::Mkdir (RESULTS_DIR);

  for (size_t i = 0, count = sizeof (TEST_FILES) / sizeof (TEST_FILES [0]); i < count; i++)
  {
    printf ("Opening file '%s'\n", TEST_FILES [i]);

    try
    {
      VideoStream stream (TEST_FILES [i]);

      dump (stream);
      
      for (size_t j=0, count=stream.FramesCount (); j<count; j++)
      {
        Image image (stream.Width (), stream.Height (), 1, PixelFormat_RGBA8);

        stream.Decode (j, image);

        stl::string image_name = common::format ("%s/%s_%03u.png", RESULTS_DIR, 
          common::notdir (common::basename (TEST_FILES [i])).c_str (), j);

        image.Save (image_name.c_str ());

        dump_hash (image_name.c_str ());
      }
    }
    catch (std::exception& e)
    {
      printf ("exception: %s\n", e.what ());
    }
  }

  return 0;
}
