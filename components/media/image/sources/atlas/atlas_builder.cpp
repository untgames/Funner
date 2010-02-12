#include "shared.h"

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

}

/*
   Реализация построителя атласа изображений
*/

struct AtlasBuilder::Impl
{
  public:
    typedef AtlasBuilder::PackHandler PackHandler;
  
/*
   Конструкторы / деструктор
*/
    Impl (const PackHandler& in_pack_handler)
      : pack_handler (in_pack_handler)
      {}

    ~Impl ()
    {
    }

/*
   Установка имени атласной картинки (полной)
*/
    void SetAtlasImageName (const char* name)
    {
      if (!name)
        xtl::make_null_argument_exception ("media::AtlasBuilder::SetAtlasImageName", "name");

      atlas_image_name = name;
    }

    const char* AtlasImageName () const
    {
      return atlas_image_name.c_str ();
    }

/*
   Добавление изображений
*/
    void Insert (Image& image, AtlasBuilderInsertMode mode)
    {
      switch (mode)
      {
        case AtlasBuilderInsertMode_Copy:
        {
          try
          {
            images.push_back (GetImageDesc (ImageHolderPtr (new ImageHolder (new Image (image)), false)));
          }
          catch (xtl::exception& exception)
          {
            exception.touch ("media::AtlasBuilder::Insert");
            throw;
          }
          
          break;
        }
        case AtlasBuilderInsertMode_Capture:
        {
          try
          {
            ImageHolderPtr new_image (new ImageHolder (new Image ()), false);

            new_image->image->Swap (image);

            images.push_back (GetImageDesc (new_image));
          }
          catch (xtl::exception& exception)
          {
            exception.touch ("media::AtlasBuilder::Insert");
            throw;
          }

          break;
        }
        case AtlasBuilderInsertMode_Reference:
        {
          try
          {
            images.push_back (GetImageDesc (ImageHolderPtr (new ImageHolder (&image))));
          }
          catch (xtl::exception& exception)
          {
            exception.touch ("media::AtlasBuilder::Insert");
            throw;
          }

          break;
        }
        default:
          throw xtl::make_argument_exception ("media::AtlasBuilder::Insert", "mode", mode, "Unknown insert mode");
      }
    }

    void Insert (const char* image_name)
    {
      try
      {
        images.push_back (GetImageDesc (ImageHolderPtr (new ImageHolder (new Image (image_name)), false)));
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("media::AtlasBuilder::Insert");
        throw;
      }
    }

/*
   Построение карты
*/
    void CalculatePackResults (size_t margin, size_t pack_flags, xtl::uninitialized_storage<math::vec2ui>& sizes,
                               xtl::uninitialized_storage<math::vec2ui>& origins,
                               size_t& result_image_width, size_t& result_image_height)
    {
      size_t unique_images_count = images_hash_map.size ();

      sizes.resize (unique_images_count);
      origins.resize (unique_images_count);

      math::vec2ui *current_size = sizes.data ();

      for (size_t i = 0; i < images.size (); i++)
      {
        ImageDescPtr current_image_desc = images [i];

        if (current_image_desc->duplicate_of_index != i)
          continue;

        current_size->x = current_image_desc->image_holder->image->Width ();
        current_size->y = current_image_desc->image_holder->image->Height ();

        current_size++;
      }

      pack_handler (sizes.size (), sizes.data (), origins.data (), margin, pack_flags & ~AtlasPackFlag_InvertTilesX & ~AtlasPackFlag_InvertTilesY);

      for (size_t i = 0; i < origins.size (); i++)
      {
        math::vec2ui& origin = origins.data ()[i];

        if (result_image_width < (origin.x + sizes.data ()[i].x))  result_image_width = origin.x + sizes.data ()[i].x;
        if (result_image_height < (origin.y + sizes.data ()[i].y)) result_image_height = origin.y + sizes.data ()[i].y;
      }

      if (pack_flags & AtlasPackFlag_PowerOfTwoEdges)
      {
        result_image_width  = get_next_higher_power_of_two (result_image_width);
        result_image_height = get_next_higher_power_of_two (result_image_height);
      }
    }

