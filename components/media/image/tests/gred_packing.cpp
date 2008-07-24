#include <cstdio>
#include <cmath>
#include <ctime>

#include <stl/algorithm>
#include <stl/list>
#include <stl/set>
#include <stl/vector>

#include <bv/axis_aligned_box.h>

#include <media/image.h>

using namespace media;
using namespace stl;
using namespace math;
using namespace bound_volumes;

const size_t MAX_IMAGE_BORDER = 100;

namespace
{

#pragma pack (1)

struct ImagePixel
{
  unsigned char color [3];
};

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k) 
{
  if (!k)
    return 1;

  k--;

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

size_t GetBpp (PixelFormat pf)
{
  switch (pf)
  {
    case PixelFormat_RGB8:   return 3;
    case PixelFormat_RGB16:  return 4;
    case PixelFormat_BGR8:   return 3;
    case PixelFormat_RGBA8:  return 4;
    case PixelFormat_RGBA16: return 8;
    case PixelFormat_BGRA8:  return 4;
    case PixelFormat_L8:     return 1;
    case PixelFormat_A8:     return 1;
    case PixelFormat_LA8:    return 2;
    default: printf ("Unknown pixel format\n"); return 0;  
  }
}

template <class T>
inline bool my_intersects (const axis_aligned_box<T>& box1, const axis_aligned_box<T>& box2)
{
  for (size_t i=0; i<3; i++)
    if (box1.maximum ()[i] < box2.minimum ()[i] || box1.minimum ()[i] > box2.maximum ()[i])
      return false;

  return true;  
}

}

float get_usage (const Image& image)
{
  size_t            non_zero_pixels = 0,
                    total_pixels    = image.Width () * image.Height ();
  const ImagePixel* pixel           = reinterpret_cast<const ImagePixel*> (image.Bitmap ());
  
  for (size_t i=total_pixels; i--; pixel++)
    if (pixel->color[0] || pixel->color[1] || pixel->color[2])
      non_zero_pixels++;

  printf ("Nonzero pixels count is %u\n", non_zero_pixels);
      
  return float (non_zero_pixels) / total_pixels;
}

class TileImageBuilder
{
  private:
    typedef stl::list<Image> ImageList;

    struct ImageEntry
    {
      size_t              width;
      size_t              height;
      ImageList::iterator image;

      ImageEntry (size_t in_width, size_t in_height, const ImageList::iterator& in_image)
        : width (in_width), height (in_height), image (in_image)
        {}
    };

    struct ImagePosition
    {
      size_t x_pos;
      size_t y_pos;
    };

    typedef vector<ImageEntry>    ImageEntriesArray;
    typedef vector<ImagePosition> ImagePositionsArray;
    typedef vector<size_t>        IndexArray;

  public:
    void AddImage (const Image& image)
    {
      images.push_back (image);
    }

