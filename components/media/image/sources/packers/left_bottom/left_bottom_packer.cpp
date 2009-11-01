#include <cstdio>

#include <stl/algorithm>
#include <stl/set>
#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/component.h>

#include <bv/axis_aligned_box.h>

#include <media/atlas_builder.h>

using namespace media;
using namespace stl;
using namespace math;
using namespace bound_volumes;

namespace
{

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k)
{
  if (!k)
    return 1;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

template <class T>
inline bool my_intersects (const axis_aligned_box<T>& box1, const axis_aligned_box<T>& box2)
{
  for (size_t i=0; i<3; i++)
    if (box1.maximum ()[i] < box2.minimum ()[i] || box1.minimum ()[i] > box2.maximum ()[i])
      return false;

  return true;
}

class TileImageBuilder
{
  private:
    typedef stl::vector<vec2ui> ImagesArray;
    typedef vector<size_t>      IndexArray;

  public:
    void AddImages (size_t images_count, const math::vec2ui* in_sizes)
    {
      images.resize (images_count);

      for (size_t i = 0; i < images_count; i++)
        images[i] = in_sizes[i];
    }

    void BuildTileImage (math::vec2ui* out_origins, size_t pack_flags)
    {
      size_t minimum_area = 0;
      size_t result_image_horisontal_side, result_image_vertical_side;

      IndexArray indices;

      //initialization

      indices.resize (images.size ());

      for (ImagesArray::iterator iter = images.begin (), end = images.end (); iter != end; ++iter)
         minimum_area += iter->x * iter->y;

      if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
        result_image_horisontal_side = result_image_vertical_side = get_next_higher_power_of_two ((size_t)sqrt ((float)minimum_area)) / 2;
      else
        result_image_horisontal_side = result_image_vertical_side = (size_t)sqrt ((float)minimum_area) + 1;

      //Building map

      bool pack_result = false;

      while (!pack_result)
      {
        if (result_image_horisontal_side > result_image_vertical_side) result_image_vertical_side   *= 2;
        else                                                           result_image_horisontal_side *= 2;

        for (size_t i = 0; i < indices.size (); i++)
          indices[i] = i;

        size_t try_count = 0;

        do
        {
          pack_result = PackImages (pack_flags & AtlasPackFlag_SwapAxises, result_image_horisontal_side, result_image_vertical_side, out_origins, indices);

          if (pack_result) break;

          random_shuffle (indices.begin (), indices.end ());
        }
        while (try_count++ < 10);
      }
    }

  private:
    struct FreeSpace
    {
      size_t x_pos;
      size_t y_pos;
      size_t width;
      size_t height;
      bool   swap_axises;

      FreeSpace (size_t in_x_pos, size_t in_y_pos, size_t in_width, size_t in_height, bool in_swap_axises)
        : x_pos (in_x_pos), y_pos (in_y_pos), width (in_width), height (in_height), swap_axises (in_swap_axises)
        {}

      bool operator < (const FreeSpace& right) const
      {
        if (swap_axises)
        {
          if      (y_pos < right.y_pos)   return true;
          else if (y_pos > right.y_pos)   return false;
          else if (x_pos < right.x_pos)   return true;
          else if (x_pos > right.x_pos)   return false;
          else if (height < right.height) return true;
          else if (height > right.height) return false;
          else if (width < right.width)   return true;
          else return false;
        }
        else
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
      }
    };

    bool PackImages (bool swap_axises, size_t result_image_horizontal_side, size_t result_image_vertical_side, math::vec2ui* out_origins, const IndexArray& indices)
    {
      typedef set<FreeSpace, less<FreeSpace> > FreeSpacesSet;

      FreeSpacesSet free_spaces;

      free_spaces.insert (FreeSpace (0, 0, result_image_horizontal_side, result_image_vertical_side, swap_axises));

      for (size_t i = 0; i < indices.size (); i++)
      {
        math::vec2ui &image_size = images [indices [i]],
                     &out_origin = out_origins [indices [i]];

        FreeSpacesSet::iterator iter = free_spaces.begin (), end = free_spaces.end ();

        for (; iter != end; ++iter)
        {
          if ((iter->width < image_size.x) || (iter->height < image_size.y))
            continue;

          out_origin.x = iter->x_pos;
          out_origin.y = iter->y_pos;

          //нашли подходящее место для картинки, для всех свободных мест с которыми пересеклась картинка проводим рассчёт новых свободных мест
          bound_volumes::axis_aligned_box<unsigned int> image_bb (vec3ui (out_origin.x, out_origin.y, 0),
                                                                  vec3ui (out_origin.x + image_size.x - 1, out_origin.y + image_size.y - 1, 1));

          for (FreeSpacesSet::iterator erase_iter = free_spaces.begin (), erase_end = free_spaces.end (); erase_iter != erase_end;)
          {
            bound_volumes::axis_aligned_box<unsigned int> free_space_bb (vec3ui (erase_iter->x_pos, erase_iter->y_pos, 0),
                                                                         vec3ui (erase_iter->x_pos + erase_iter->width - 1, erase_iter->y_pos + erase_iter->height - 1, 1));

            if (my_intersects (free_space_bb, image_bb)) //картинка пересеклась с этим свободным местом, удаляем его и добавляем образовавшиеся новые свободные места
            {
              if (out_origin.x > erase_iter->x_pos) //есть новое свободное место левее картинки
              {
                FreeSpace left_free_space (erase_iter->x_pos, erase_iter->y_pos, out_origin.x - erase_iter->x_pos, erase_iter->height, swap_axises);

                free_spaces.insert (left_free_space);
              }
              if ((out_origin.x + image_size.x) < (erase_iter->x_pos + erase_iter->width)) //есть новое свободное место правее картинки
              {
                FreeSpace right_free_space (out_origin.x + image_size.x, erase_iter->y_pos, erase_iter->x_pos + erase_iter->width - (out_origin.x + image_size.x), erase_iter->height, swap_axises);

                free_spaces.insert (right_free_space);
              }
              if (out_origin.y > erase_iter->y_pos) //есть новое свободное место ниже картинки
              {
                FreeSpace bottom_free_space (erase_iter->x_pos, erase_iter->y_pos, erase_iter->width, out_origin.y - erase_iter->y_pos, swap_axises);

                free_spaces.insert (bottom_free_space);
              }
              if ((out_origin.y + image_size.y) < (erase_iter->y_pos + erase_iter->height)) //есть новое свободное место выше картинки
              {
                FreeSpace top_free_space (erase_iter->x_pos, out_origin.y + image_size.y, erase_iter->width, erase_iter->y_pos + erase_iter->height - (out_origin.y + image_size.y), swap_axises);

                free_spaces.insert (top_free_space);
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
    ImagesArray images;
};

void left_bottom_pack (size_t images_count, const math::vec2ui* in_sizes, math::vec2ui* out_origins, size_t pack_flags)
{
  static const char* METHOD_NAME = "media::LeftBottomPacker";

  if (!in_sizes)
    throw xtl::make_null_argument_exception (METHOD_NAME, "in_sizes");

  if (!out_origins)
    throw xtl::make_null_argument_exception (METHOD_NAME, "out_origins");

  TileImageBuilder tile_image_builder;

  tile_image_builder.AddImages (images_count, in_sizes);
  tile_image_builder.BuildTileImage (out_origins, pack_flags);
}

/*
   Компонент упаковки картинок
*/

class LeftBottomPackerComponent
{
  public:
    //загрузка компонента
    LeftBottomPackerComponent ()
    {
      AtlasBuilderManager::RegisterPacker ("default", &left_bottom_pack);
      AtlasBuilderManager::RegisterPacker ("left_bottom", &left_bottom_pack);
    }
};

extern "C"
{

common::ComponentRegistrator<LeftBottomPackerComponent> LeftBottomPacker ("media.image.atlas_builder.packers.left_bottom");

}

}