    void Build (Atlas& out_atlas, Image& out_atlas_image, size_t margin, size_t pack_flags)
    {
      if (images.empty ())
      {
        Atlas ().Swap (out_atlas);
        Image ().Swap (out_atlas_image);

        return;
      }

      try
      {
          //упаковка
        xtl::uninitialized_storage<math::vec2ui> sizes, origins;

          //заполнение атласа
          
        Atlas  result_atlas;
        size_t result_image_width = 0;
        size_t result_image_height = 0;                                  

        CalculatePackResults (margin, pack_flags, sizes, origins, result_image_width, result_image_height);
        
        for (size_t i = 0; i < images.size (); i++)
        {
          ImageDescPtr current_image_desc = images [i];
          Tile         new_tile;
          
          math::vec2ui& origin = origins.data ()[current_image_desc->unique_index];
          math::vec2ui& size   = sizes.data ()[current_image_desc->unique_index];

          if (pack_flags & AtlasPackFlag_InvertTilesX) origin.x = result_image_width  - origin.x - size.x;
          if (pack_flags & AtlasPackFlag_InvertTilesY) origin.y = result_image_height - origin.y - size.y;          

          new_tile.name   = current_image_desc->image_holder->image->Name ();
          new_tile.image  = atlas_image_name.c_str ();
          new_tile.origin = origin;
          new_tile.size   = size;

          result_atlas.Insert (new_tile);
        }

          //создание изображения атласа

        Image result_image (result_image_width, result_image_height, 1, (*images.begin ())->image_holder->image->Format ());

        memset (result_image.Bitmap (), 0, result_image_width * result_image_height * get_bytes_per_pixel (result_image.Format ()));

        for (size_t i = 0; i < images.size (); i++)
        {
          ImageDescPtr current_image_desc = images [i];

          if (current_image_desc->duplicate_of_index != i)
            continue;

          size_t unique_index = current_image_desc->unique_index;

          result_image.PutImage (origins.data ()[unique_index].x, origins.data ()[unique_index].y, 0,
                                 sizes.data ()[unique_index].x, sizes.data ()[unique_index].y, 1,
                                 current_image_desc->image_holder->image->Format (), current_image_desc->image_holder->image->Bitmap ());
        }

        result_atlas.SetImageSize (atlas_image_name.c_str (), result_image_width, result_image_height);

        result_atlas.Swap (out_atlas);
        result_image.Swap (out_atlas_image);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("media::AtlasBuilder::Build");
        throw;
      }
    }

    void GetBuildResults (size_t& image_width, size_t& image_height, size_t margin, size_t pack_flags)
    {
      xtl::uninitialized_storage<math::vec2ui> sizes, origins;

      CalculatePackResults (margin, pack_flags, sizes, origins, image_width, image_height);
    }

  private:
    struct ImageHolder : public xtl::reference_counter
    {
      public:
        Image* image;

        ImageHolder (Image* in_image) : image (in_image) {}

        ~ImageHolder ()
        {
          delete image;
        }

        void AddRef ()
        {
          addref (this);
        }

        void Release ()
        {
          release (this);
        }

      private:
        ImageHolder (const ImageHolder&); //no impl
        ImageHolder& operator = (const ImageHolder&); //no impl
    };

    typedef xtl::com_ptr<ImageHolder> ImageHolderPtr;

    struct ImageDesc
    {
      ImageHolderPtr image_holder;
      size_t         duplicate_of_index;   //индекс картинки в массиве images, дубликатом которой является эта картинка
      size_t         unique_index;         //размер карты уникальных картинок в момент добавления этой картинки
    };

    typedef xtl::shared_ptr<ImageDesc>    ImageDescPtr;
    typedef stl::vector<ImageDescPtr>     ImagesDescsArray;
    typedef stl::hash_map<size_t, size_t> BitmapHashMap;

  private:
    ImageDescPtr GetImageDesc (ImageHolderPtr image_holder)
    {
      ImageDescPtr image_desc (new ImageDesc);
      Image*       image = image_holder->image;

      image_desc->image_holder = image_holder;

      size_t bitmap_hash = common::crc32 (image->Bitmap (), image->Width () * image->Height () * get_bytes_per_pixel (image->Format ()));

      BitmapHashMap::iterator iter = images_hash_map.find (bitmap_hash);

      if (iter == images_hash_map.end ())
      {
        image_desc->duplicate_of_index = images.size ();
        image_desc->unique_index       = images_hash_map.size ();
        images_hash_map.insert_pair (bitmap_hash, image_desc->duplicate_of_index);
      }
      else
      {
        image_desc->duplicate_of_index = iter->second;
        image_desc->unique_index       = images [image_desc->duplicate_of_index]->unique_index;
      }

      return image_desc;
    }

  private:
    PackHandler      pack_handler;
    stl::string      atlas_image_name;
    ImagesDescsArray images;
    BitmapHashMap    images_hash_map;
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
   Установка имени атласной картинки (полной)
*/

void AtlasBuilder::SetAtlasImageName (const char* name)
{
  impl->SetAtlasImageName (name);
}

const char* AtlasBuilder::AtlasImageName () const
{
  return impl->AtlasImageName ();
}

/*
   Добавление изображений
*/

void AtlasBuilder::Insert (Image& image, AtlasBuilderInsertMode mode)
{
  impl->Insert (image, mode);
}

void AtlasBuilder::Insert (const char* image_name)
{
  impl->Insert (image_name);
}

/*
   Сброс
*/

void AtlasBuilder::Reset ()
{
  AtlasBuilder ().Swap (*this);
}

/*
   Построение карты/получение размеров результирующей картинки без формирования
*/

void AtlasBuilder::Build (Atlas& out_atlas, Image& out_atlas_image, size_t margin, size_t pack_flags)
{
  impl->Build (out_atlas, out_atlas_image, margin, pack_flags);
}

void AtlasBuilder::GetBuildResults (size_t& image_width, size_t& image_height, size_t margin, size_t pack_flags)
{
  impl->GetBuildResults (image_width, image_height, margin, pack_flags);
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