    void BuildTileImage (const char* image_name, const char* xml_name, bool turbo)
    {
      size_t minimum_area = 0;
      size_t result_image_horisontal_side, result_image_vertical_side;

      ImageEntriesArray   image_entries;
      ImagePositionsArray image_positions;
      IndexArray          indices;

      //initialization

      image_entries.reserve  (images.size ());
      image_positions.resize (images.size ());
      indices.resize         (images.size ());

      for (ImageList::iterator iter = images.begin (), end = images.end (); iter != end; ++iter)
      {
         image_entries.push_back (ImageEntry (iter->Width (), iter->Height (), iter));
      
         minimum_area += iter->Width () * iter->Height ();
      }

      result_image_horisontal_side = result_image_vertical_side = get_next_higher_power_of_two ((size_t)sqrt ((float)minimum_area)) / 2;
      
      //Building map

      bool pack_result = false;

      clock_t start_time = clock ();

      size_t iterations_count = 0;

      while (!pack_result)
      {
        if (result_image_horisontal_side > result_image_vertical_side) result_image_vertical_side   *= 2;
        else                                                           result_image_horisontal_side *= 2;

        for (size_t i = 0; i < indices.size (); i++)
          indices[i] = i;

        if (turbo)
        {
          size_t try_count = 0;

          do
          {
            iterations_count++;
            pack_result = PackImages (result_image_horisontal_side, result_image_vertical_side, image_entries, image_positions, indices);

            if (pack_result) break;

            random_shuffle (indices.begin (), indices.end ());
          }
          while (try_count++ < 10);
        }
        else
        {
          do
          {
            iterations_count++;
            pack_result = PackImages (result_image_horisontal_side, result_image_vertical_side, image_entries, image_positions, indices);

            if (pack_result) break;
          }
          while (next_permutation (indices.begin (), indices.end ()));
        }
      }

      printf ("Iterations count is %u\n", iterations_count);
      printf ("Ellapsed time is %f\n", (float)(clock () - start_time) / CLOCKS_PER_SEC);
      printf ("Image size %ux%u\n", result_image_horisontal_side, result_image_vertical_side);

      //constructing final image

      Image result_image (result_image_horisontal_side, result_image_vertical_side, 1, images.begin ()->Format ());

      memset (result_image.Bitmap (), 0, result_image_horisontal_side * result_image_vertical_side * GetBpp (result_image.Format ()));

      size_t top_side = 0;
      size_t right_side = 0;

      for (size_t i = 0; i < image_positions.size (); i++)
      {
        if ((image_positions[i].x_pos + image_entries[indices[i]].width) > right_side)
          right_side = image_positions[i].x_pos + image_entries[indices[i]].width;
        if ((image_positions[i].y_pos + image_entries[indices[i]].height) > top_side)
          top_side = image_positions[i].y_pos + image_entries[indices[i]].height;

        result_image.PutImage (image_positions[i].x_pos, image_positions[i].y_pos, 0, image_entries[indices[i]].width, image_entries[indices[i]].height, 1, image_entries[indices[i]].image->Format (), image_entries[indices[i]].image->Bitmap ());
      }

      result_image.Save (image_name);
 
      printf ("Usage = %f\n", get_usage (result_image));
      printf ("Right side = %u\n", right_side);
      printf ("Top side = %u\n", top_side);
      printf ("Cutted size = %u\n", right_side * top_side);
    }

