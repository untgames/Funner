#include <cstdio>
#include <cstdlib>

#include <media/image.h>

using namespace media;

int main (int argc, char *argv[])
{
  if (argc != 6)
  {
    printf ("Usage: animation_enhancer source_animation target_animation animation_width animation_height new_frames_between_two_frames\n");
    return 1;
  }

  int animation_width               = atoi (argv [3]),
      animation_height              = atoi (argv [4]),
      new_frames_between_two_frames = atoi (argv [5]);

  if ((animation_width < 1) || (animation_height < 1) || (new_frames_between_two_frames < 1))
  {
    printf ("Animation width, height and new_frames_between_two_frames must be greater than 0\n");
    return 1;
  }

  try
  {
    Image source_animation (argv [1]);

    if (source_animation.Width () < ((size_t)animation_width * 2))
    {
      printf ("Invalid animation_width, source animation must have at least two frames\n");
      return 1;
    }

    if (source_animation.Height () != animation_height)
    {
      printf ("Invalid animation_height, source animation height is %u\n", source_animation.Height ());
      return 1;
    }

    size_t target_frames_count = source_animation.Width () / animation_width * (new_frames_between_two_frames + 1);

    Image target_animation (animation_width * target_frames_count, animation_height, 1, source_animation.Format ());

    size_t image_bpp = get_bytes_per_pixel (source_animation.Format ());

    for (size_t i = 0, base_frame = 0; i < target_frames_count; i++)
    {
      size_t next_frame = base_frame + 1;

      if (next_frame == source_animation.Width () / animation_width)
        next_frame = 0;

      size_t frame1_x = base_frame * animation_width,
             frame1_y = 0,
             frame2_x = next_frame * animation_width,
             frame2_y = 0,
             target_x = i * animation_width,
             target_y = 0;

      float frame1_weight = 1.f - (i % (new_frames_between_two_frames + 1)) / (float)(new_frames_between_two_frames + 1),
            frame2_weight = 1.f - frame1_weight;

      for (int j = 0; j < animation_height; j++)
      {
        unsigned char *frame1_data = (unsigned char*)source_animation.Bitmap () + source_animation.Width () * (frame1_y + j) * image_bpp + frame1_x * image_bpp,
                      *frame2_data = (unsigned char*)source_animation.Bitmap () + source_animation.Width () * (frame2_y + j) * image_bpp + frame2_x * image_bpp,
                      *target_data = (unsigned char*)target_animation.Bitmap () + target_animation.Width () * (target_y + j) * image_bpp + target_x * image_bpp;

        for (size_t k = 0; k < animation_width * image_bpp; k++, frame1_data++, frame2_data++, target_data++)
          *target_data = (unsigned char)(*frame1_data * frame1_weight + *frame2_data * frame2_weight);
      }

      if (!((i + 1) % (new_frames_between_two_frames + 1)))
        base_frame++;
    }

    target_animation.Save (argv [2]);
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
