#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <stl/algorithm>

#include <xtl/string.h>

#include <media/image.h>

using namespace media;

const char* BLEND_3_FRAMES = "-3_frames_blending";

//??????? Доработать проверку входных данных; квадратные входные картинки

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
  if ((argc != 6) && (argc != 7))
  {
    printf ("Usage: animation_enhancer source_animation target_animation animation_width animation_height new_frames_between_two_frames [%s]\n", BLEND_3_FRAMES);
    return 1;
  }

  bool use_3_frames_blending = false;

  if (argc == 7)
  {
    if (!xtl::xstrcmp (argv [6], BLEND_3_FRAMES))
      use_3_frames_blending = true;
    else
    {
      printf ("Invalid 6 parameter\nUsage: animation_enhancer source_animation target_animation animation_width animation_height new_frames_between_two_frames [%s]\n", BLEND_3_FRAMES);
      return 1;
    }
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

    if (source_animation.Height () != (size_t)animation_height)
    {
      printf ("Invalid animation_height, source animation height is %u\n", source_animation.Height ());
      return 1;
    }

    size_t target_frames_count  = source_animation.Width () / animation_width * (new_frames_between_two_frames + 1),
           target_rows_count    = optimal_rows_count (animation_width, animation_height, target_frames_count),
           target_columns_count = (size_t)ceil (target_frames_count / (float)target_rows_count);

    Image target_animation (animation_width * target_columns_count, animation_height * target_rows_count, 1, source_animation.Format ());

    size_t image_bpp = get_bytes_per_pixel (source_animation.Format ());

    if (!use_3_frames_blending)
    {
      for (size_t i = 0, base_frame = 0; i < target_frames_count; i++)
      {
        size_t next_frame = base_frame + 1;

        if (next_frame == source_animation.Width () / animation_width)
          next_frame = 0;

        size_t frame1_x = base_frame * animation_width,
               frame1_y = 0,
               frame2_x = next_frame * animation_width,
               frame2_y = 0,
               target_x = (i % target_columns_count) * animation_width,
               target_y = (i / target_columns_count) * animation_height;

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
    }
    else
    {
      for (size_t i = 0, base_frame = source_animation.Width () / animation_width - 1; i < target_frames_count; i++)
      {
        size_t second_frame = base_frame + 1,
               third_frame  = base_frame + 2;

        if (second_frame == source_animation.Width () / animation_width)
        {
          second_frame = 0;
          third_frame  = 1;
        }

        if (third_frame == source_animation.Width () / animation_width)
          third_frame = 0;

        size_t frame1_x = base_frame * animation_width,
               frame1_y = 0,
               frame2_x = second_frame * animation_width,
               frame2_y = 0,
               frame3_x = third_frame * animation_width,
               frame3_y = 0,
               target_x = (i % target_columns_count) * animation_width,
               target_y = (i / target_columns_count) * animation_height;

        float frame2_weight = (1.f - (i % (new_frames_between_two_frames + 1)) / (float)(new_frames_between_two_frames + 1)) * 0.2f + 0.4f,
              frame1_weight = (1.f - (i % (new_frames_between_two_frames + 1)) / (float)(new_frames_between_two_frames + 1)) * 0.4f,
              frame3_weight = 1.f - frame1_weight - frame2_weight;

        for (int j = 0; j < animation_height; j++)
        {
          unsigned char *frame1_data = (unsigned char*)source_animation.Bitmap () + source_animation.Width () * (frame1_y + j) * image_bpp + frame1_x * image_bpp,
                        *frame2_data = (unsigned char*)source_animation.Bitmap () + source_animation.Width () * (frame2_y + j) * image_bpp + frame2_x * image_bpp,
                        *frame3_data = (unsigned char*)source_animation.Bitmap () + source_animation.Width () * (frame3_y + j) * image_bpp + frame3_x * image_bpp,
                        *target_data = (unsigned char*)target_animation.Bitmap () + target_animation.Width () * (target_y + j) * image_bpp + target_x * image_bpp;

          for (size_t k = 0; k < animation_width * image_bpp; k++, frame1_data++, frame2_data++, frame3_data++, target_data++)
            *target_data = (unsigned char)(*frame1_data * frame1_weight + *frame2_data * frame2_weight + *frame3_data * frame3_weight);
        }

        if (!((i + 1) % (new_frames_between_two_frames + 1)))
          base_frame++;
      }
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
