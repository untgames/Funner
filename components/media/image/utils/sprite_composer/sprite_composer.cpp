#include <cmath>
#include <cstdio>
//#include <cstdlib>

#include <stl/algorithm>

#include <common/strlib.h>

#include <media/image.h>

using namespace media;

size_t optimal_rows_count (size_t frame_width, size_t frame_height, size_t frames_count)
{
  size_t optimal_rows_count = 1,
         max_side = stl::max (frame_width * frames_count, frame_height);

  for (size_t rows_count = 2; rows_count <= frames_count; rows_count++)
  {
    if ((rows_count * (size_t)ceil (frames_count / (float)rows_count)) != frames_count)
      continue;

    size_t current_max_side = stl::max (frame_height * rows_count, (size_t)(frame_width * ceil (frames_count / (float)rows_count)));

    if (current_max_side < max_side)
    {
      max_side           = current_max_side;
      optimal_rows_count = rows_count;
    }
  }

  return optimal_rows_count;
}

int main (int argc, char *argv[])
{
  if (argc != 6)
  {
    printf ("Usage: sprite_composer base_name extension start_frame frames_count result_name\n");
    return 1;
  }

  int start_frame  = atoi (argv [3]),
      frames_count = atoi (argv [4]);

  if (frames_count < 1)
  {
    printf ("Invalid frames_count argument\n");
    return 1;
  }

  const char *base_name = argv [1],
             *extension = argv [2];

  try
  {
    size_t current_frame = start_frame;

    Image frame (common::format ("%s%04d.%s", base_name, current_frame, extension).c_str ());

    size_t frame_width  = frame.Width (),
           frame_height = frame.Height ();

    size_t rows_count    = optimal_rows_count (frame.Width (), frame.Height (), frames_count),
           columns_count = frames_count / rows_count;

    Image result_image (frame.Width () * columns_count, frame.Height() * rows_count, 1, frame.Format ());

    for (size_t y = 0; y < rows_count; y++)
    {
      for (size_t x = 0; x < columns_count; x++, current_frame++)
      {
        frame.Load (common::format ("%s%04d.%s", base_name, current_frame, extension).c_str ());

        result_image.PutImage (x * frame_width, y * frame_height, 0, frame_width, frame_height, 1, frame.Format (), frame.Bitmap ());
      }
    }

    result_image.Save (argv [5]);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
