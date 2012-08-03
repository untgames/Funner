#include <cstdio>

#include <stl/algorithm>
#include <stl/set>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>

#include <bv/axis_aligned_box.h>

#include <media/atlas_builder.h>

using namespace media;
using namespace stl;
using namespace math;
using namespace bound_volumes;

namespace components
{

namespace left_bottom_packer
{

static size_t intersects_count = 0;
const size_t PACK_TRY_COUNT = 100;

struct MyRand
{
  size_t next;

  MyRand () : next (0) {}

  size_t operator () (int max_rand)
  {
    next = next * 1103515245 + 12345;

    return (size_t)(next / 65536) % max_rand;
  }
};

bool largest_area (size_t first, size_t second, math::vec2ui* images)
{
  return images [first].x * images [first].y > images [second].x * images [second].y;
}

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
    typedef stl::vector<size_t> IndexArray;

  public:
    void AddImages (size_t images_count, const math::vec2ui* in_sizes)
    {
      images.resize (images_count);

      average_image_width  = 0;
      average_image_height = 0;

      for (size_t i = 0; i < images_count; i++)
      {
        average_image_width  += in_sizes[i].x;
        average_image_height += in_sizes[i].y;
        images[i] = in_sizes[i];
      }

      average_image_width  /= images_count;
      average_image_height /= images_count;
    }

