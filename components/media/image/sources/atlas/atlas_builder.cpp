#include "shared.h"

#include <common/time.h>

using namespace media;

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

media::PixelFormat max_image_format (media::PixelFormat format_1, media::PixelFormat format_2)
{
  static const char* METHOD_NAME = "media::max_image_format";

  switch (format_2)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGB16:
    case PixelFormat_BGR8:
    case PixelFormat_RGBA8:
    case PixelFormat_RGBA16:
    case PixelFormat_BGRA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format_1", format_1);
  }

  switch (format_1)
  {
    case PixelFormat_RGB8:
    case PixelFormat_BGR8:
    {
      switch (format_2)
      {
        case PixelFormat_RGBA8:
        case PixelFormat_BGRA8:
        case PixelFormat_RGBA16:
        case PixelFormat_RGB16:
          return format_2;
        case PixelFormat_A8:
        case PixelFormat_LA8:
          return PixelFormat_RGBA8;
        default:
          return format_1;
      }
    }
    case PixelFormat_RGB16:
    {
      switch (format_2)
      {
        case PixelFormat_RGBA8:
        case PixelFormat_BGRA8:
        case PixelFormat_RGBA16:
        case PixelFormat_A8:
        case PixelFormat_LA8:
          return PixelFormat_RGBA16;
        default:
          return format_1;
      }
    }
    case PixelFormat_RGBA8:
    case PixelFormat_BGRA8:
    {
      switch (format_2)
      {
        case PixelFormat_RGBA16:
        case PixelFormat_RGB16:
          return PixelFormat_RGBA16;
        default:
          return format_1;
      }
    }
    case PixelFormat_RGBA16:
      return format_1;
    case PixelFormat_L8:
    {
      switch (format_2)
      {
        case PixelFormat_RGB8:
        case PixelFormat_RGB16:
        case PixelFormat_BGR8:
        case PixelFormat_RGBA8:
        case PixelFormat_RGBA16:
        case PixelFormat_BGRA8:
        case PixelFormat_LA8:
          return format_2;
        case PixelFormat_A8:
          return PixelFormat_LA8;
        default:
          return format_1;
      }
    }
    case PixelFormat_A8:
    {
      switch (format_2)
      {
        case PixelFormat_RGB8:
        case PixelFormat_BGR8:
        case PixelFormat_RGBA8:
        case PixelFormat_BGRA8:
          return PixelFormat_RGBA8;
        case PixelFormat_RGB16:
        case PixelFormat_RGBA16:
          return PixelFormat_RGBA16;
        case PixelFormat_LA8:
        case PixelFormat_L8:
          return PixelFormat_LA8;
        default:
          return format_1;
      }
    }
    case PixelFormat_LA8:
    {
      switch (format_2)
      {
        case PixelFormat_RGB8:
        case PixelFormat_BGR8:
        case PixelFormat_RGBA8:
        case PixelFormat_BGRA8:
          return PixelFormat_RGBA8;
        case PixelFormat_RGB16:
        case PixelFormat_RGBA16:
          return PixelFormat_RGBA16;
        default:
          return format_1;
      }
    }
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format_1", format_1);
  }
}

//класс предоставляющий информацию о картинке
class IImageHolder : public xtl::reference_counter
{
  public:
    virtual ~IImageHolder () {}

    //Получение картинки
    virtual media::Image Image () = 0;

    //Получение атрибутов картинки
    virtual size_t      ImageWidth  () = 0;
    virtual size_t      ImageHeight () = 0;
    virtual size_t      ImageHash   () = 0;
    virtual PixelFormat ImageFormat () = 0;
    virtual const char* ImageName   () = 0;
};

//Класс, хранящий картинку в памяти
class MemoryImageHolder : public IImageHolder
{
  public:
    MemoryImageHolder (const media::Image& in_image)
      : image (in_image)
    {
      image_hash = common::crc32 (image.Bitmap (), image.Width () * image.Height () * get_bytes_per_pixel (image.Format ()));
    }

    //Получение картинки
    media::Image Image ()
    {
      return image;
    }

    //Получение атрибутов картинки
    size_t ImageWidth ()
    {
      return image.Width ();
    }