  private:
    bool PackImages (size_t result_image_horisontal_side, size_t result_image_vertical_side, const ImageEntriesArray& image_entries, ImagePositionsArray& image_positions, const IndexArray& indices)
    {
      struct FreeSpace
      {
        size_t x_pos;
        size_t y_pos;
        size_t width;
        size_t height;

        FreeSpace (size_t in_x_pos, size_t in_y_pos, size_t in_width, size_t in_height)
          : x_pos (in_x_pos), y_pos (in_y_pos), width (in_width), height (in_height)
        {
//          printf (">>>New free space %u.%u, %u.%u\n", x_pos, y_pos, width, height);
        }

        bool operator < (const FreeSpace& right) const
        {
          if      (x_pos < right.x_pos)   return true;
          else if (x_pos > right.x_pos)   return false;
          else if (y_pos < right.y_pos)   return true;
          else if (y_pos > right.y_pos)   return false;
          else if (width < right.width)   return true;
          else if (width > right.width)   return false;
          else if (height < right.height) return true;
          else return false;
        }
      };

      typedef set<FreeSpace, less<FreeSpace>> FreeSpacesSet;

      FreeSpacesSet free_spaces;

      free_spaces.insert (FreeSpace (0, 0, result_image_horisontal_side, result_image_vertical_side));

      for (size_t i = 0; i < indices.size (); i++)
      {
/*        printf ("\n\n\n");
        for (FreeSpacesSet::iterator iter = free_spaces.begin (), end = free_spaces.end (); iter != end; ++iter)
        {
          printf ("FreeSpace: pos=%u.%u size=%ux%u\n", iter->x_pos, iter->y_pos, iter->width, iter->height);
        }
        printf ("\n\n\n");*/

        FreeSpacesSet::iterator iter = free_spaces.begin (), end = free_spaces.end ();


//        printf ("putting image %u\n", i);

        for (; iter != end; ++iter)
        {
          if ((iter->width < image_entries[indices[i]].width) || (iter->height < image_entries[indices[i]].height))
            continue;

//          printf ("<<<putting image %u.%u to %u.%u\n", image_entries[indices[i]].width, image_entries[indices[i]].height, iter->x_pos, iter->y_pos);

          image_positions[i].x_pos = iter->x_pos;
          image_positions[i].y_pos = iter->y_pos;

          bound_volumes::axis_aligned_box<size_t> image_bb (vec3ui (image_positions[i].x_pos, image_positions[i].y_pos, 0), 
                                                            vec3ui (image_positions[i].x_pos + image_entries[indices[i]].width - 1, image_positions[i].y_pos + image_entries[indices[i]].height - 1, 1));

//          printf ("Image bb is %u.%ux%u.%u\n", image_positions[i].x_pos, image_positions[i].y_pos, image_positions[i].x_pos + image_entries[indices[i]].width - 1, image_positions[i].y_pos + image_entries[indices[i]].height - 1);

          for (FreeSpacesSet::iterator erase_iter = free_spaces.begin (), erase_end = free_spaces.end (); erase_iter != erase_end;)
          {
            bound_volumes::axis_aligned_box<size_t> free_space_bb (vec3ui (erase_iter->x_pos, erase_iter->y_pos, 0), 
                                                                   vec3ui (erase_iter->x_pos + erase_iter->width - 1, erase_iter->y_pos + erase_iter->height - 1, 1));

//            printf ("free_space bb is %u.%ux%u.%u\n", erase_iter->x_pos, erase_iter->y_pos, erase_iter->x_pos + erase_iter->width - 1, erase_iter->y_pos + iter->height - 1);

            if (my_intersects (free_space_bb, image_bb))
            {
//              printf ("Intersected\n");
              if (erase_iter->x_pos >= image_positions[i].x_pos)
              {
                if (image_positions[i].y_pos <= erase_iter->y_pos)
                {
                  if (image_entries[indices[i]].height < erase_iter->height)
                  {
                    FreeSpace top_free_space (erase_iter->x_pos, image_positions[i].y_pos + image_entries[indices[i]].height, erase_iter->width, erase_iter->height - (image_positions[i].y_pos + image_entries[indices[i]].height - erase_iter->y_pos));

                    free_spaces.insert (top_free_space);
//                    printf ("Add new top space\n");
                  }
                }
                else if (erase_iter->x_pos < (image_positions[i].x_pos + image_entries[indices[i]].width))
                {
                  FreeSpace lower_free_space (erase_iter->x_pos, erase_iter->y_pos, erase_iter->width, image_positions[i].y_pos - erase_iter->y_pos);

                  free_spaces.insert (lower_free_space);
//                  printf ("Add new lower space, free_spaces count is %u\n", free_spaces.size ());// %u.%u\n", lower_free_space.x_pos, lower_free_space.y_pos);
                }

                if ((erase_iter->x_pos + erase_iter->width) > (image_positions[i].x_pos + image_entries[indices[i]].width))
                {
                  FreeSpace right_free_space (image_positions[i].x_pos + image_entries[indices[i]].width, erase_iter->y_pos, erase_iter->width - (image_positions[i].x_pos + image_entries[indices[i]].width - erase_iter->x_pos), erase_iter->height);

                  free_spaces.insert (right_free_space);

//                  printf ("Add new right space, free_spaces count is %u\n", free_spaces.size ());// %u.%u\n", top_free_space.x_pos, top_free_space.y_pos);
                }
              }
              if (erase_iter->y_pos >= image_positions[i].y_pos)
              {
                if (image_positions[i].x_pos <= erase_iter->x_pos)
                {
                  if (image_entries[indices[i]].width < erase_iter->width)
                  {
                    FreeSpace right_free_space (image_positions[i].x_pos + image_entries[indices[i]].width, erase_iter->y_pos, erase_iter->width - (image_positions[i].x_pos + image_entries[indices[i]].width - erase_iter->x_pos), erase_iter->height);

                    free_spaces.insert (right_free_space);
//                    printf ("Add new right space\n");
                  }
                }
                else if (erase_iter->y_pos < (image_positions[i].y_pos + image_entries[indices[i]].height))
                {
                  FreeSpace narrow_free_space (erase_iter->x_pos, erase_iter->y_pos, image_positions[i].x_pos - erase_iter->x_pos, erase_iter->height);
                  
                  free_spaces.insert (narrow_free_space);

//                  printf ("Add new narrow space, free_spaces count is %u\n", free_spaces.size ());// %u.%u\n", narrow_free_space.x_pos, narrow_free_space.y_pos);
                }

                if ((erase_iter->y_pos + erase_iter->height) > (image_positions[i].y_pos + image_entries[indices[i]].height))
                {
                  FreeSpace top_free_space (erase_iter->x_pos, image_positions[i].y_pos + image_entries[indices[i]].height, erase_iter->width, erase_iter->height - (image_positions[i].y_pos + image_entries[indices[i]].height - erase_iter->y_pos));

                  free_spaces.insert (top_free_space);

//                  printf ("Add new top space, free_spaces count is %u\n", free_spaces.size ());// %u.%u\n", top_free_space.x_pos, top_free_space.y_pos);
                }
              }

              FreeSpacesSet::iterator next = erase_iter;
              ++next;

              free_spaces.erase (erase_iter);

              erase_iter = next;
            }
            else
              ++erase_iter;
          }

          break;
        }

        if (iter == end)
          return false;
      }

      return true;
    }