    void BuildTileImage (math::vec2ui* out_origins, bool* out_was_packed, size_t margin, size_t max_image_size, size_t pack_flags)
    {
      if (!pack_flags & AtlasPackFlag_PackToMaxImageSize)
        max_image_size = 0;

      IndexArray indices;

      //initialization

      indices.resize (images.size ());

      size_t minimum_area = 0;
      size_t result_image_horisontal_side, result_image_vertical_side;

      for (ImagesArray::iterator iter = images.begin (), end = images.end (); iter != end; ++iter)
         minimum_area += iter->x * iter->y;

      if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
        result_image_horisontal_side = result_image_vertical_side = stl::max ((size_t)1, get_next_higher_power_of_two ((size_t)sqrt ((float)minimum_area)) / 2);
      else
        result_image_horisontal_side = result_image_vertical_side = (size_t)sqrt ((float)minimum_area) + 1;

      //Building map

      bool pack_result = false;

      MyRand my_rand;

      size_t pack_try_count = pack_flags & AtlasPackFlag_Fast ? 1 : PACK_TRY_COUNT;

        //массивы для сохранения результатов разных попыток упаковки (для выбора наилучшей)
      xtl::uninitialized_storage<math::vec2ui> origins    (images.size () * pack_try_count);
      xtl::uninitialized_storage<bool>         was_packed (images.size () * pack_try_count);

      while (!pack_result)
      {
        if (result_image_horisontal_side > result_image_vertical_side) result_image_vertical_side   *= 2;
        else                                                           result_image_horisontal_side *= 2;

        if (pack_flags & AtlasPackFlag_PackToMaxImageSize)
        {
          if (result_image_horisontal_side > max_image_size)
            result_image_horisontal_side = max_image_size;
          if (result_image_vertical_side > max_image_size)
            result_image_vertical_side = max_image_size;
        }

        for (size_t i = 0; i < indices.size (); i++)
          indices[i] = i;

        bool pack_to_max_image_size = max_image_size == result_image_horisontal_side && max_image_size == result_image_vertical_side;

        size_t try_count     = 0,
               max_used_area = 0,
               best_try      = 0;

        do
        {
          if (!try_count)
            stl::sort (indices.begin (), indices.end (), xtl::bind (largest_area, _1, _2, images.begin ()));

          math::vec2ui* current_out_origins    = pack_to_max_image_size ? origins.data () + images.size () * try_count : out_origins;
          bool*         current_out_was_packed = pack_to_max_image_size ? was_packed.data () + images.size () * try_count : out_was_packed;

          size_t used_area;

          pack_result = PackImages (margin, (pack_flags & AtlasPackFlag_SwapAxises) != 0, (pack_flags & AtlasPackFlag_Fast) != 0,
                                    result_image_horisontal_side, result_image_vertical_side, indices, pack_to_max_image_size,
                                    current_out_origins, current_out_was_packed, used_area);

          if (used_area > max_used_area)
          {
            max_used_area = used_area;
            best_try      = try_count;
          }

          if (pack_result && !pack_to_max_image_size)
            break;

          random_shuffle (indices.begin (), indices.end (), my_rand);

          try_count++;
        }
        while (try_count < pack_try_count);

        if (pack_to_max_image_size)
        {
          memcpy (out_origins, origins.data () + images.size () * best_try, sizeof (*out_origins) * images.size ());
          memcpy (out_was_packed, was_packed.data () + images.size () * best_try, sizeof (*out_was_packed) * images.size ());
        }
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

    typedef set<FreeSpace, less<FreeSpace> > FreeSpacesSet;

    void AddFreeSpace (const FreeSpace& free_space, FreeSpacesSet& free_spaces, bool fast)
    {
      if (fast && (free_space.width < average_image_width || free_space.height < average_image_height))
        return;

      free_spaces.insert (free_space);
    }

    bool PackImages (size_t margin, bool swap_axises, bool fast, size_t result_image_horizontal_side,
                     size_t result_image_vertical_side, const IndexArray& indices, bool pack_to_max_image_size,
                     math::vec2ui* out_origins, bool* out_was_packed, size_t& used_area)
    {
      FreeSpacesSet free_spaces;

      free_spaces.insert (FreeSpace (0, 0, result_image_horizontal_side, result_image_vertical_side, swap_axises));

      used_area = 0;

      for (size_t i = 0, count = indices.size (); i < count; i++)
      {
        math::vec2ui &image_size = images [indices [i]],
                     &out_origin = out_origins [indices [i]];

        FreeSpacesSet::iterator iter = free_spaces.begin (), end = free_spaces.end ();

        for (; iter != end; ++iter)
        {
          if ((iter->width < image_size.x) || (iter->height < image_size.y))
            continue;

          //проверяем можно ли вставить картинку с отступом в это место
          if ((iter->x_pos + iter->width != result_image_horizontal_side) && (iter->width < image_size.x + margin))
            continue;

          if ((iter->y_pos + iter->height != result_image_vertical_side) && (iter->height < image_size.y + margin))
            continue;

          math::vec2ui margined_image_size (stl::min (image_size.x + margin, iter->width), stl::min (image_size.y + margin, iter->height));

          out_origin.x = iter->x_pos;
          out_origin.y = iter->y_pos;

          //нашли подходящее место для картинки, для всех свободных мест с которыми пересеклась картинка проводим рассчёт новых свободных мест
          bound_volumes::axis_aligned_box<unsigned int> image_bb (vec3ui (out_origin.x, out_origin.y, 0),
                                                                  vec3ui (out_origin.x + margined_image_size.x - 1, out_origin.y + margined_image_size.y - 1, 1));

          for (FreeSpacesSet::iterator erase_iter = free_spaces.begin (), erase_end = free_spaces.end (); erase_iter != erase_end;)
          {
            bound_volumes::axis_aligned_box<unsigned int> free_space_bb (vec3ui (erase_iter->x_pos, erase_iter->y_pos, 0),
                                                                         vec3ui (erase_iter->x_pos + erase_iter->width - 1, erase_iter->y_pos + erase_iter->height - 1, 1));

            intersects_count++;

            if (my_intersects (free_space_bb, image_bb)) //картинка пересеклась с этим свободным местом, удаляем его и добавляем образовавшиеся новые свободные места
            {
              if (out_origin.x > erase_iter->x_pos) //есть новое свободное место левее картинки
              {
                FreeSpace left_free_space (erase_iter->x_pos, erase_iter->y_pos, out_origin.x - erase_iter->x_pos, erase_iter->height, swap_axises);

                AddFreeSpace (left_free_space, free_spaces, fast);
              }
              if ((out_origin.x + margined_image_size.x) < (erase_iter->x_pos + erase_iter->width)) //есть новое свободное место правее картинки
              {
                FreeSpace right_free_space (out_origin.x + margined_image_size.x, erase_iter->y_pos, erase_iter->x_pos + erase_iter->width - (out_origin.x + margined_image_size.x), erase_iter->height, swap_axises);

                AddFreeSpace (right_free_space, free_spaces, fast);
              }
              if (out_origin.y > erase_iter->y_pos) //есть новое свободное место ниже картинки
              {
                FreeSpace bottom_free_space (erase_iter->x_pos, erase_iter->y_pos, erase_iter->width, out_origin.y - erase_iter->y_pos, swap_axises);

                AddFreeSpace (bottom_free_space, free_spaces, fast);
              }
              if ((out_origin.y + margined_image_size.y) < (erase_iter->y_pos + erase_iter->height)) //есть новое свободное место выше картинки
              {
                FreeSpace top_free_space (erase_iter->x_pos, out_origin.y + margined_image_size.y, erase_iter->width, erase_iter->y_pos + erase_iter->height - (out_origin.y + margined_image_size.y), swap_axises);

                AddFreeSpace (top_free_space, free_spaces, fast);
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

        bool packed = iter != end;

        if (packed)
          used_area += image_size.x * image_size.y;

        out_was_packed [indices [i]] = packed;

        if (!packed && !pack_to_max_image_size)
          return false;
      }

      return true;
    }

  private:
    ImagesArray images;
    size_t      average_image_width;
    size_t      average_image_height;
};

void left_bottom_pack (const AtlasBuilder::PackHandlerParams& params)
{
  static const char* METHOD_NAME = "media::LeftBottomPacker";

  if (!params.in_sizes)
    throw xtl::make_null_argument_exception (METHOD_NAME, "in_sizes");

  if (!params.out_origins)
    throw xtl::make_null_argument_exception (METHOD_NAME, "out_origins");

  TileImageBuilder tile_image_builder;

  tile_image_builder.AddImages (params.images_count, params.in_sizes);

  if (params.pack_flags & AtlasPackFlag_PackToMaxImageSize)
  {
    if (!params.max_image_size)
      throw xtl::make_null_argument_exception (METHOD_NAME, "max_image_size");

    if (!params.out_was_packed)
      throw xtl::make_null_argument_exception (METHOD_NAME, "out_was_packed");
  }

  tile_image_builder.BuildTileImage (params.out_origins, params.out_was_packed, params.margin, params.max_image_size, params.pack_flags);
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
      AtlasBuilderManager::PackHandler pack_handler = xtl::bind (&left_bottom_pack, _1);

      AtlasBuilderManager::RegisterPacker ("default", pack_handler);
      AtlasBuilderManager::RegisterPacker ("left_bottom", pack_handler);
    }
};

extern "C"
{

common::ComponentRegistrator<LeftBottomPackerComponent> LeftBottomPacker ("media.image.atlas_builder.packers.left_bottom");

}

}

}