    size_t ImageHeight ()
    {
      return image.Height ();
    }

    size_t ImageHash ()
    {
      return image_hash;
    }

    PixelFormat ImageFormat ()
    {
      return image.Format ();
    }

    const char* ImageName ()
    {
      return image.Name ();
    }

  private:
    media::Image image;
    size_t       image_hash;
};

//Класс, загружающий картинку из файла по требованию
class LoadOnDemandImageHolder : public IImageHolder
{
  public:
    LoadOnDemandImageHolder (const char* in_image_name)
      : image_name (in_image_name)
    {
      media::Image image (in_image_name);

      image_format = image.Format ();
      image_width  = image.Width  ();
      image_height = image.Height ();
      image_hash   = common::crc32 (image.Bitmap (), image.Width () * image.Height () * get_bytes_per_pixel (image.Format ()));
    }

    //Получение картинки
    media::Image Image ()
    {
      return media::Image (image_name.c_str ());
    }

    //Получение атрибутов картинки
    size_t ImageWidth ()
    {
      return image_width;
    }

    size_t ImageHeight ()
    {
      return image_height;
    }

    size_t ImageHash ()
    {
      return image_hash;
    }

    PixelFormat ImageFormat ()
    {
      return image_format;
    }

    const char* ImageName ()
    {
      return image_name.c_str ();
    }

  private:
    stl::string image_name;
    PixelFormat image_format;
    size_t      image_width;
    size_t      image_height;
    size_t      image_hash;
};

typedef xtl::intrusive_ptr<IImageHolder> ImageHolderPtr;

//Описание экземпляра картинки
struct ImageDesc : public xtl::reference_counter
{
  ImageHolderPtr image_holder;       //картинка
  size_t         duplicate_of_index; //индекс картинки в массиве images, дубликатом которой является эта картинка
};

struct PackResult : public xtl::reference_counter
{
  size_t                                   image_width;   //ширина результирующей картинки
  size_t                                   image_height;  //высота результирующей картинки
  xtl::uninitialized_storage<math::vec2ui> origins;       //позиции картинок
  xtl::uninitialized_storage<size_t>       indices;       //индексы картинок
};

typedef stl::vector<size_t>            IndexArray;
typedef xtl::intrusive_ptr<PackResult> PackResultPtr;
typedef stl::vector<PackResultPtr>     PackResultsArray;
typedef xtl::intrusive_ptr<ImageDesc>  ImageDescPtr;
typedef stl::vector<ImageDescPtr>      ImagesDescsArray;
typedef stl::hash_map<size_t, size_t>  BitmapHashMap;
typedef AtlasBuilder::PackHandler      PackHandler;

}

/*
   Реализация построителя атласа изображений
*/

struct AtlasBuilder::Impl
{
  PackHandler      pack_handler;      //обработчик упаковки
  ImagesDescsArray images;            //картинки
  BitmapHashMap    images_hash_map;   //карта хешей картинок
  PackResultsArray pack_results;      //результаты упаковки
  size_t           max_image_size;    //максимальный размер одного атласа
  size_t           margin;            //размер поля
  size_t           pack_flags;        //флаги сжатия
  bool             needs_rebuild;     //необходимо ли перестроить атлас
  
  ///Конструктор
  Impl (const PackHandler& in_pack_handler)
    : pack_handler (in_pack_handler)
    , max_image_size (-1)
    , margin (0)
    , pack_flags (0)
    , needs_rebuild (true)
    {}

