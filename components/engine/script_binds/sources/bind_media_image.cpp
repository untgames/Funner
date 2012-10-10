#include "shared.h"

using namespace media;
using namespace script;
using namespace xtl;

namespace components
{

namespace media_image_script_bind
{

/*
    Константы (имена библиотек)
*/

const char* IMAGE_LIBRARY        = "Media.Image";
const char* IMAGE_FORMAT_LIBRARY = "Media.ImageFormat";
const char* BINDER_NAME          = "MediaImage";
const char* COMPONENT_NAME       = "script.binds.MediaImage";

void bind_image_format_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (IMAGE_FORMAT_LIBRARY);

  lib.Register ("get_Default", make_const (PixelFormat_Default));
  lib.Register ("get_RGB8",    make_const (PixelFormat_RGB8));
  lib.Register ("get_RGB16",   make_const (PixelFormat_RGB16));
  lib.Register ("get_BGR8",    make_const (PixelFormat_BGR8));
  lib.Register ("get_RGBA8",   make_const (PixelFormat_RGBA8));
  lib.Register ("get_RGBA16",  make_const (PixelFormat_RGBA16));
  lib.Register ("get_BGRA8",   make_const (PixelFormat_BGRA8));
  lib.Register ("get_L8",      make_const (PixelFormat_L8));
  lib.Register ("get_A8",      make_const (PixelFormat_A8));
  lib.Register ("get_LA8",     make_const (PixelFormat_LA8));
}

///Функции
Image create_image (size_t width, size_t height, size_t depth, PixelFormat format)
{
  return Image (width, height, depth, format);
}

Image load_image (const char* file_name)
{
  return Image (file_name);
}

Image create_default_image ()
{
  return Image ();
}

void fill_rgba (Image& image, math::vec4f color)
{
  image.Fill (PixelFormat_RGBA8, &color);
}

size_t put_image (IStack& stack)
{
  Image  sender   = stack.GetVariant (1).cast<Image&> ();
  size_t x        = stack.GetInteger (2);
  size_t y        = stack.GetInteger (3);
  size_t z        = stack.GetInteger (4);
  size_t width    = stack.GetInteger (5);
  size_t height   = stack.GetInteger (6);
  size_t depth    = stack.GetInteger (7);
  Image  source   = stack.GetVariant (8).cast<Image&> ();
  size_t target_x = stack.GetInteger (9);
  size_t target_y = stack.GetInteger (10);
  size_t target_z = stack.GetInteger (11);

  sender.PutImage (x, y, z, width, height, depth, source, target_x, target_y, target_z);

  return 0;
}

///Регистрация библиотеки работы с картинками
void bind_image_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (IMAGE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (&create_image),
                                        make_invoker (&load_image),
                                        make_invoker (&create_default_image)));

    //регистрация операций

  lib.Register ("get_Width",    make_invoker (&Image::Width));
  lib.Register ("get_Height",   make_invoker (&Image::Height));
  lib.Register ("get_Depth",    make_invoker (&Image::Depth));
  lib.Register ("get_Format",   make_invoker (&Image::Format));
  lib.Register ("get_Name",     make_invoker (&Image::Name));
  lib.Register ("set_Name",     make_invoker (&Image::Rename));

  lib.Register ("Clone",    make_invoker (&Image::Clone));
  lib.Register ("Load",     make_invoker (&Image::Clone));
  lib.Register ("Save",     make_invoker (make_invoker (&Image::Save),
      make_invoker<void (Image&, const char*, PixelFormat)>(xtl::bind (&Image::Save, _1, _2, _3, "")),
      make_invoker<void (Image&, const char*)>(xtl::bind (&Image::Save, _1, _2, PixelFormat_Default, ""))));
  lib.Register ("Convert",  make_invoker (&Image::Convert));
  lib.Register ("Resize",   make_invoker (&Image::Resize));
  lib.Register ("FillRGBA", make_invoker (&fill_rgba));


  lib.Register ("PutImage", &put_image);

  environment.RegisterType<Image> (IMAGE_LIBRARY);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_image_format_library (environment);
      bind_image_library        (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> MediaImageScriptBind (COMPONENT_NAME);

}

}

}