  private:
    ImageList images;
};

int main (int argc, char *argv[])
{
  if (argc != 5)
  {
//    printf ("Usage: tilemap_builder result_image_name tilemap_name turbo[off][on] imagename [imagename]\n");
    printf ("Usage: tilemap_builder result_image_name tilemap_name turbo[off][on] image_count\n");
    return 1;
  }

  try
  {
    TileImageBuilder tile_image_builder;

/*    for (size_t i = 0; i < (size_t)(argc - 4); i++)
    {
      Image image (argv[i + 4]);

      tile_image_builder.AddImage (image);
    }*/

    size_t image_count = atoi (argv[4]);

    size_t pixels_count = 0;

    srand (0);

    for (size_t i = 0; i < image_count; i++)
    {
//      Image image (rand () % MAX_IMAGE_BORDER + 1, rand () % MAX_IMAGE_BORDER + 1, 1, PixelFormat_RGB8);
      Image image (10, (size_t)((cos ((float)i) + 1.f) * MAX_IMAGE_BORDER) + 1, 1, PixelFormat_RGB8);

/*      ImagePixel* current_pixel = (ImagePixel*)image.Bitmap ();
      ImagePixel white_color = {255, 255, 255};
      
      for (size_t j = 0; j < image.Width () * image.Height (); j++, current_pixel++)
        *current_pixel = white_color;

      ImagePixel image_color = {0, 0, 0};
      
      image_color.color[i%3] = (unsigned char)(rand () % 128 + 127);

      current_pixel = (ImagePixel*)image.Bitmap () + image.Width () + 1;

      for (size_t j = 1; j < image.Height () - 1; j++, current_pixel += 2)
        for (size_t k = 1; k < image.Width () - 1; k++, current_pixel++)
          *current_pixel = image_color;
*/

      ImagePixel* current_pixel = (ImagePixel*)image.Bitmap ();
      ImagePixel image_color = {0, 0, 0};
      
      image_color.color[i%3] = (unsigned char)(rand () % 128 + 127);

      for (size_t j = 0; j < image.Width () * image.Height (); j++, current_pixel++)
        *current_pixel = image_color;

      tile_image_builder.AddImage (image);

      pixels_count += image.Width () * image.Height ();
    }

    printf ("Total pixels count is %u\n", pixels_count);

    tile_image_builder.BuildTileImage (argv[1], argv[2], !strcmp (argv[3], "turboon"));
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
    return 1;
  }                                               
  catch (...)
  {                                               
    printf ("exception..."); 
    return 1;
  }                                               

  return 0;
}