  ///Добавление изображений
  void Insert (Image& image, AtlasBuilderInsertMode mode)
  {
    try
    {
      switch (mode)
      {
        case AtlasBuilderInsertMode_Copy:
          images.push_back (GetImageDesc (ImageHolderPtr (new MemoryImageHolder (image.Clone ()), false)));
          break;
        case AtlasBuilderInsertMode_Capture:
        {
          Image new_image;

          new_image.Swap (image);

          images.push_back (GetImageDesc (ImageHolderPtr (new MemoryImageHolder (new_image), false)));

          break;
        }
        case AtlasBuilderInsertMode_Reference:
        {
          images.push_back (GetImageDesc (ImageHolderPtr (new MemoryImageHolder (image))));
          break;
        }
        default:
          throw xtl::make_argument_exception ("", "mode", mode, "Unknown insert mode");
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::AtlasBuilder::Insert");
      throw;
    }
  }

  void Insert (const char* image_name, bool keep_in_memory)
  {
    try
    {
      ImageHolderPtr image_holder;

      if (keep_in_memory)
        image_holder = ImageHolderPtr (new MemoryImageHolder (Image (image_name)), false);
      else
        image_holder = ImageHolderPtr (new LoadOnDemandImageHolder (image_name), false);

      images.push_back (GetImageDesc (image_holder));
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::AtlasBuilder::Insert");
      throw;
    }
  }

  ImageDescPtr GetImageDesc (ImageHolderPtr image_holder)
  {
    ImageDescPtr image_desc (new ImageDesc);

    image_desc->image_holder = image_holder;

    size_t bitmap_hash = image_holder->ImageHash ();

    BitmapHashMap::iterator iter = images_hash_map.find (bitmap_hash);

    if (iter == images_hash_map.end ())
    {
      image_desc->duplicate_of_index = images.size ();
      images_hash_map.insert_pair (bitmap_hash, image_desc->duplicate_of_index);
    }
    else
    {
      image_desc->duplicate_of_index = iter->second;
    }

    return image_desc;
  }

  void Build ()
  {
    if (!needs_rebuild)
      return;

    pack_results.clear ();

    if (images.empty ())
      return;

    try
    {
        //построение массива индексов уникальных картинок
      IndexArray images_to_process;

      for (size_t i = 0, count = images.size (); i != count; i++)
      {
        ImageDescPtr image_desc = images [i];

        if (i == image_desc->duplicate_of_index)
          images_to_process.push_back (i);
      }

        //упаковка
      xtl::uninitialized_storage<math::vec2ui> sizes (images_to_process.size ()), origins (images_to_process.size ()), success_origins (images_to_process.size ());
      xtl::uninitialized_storage<bool>         was_packed (images_to_process.size ());
      IndexArray                               packed_indices;

      while (!images_to_process.empty ())
      {
        size_t images_count = images_to_process.size ();

        for (size_t i = 0; i < images_count; i++)
        {
          size_t image_index = images_to_process [i];

          ImageHolderPtr image_holder = images [image_index]->image_holder;

          sizes.data () [i].x = image_holder->ImageWidth ();
          sizes.data () [i].y = image_holder->ImageHeight ();
        }

        PackHandlerParams pack_params;

        memset (&pack_params, 0, sizeof (pack_params));

        pack_params.images_count   = images_count;
        pack_params.in_sizes       = sizes.data ();
        pack_params.out_origins    = origins.data ();
        pack_params.out_was_packed = was_packed.data ();
        pack_params.margin         = margin;
        pack_params.max_image_size = max_image_size;
        pack_params.pack_flags     = (pack_flags & ~AtlasPackFlag_InvertTilesX & ~AtlasPackFlag_InvertTilesY) | AtlasPackFlag_PackToMaxImageSize;

        pack_handler (pack_params);

        math::vec2ui *current_origin     = origins.data (),
                     *current_size       = sizes.data ();
        bool         *current_was_packed = was_packed.data ();

        size_t result_image_width  = 0,
               result_image_height = 0;

        packed_indices.clear   ();
        packed_indices.reserve (images_count);

        size_t top_right_edge_margin = pack_flags & AtlasPackFlag_TopRightEdgeMargin ? margin : 0;

        for (size_t i = 0; i < images_count; i++, current_origin++, current_size++, current_was_packed++)
        {
          if (!*current_was_packed)
            continue;

          packed_indices.push_back (i);

          size_t right = current_origin->x + current_size->x,
                 top   = current_origin->y + current_size->y;

          if (current_origin->x)
            right += top_right_edge_margin;

          if (current_origin->y)
            top += top_right_edge_margin;

          if (result_image_width < right) result_image_width  = right;
          if (result_image_height < top)  result_image_height = top;
        }

        if (packed_indices.empty ())
          throw xtl::format_operation_exception ("", "Can't pack images, image '%s' greater then maximum image size %u",
                                                 images [images_to_process.front ()]->image_holder->ImageName (), max_image_size);

        if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
        {
          result_image_width  = get_next_higher_power_of_two (result_image_width);
          result_image_height = get_next_higher_power_of_two (result_image_height);
        }

        if (pack_flags & AtlasPackFlag_SquareAxises)
        {
          result_image_width = result_image_height = stl::max (result_image_width, result_image_height);
        }

          //подсчет количества упакованных неуникальных картинок
        size_t packed_images_count = 0;

        for (size_t i = 0, images_count = images.size (); i < images_count; i++)
        {
          size_t duplicate_of_index = images [i]->duplicate_of_index;

          for (IndexArray::iterator iter = packed_indices.begin (), end = packed_indices.end (); iter != end; ++iter)
          {
            size_t image_index = images_to_process [*iter];

            if (image_index == duplicate_of_index)
            {
              packed_images_count++;
              break;
            }
          }
        }

          //формирование результата упаковки
        PackResultPtr pack_result (new PackResult, false);

        pack_result->image_width  = result_image_width;
        pack_result->image_height = result_image_height;
        pack_result->origins.resize (packed_images_count);
        pack_result->indices.resize (packed_images_count);

        current_origin = pack_result->origins.data ();

        size_t* current_index = pack_result->indices.data ();

        for (size_t i = 0, images_count = images.size (); i < images_count; i++)
        {
          size_t duplicate_of_index = images [i]->duplicate_of_index;

          for (IndexArray::iterator iter = packed_indices.begin (), end = packed_indices.end (); iter != end; ++iter)
          {
            size_t image_index = images_to_process [*iter];

            if (image_index == duplicate_of_index)
            {
              *current_origin = origins.data () [*iter];
              *current_index  = i;

              current_origin++;
              current_index++;

              break;
            }
          }
        }

        pack_results.push_back (pack_result);

        for (IndexArray::reverse_iterator iter = packed_indices.rbegin (), end = packed_indices.rend (); iter != end; ++iter)
          images_to_process.erase (images_to_process.begin () + *iter);
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::AtlasBuilder::Build");
      throw;
    }

    needs_rebuild = false;
  }

  void BuildAtlas (size_t index, const char* atlas_image_name, Atlas& out_atlas)
  {
    Atlas result;

    PackResult& pack_result = *pack_results [index];

    size_t*       current_index  = pack_result.indices.data ();
    math::vec2ui* current_origin = pack_result.origins.data ();

    for (size_t i = 0, count = pack_result.indices.size (); i < count; i++, current_index++, current_origin++)
    {
      ImageDescPtr   current_image_desc = images [*current_index];
      ImageHolderPtr image_holder       = current_image_desc->image_holder;
      Tile           new_tile;

      math::vec2ui origin = *current_origin,
                   size (image_holder->ImageWidth (), image_holder->ImageHeight ());

      if (pack_flags & AtlasPackFlag_InvertTilesX) origin.x = pack_result.image_width  - origin.x - size.x;
      if (pack_flags & AtlasPackFlag_InvertTilesY) origin.y = pack_result.image_height - origin.y - size.y;

      new_tile.name   = image_holder->ImageName ();
      new_tile.image  = atlas_image_name;
      new_tile.origin = origin;
      new_tile.size   = size;

      result.Insert (new_tile);
    }

    result.SetImageSize (atlas_image_name, pack_result.image_width, pack_result.image_height);

    result.Swap (out_atlas);
  }

  void BuildAtlasImage (size_t index, Image& out_atlas_image)
  {
    PackResult& pack_result = *pack_results [index];

    media::PixelFormat atlas_format  = (*images.begin ())->image_holder->ImageFormat ();
    size_t*            current_index = pack_result.indices.data ();

    for (size_t i = 0, count = pack_result.indices.size (); i < count; i++, current_index++)
      atlas_format = max_image_format (atlas_format, images [*current_index]->image_holder->ImageFormat ());

    Image result_image (pack_result.image_width, pack_result.image_height, 1, atlas_format);

    memset (result_image.Bitmap (), 0, pack_result.image_width * pack_result.image_height * get_bytes_per_pixel (result_image.Format ()));

    current_index = pack_result.indices.data ();

    math::vec2ui* current_origin = pack_result.origins.data ();

    for (size_t i = 0, count = pack_result.indices.size (); i < count; i++, current_index++, current_origin++)
    {
      ImageDescPtr   current_image_desc = images [*current_index];
      ImageHolderPtr image_holder       = current_image_desc->image_holder;

      if (current_image_desc->duplicate_of_index != *current_index)
        continue;

      Image image (image_holder->Image ());

      result_image.PutImage (current_origin->x, current_origin->y, 0,
                             image_holder->ImageWidth (), image_holder->ImageHeight (), 1,
                             image_holder->ImageFormat (), image.Bitmap ());
    }

    result_image.Swap (out_atlas_image);
  }
};

/*
   Построитель атласа изображений
*/
  
/*
   Конструкторы / деструктор
*/

AtlasBuilder::AtlasBuilder (const char* pack_handler_name)
{
  try
  {
    const PackHandler& pack_handler = AtlasBuilderManager::GetPacker (pack_handler_name);

    impl = new Impl (pack_handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::AtlasBuilder::AtlasBuilder");
    throw;
  }
}

AtlasBuilder::AtlasBuilder (const PackHandler& pack_handler)
  : impl (new Impl (pack_handler))
{
}

AtlasBuilder::~AtlasBuilder ()
{
}

/*
   Установка/получение максимального размера одного атласа
*/

void AtlasBuilder::SetMaxImageSize (size_t max_image_size)
{
  impl->max_image_size = max_image_size;
  impl->needs_rebuild  = true;
}

size_t AtlasBuilder::MaxImageSize () const
{
  return impl->max_image_size;
}

/*
   Установка/получение размера поля
*/

void AtlasBuilder::SetMargin (size_t margin)
{
  impl->margin        = margin;
  impl->needs_rebuild = true;
}

size_t AtlasBuilder::Margin () const
{
  return impl->margin;
}

/*
   Установка/получение флагов упаковки
*/

void AtlasBuilder::SetPackFlags (size_t pack_flags)
{
  impl->pack_flags    = pack_flags;
  impl->needs_rebuild = true;
}

size_t AtlasBuilder::PackFlags () const
{
  return impl->pack_flags;
}

/*
   Добавление изображений
*/

void AtlasBuilder::Insert (Image& image, AtlasBuilderInsertMode mode)
{
  impl->Insert (image, mode);

  impl->needs_rebuild = true;
}

void AtlasBuilder::Insert (const char* image_name, bool keep_in_memory)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("media::AtlasBuilder::Insert (const char*, bool)", "image_name");

  impl->Insert (image_name, keep_in_memory);

  impl->needs_rebuild = true;
}

/*
   Сброс
*/

void AtlasBuilder::Reset ()
{
  AtlasBuilder ().Swap (*this);
}

/*
   Получение результатов упаковки
*/

size_t AtlasBuilder::AtlasesCount ()
{
  try
  {
    impl->Build ();

    return impl->pack_results.size ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::AtlasBuilder::AtlasesCount");
    throw;
  }
}

void AtlasBuilder::BuildAtlas (size_t index, const char* atlas_image_name, Atlas& result)
{
  try
  {
    if (!atlas_image_name)
      throw xtl::make_null_argument_exception ("", "atlas_image_name");

    impl->Build ();

    if (index >= impl->pack_results.size ())
      throw xtl::make_range_exception ("", "index", index, 0u, impl->pack_results.size ());

    impl->BuildAtlas (index, atlas_image_name, result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::AtlasBuilder::BuildAtlas");
    throw;
  }
}

void AtlasBuilder::BuildAtlasImage (size_t index, Image& result)
{
  try
  {
    impl->Build ();

    if (index >= impl->pack_results.size ())
      throw xtl::make_range_exception ("", "index", index, 0u, impl->pack_results.size ());

    impl->BuildAtlasImage (index, result);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::AtlasBuilder::BuildAtlasImage");
    throw;
  }
}

/*
   Обмен
*/

void AtlasBuilder::Swap (AtlasBuilder& atlas_builder)
{
  stl::swap (impl, atlas_builder.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (AtlasBuilder& atlas_builder1, AtlasBuilder& atlas_builder2)
{
  atlas_builder1.Swap (atlas_builder2);
}

}
