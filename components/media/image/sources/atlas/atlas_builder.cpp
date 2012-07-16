#include "shared.h"

using namespace media;

namespace
{

const size_t MIN_BUILD_PACKET_SIZE = 16; //минимальный размер пакета после которого идет упаковка по одной картинке

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

  ///Построение карты
  void CalculatePackResults (size_t count, xtl::uninitialized_storage<math::vec2ui>& sizes,
                             xtl::uninitialized_storage<math::vec2ui>& origins,
                             size_t& result_image_width, size_t& result_image_height)
  {
    result_image_width = result_image_height = 0;

    pack_handler (count, sizes.data (), origins.data (), margin, pack_flags & ~AtlasPackFlag_InvertTilesX & ~AtlasPackFlag_InvertTilesY);

    math::vec2ui *current_origin = origins.data (),
                 *current_size   = sizes.data ();

    for (size_t i = 0; i < count; i++, current_origin++, current_size++)
    {
      if (result_image_width < (current_origin->x + current_size->x))  result_image_width = current_origin->x + current_size->x;
      if (result_image_height < (current_origin->y + current_size->y)) result_image_height = current_origin->y + current_size->y;
    }

    if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
    {
      result_image_width  = get_next_higher_power_of_two (result_image_width);
      result_image_height = get_next_higher_power_of_two (result_image_height);
    }

    if (pack_flags & AtlasPackFlag_SquareAxises)
    {
      result_image_width = result_image_height = stl::max (result_image_width, result_image_height);
    }
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
      stl::vector<size_t> images_to_process;

      images_to_process.reserve (images.size ());

      for (size_t i = 0, count = images.size (); i != count; i++)
      {
        ImageDescPtr image_desc = images [i];

        if (i == image_desc->duplicate_of_index)
          images_to_process.push_back (i);
      }

        //упаковка
      xtl::uninitialized_storage<math::vec2ui> sizes (images_to_process.size ()), origins (images_to_process.size ()), success_origins (images_to_process.size ());
      xtl::uninitialized_storage<size_t> processed_images (images_to_process.size ());

      while (!images_to_process.empty ())
      {
        size_t packed_count = 0,
               result_image_width,
               result_image_height,
               success_result_image_width,
               success_result_image_height,
               try_packet_size = images_to_process.size ();  //количество картинок, которое пробуем добавить в атлас за один раз

        while (try_packet_size >= 1)
        {
          size_t try_from = 0;

          for (size_t i = 0, packets_count = (size_t)ceil (images_to_process.size () / (float)try_packet_size); i < packets_count; i++)
          {
            size_t begin_index = try_from,
                   end_index   = stl::min (begin_index + try_packet_size, images_to_process.size ()),
                   packet_size = end_index - begin_index;

            for (size_t j = begin_index; j < end_index; j++)
            {
              size_t image_index = images_to_process [j],
                     dst_index   = packed_count + j - begin_index;

              ImageHolderPtr image_holder = images [image_index]->image_holder;

              sizes.data () [dst_index].x = image_holder->ImageWidth ();
              sizes.data () [dst_index].y = image_holder->ImageHeight ();

              processed_images.data () [dst_index] = image_index;
            }

            CalculatePackResults (packed_count + packet_size, sizes, origins, result_image_width, result_image_height);

            if (result_image_width <= max_image_size && result_image_height <= max_image_size) //размещены все картинки пакета
            {
              images_to_process.erase (images_to_process.begin () + begin_index, images_to_process.begin () + end_index);
              packed_count += packet_size;

              success_result_image_width  = result_image_width;
              success_result_image_height = result_image_height;

              memcpy (success_origins.data (), origins.data (), packed_count * sizeof (*origins.data ()));
            }
            else  //не удалось разместить все картинки пакета
            {
              try_from += packet_size;
            }
          }

          try_packet_size /= 2;

          if (try_packet_size && try_packet_size < MIN_BUILD_PACKET_SIZE)
            try_packet_size = 1;
        }

        if (!packed_count)
          throw xtl::format_operation_exception ("", "Can't pack images, image '%s' greater then maximum image size %u",
                                                 images [images_to_process [0]]->image_holder->ImageName (), max_image_size);

          //подсчет количества упакованных неуникальных картинок
        size_t packed_images_count = 0;

        for (size_t i = 0, images_count = images.size (); i < images_count; i++)
        {
          size_t duplicate_of_index = images [i]->duplicate_of_index;

          size_t* current_processed_image = processed_images.data ();

          for (size_t j = 0; j < packed_count; j++, current_processed_image++)
          {
            if (*current_processed_image == duplicate_of_index)
            {
              packed_images_count++;
              break;
            }
          }
        }

          //формирование результата упаковки
        PackResultPtr pack_result (new PackResult, false);

        pack_result->image_width  = success_result_image_width;
        pack_result->image_height = success_result_image_height;
        pack_result->origins.resize (packed_images_count);
        pack_result->indices.resize (packed_images_count);

        size_t        pack_result_index = 0;
        math::vec2ui* current_origin    = pack_result->origins.data ();
        size_t*       current_index     = pack_result->indices.data ();

        for (size_t i = 0, images_count = images.size (); i < images_count; i++)
        {
          size_t duplicate_of_index = images [i]->duplicate_of_index;

          size_t* current_processed_image = processed_images.data ();

          for (size_t j = 0; j < packed_count; j++, current_processed_image++)
          {
            if (*current_processed_image == duplicate_of_index)
            {
              *current_origin = success_origins.data () [j];
              *current_index  = i;

              pack_result_index++;
              current_origin++;
              current_index++;

              break;
            }
          }
        }

        pack_results.push_back (pack_result);
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

    Image result_image (pack_result.image_width, pack_result.image_height, 1, (*images.begin ())->image_holder->ImageFormat ());

    memset (result_image.Bitmap (), 0, pack_result.image_width * pack_result.image_height * get_bytes_per_pixel (result_image.Format ()));

    size_t*       current_index  = pack_result.indices.data ();
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
