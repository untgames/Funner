#include "shared.h"

namespace render {
namespace scene {
namespace interchange {

void write (OutputStream& s, const media::Image& image)
{
  if (s.InprocessOwnerId ())
  {
    object_id_t object_id = InprocessExchangeStorage::Attach (s.InprocessOwnerId (), image);

    write (s, object_id);
  }
  else
  {
    object_id_t object_id = 0;

    write (s, object_id);

    uint32 width = image.Width (), height = image.Height (), depth = image.Depth (), format = (uint32)image.Format ();

    write (s, width);
    write (s, height);
    write (s, depth);
    write (s, format);

    size_t layer_size = width * height * media::get_bytes_per_pixel (image.Format ());

    for (size_t i=0; i<depth; i++)
      s.WriteData (image.Bitmap (i), layer_size);
  }
}

media::Image read (InputStream& s, xtl::type<media::Image>)
{
  object_id_t object_id = read (s, xtl::type<object_id_t> ());

  if (object_id)
  {
    media::Image image = InprocessExchangeStorage::Attachment<media::Image> (object_id);

    InprocessExchangeStorage::Detach (object_id);

    return image;
  }
  else
  {
    uint32 width = read (s, xtl::type<uint32> ()), height = read (s, xtl::type<uint32> ()), depth = read (s, xtl::type<uint32> ()), format = read (s, xtl::type<uint32> ());

    media::Image image (width, height, depth, (media::PixelFormat)format);

    size_t layer_size = width * height * media::get_bytes_per_pixel (image.Format ());

    for (size_t i=0; i<depth; i++)
      s.ReadData (image.Bitmap (i), layer_size);

    return image;
  }
}

}}}
