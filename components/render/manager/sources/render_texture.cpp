#include "shared.h"

using namespace render;
using namespace render::manager;
using namespace render::low_level;

namespace
{

/*
    ÷ель рендеринга
*/

struct RenderTargetViewDesc
{
  size_t          layer;         //номер сло€
  size_t          mip_level;     //номер мип-уровн€
  RenderTargetPtr render_target; //цель рендеринга 
  
  RenderTargetViewDesc (size_t in_layer, size_t in_mip_level, const RenderTargetPtr& in_render_target)
    : layer (in_layer)
    , mip_level (in_mip_level)
    , render_target (in_render_target)
  {
  }
};

typedef stl::vector<RenderTargetViewDesc> RenderTargetArray;

render::low_level::PixelFormat get_compressed_pixel_format (const char* name)
{
  if      (!strcmp (name, "rgb_pvrtc2"))  return render::low_level::PixelFormat_RGB_PVRTC2;
  else if (!strcmp (name, "rgb_pvrtc4"))  return render::low_level::PixelFormat_RGB_PVRTC4;
  else if (!strcmp (name, "rgba_pvrtc2")) return render::low_level::PixelFormat_RGBA_PVRTC2;
  else if (!strcmp (name, "rgba_pvrtc4")) return render::low_level::PixelFormat_RGBA_PVRTC4;
  else if (!strcmp (name, "atc"))         return render::low_level::PixelFormat_ATC_RGB_AMD;
  else if (!strcmp (name, "atci"))        return render::low_level::PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD;
  else if (!strcmp (name, "atca"))        return render::low_level::PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
  else if (!strcmp (name, "dxt1"))        return render::low_level::PixelFormat_DXT1;
  else if (!strcmp (name, "dxt3"))        return render::low_level::PixelFormat_DXT3;
  else if (!strcmp (name, "dxt5"))        return render::low_level::PixelFormat_DXT5;

  throw xtl::format_not_supported_exception ("render::manager::get_pixel_format", "Unsupported compression format '%s'", name);
}

}

/*
    ќписание реализации текстуры
*/

struct TextureImpl::Impl: public DebugIdHolder
{
  DeviceManagerPtr               device_manager; //менеджер устройства
  LowLevelTexturePtr             texture;        //текстура
  TextureDimension               dimension;      //размерность текстуры
  render::manager::PixelFormat   format;         //формат текстуры
  render::low_level::PixelFormat target_format;  //целевой формат текстуры
  size_t                         width;          //ширина текстуры
  size_t                         height;         //высота текстуры
  size_t                         depth;          //глубина текстуры либо количество слоЄв
  RenderTargetArray              render_targets; //цели рендеринга
  stl::string                    name;           //им€ текстуры

/// онструктор
  Impl (const DeviceManagerPtr& in_device_manager, TextureDimension in_dimension, PixelFormat in_format, render::low_level::PixelFormat in_target_format, const char* in_name)
    : device_manager (in_device_manager)
    , dimension (in_dimension)
    , format (in_format)
    , target_format (in_target_format)
    , width (0)
    , height (0)
    , depth (0)
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("render::manager::TextureImpl::Impl::Impl", "name");
      
    name = in_name;
    
    if (device_manager->Settings ().HasDebugLog ())
      Log ().Printf  ("Texture '%s' created (id=%u)", name.c_str (), Id ());
  }
  
///ƒеструктор
  ~Impl ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      Log ().Printf  ("Texture '%s' destroyed (id=%u)", name.c_str (), Id ());
  }
  
///ѕолучение цели рендеринга
  RenderTargetPtr GetRenderTarget (size_t layer, size_t mip_level)
  {
      //поиск цели рендеринга среди списка уже созданных
    
    for (RenderTargetArray::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
      if (iter->layer == layer && iter->mip_level == mip_level)
        return iter->render_target;
        
     //создание новой цели рендеринга
     
   RenderTargetPtr render_target (new RenderTargetImpl (device_manager, &*texture, layer, mip_level), false);
   
   render_targets.push_back (RenderTargetViewDesc (layer, mip_level, render_target));
   
   return render_target;
  }
};

/*
     онструкторы / деструктор / присваивание
*/

TextureImpl::TextureImpl 
 (const DeviceManagerPtr&           device_manager,
  render::manager::TextureDimension dimension,
  size_t                            width,
  size_t                            height,
  size_t                            depth,
  render::manager::PixelFormat      format,
  bool                              generate_mips_enable,
  const char*                       name)
{
  try
  {
      //преобразование аргументов
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    low_level::PixelFormat      target_format;
    low_level::TextureDimension target_dimension;
    
    switch (format)
    {
      case PixelFormat_RGB8:
        target_format = low_level::PixelFormat_RGB8;
        break;
      case PixelFormat_RGBA8:
        target_format = low_level::PixelFormat_RGBA8;
        break;
      case PixelFormat_L8:
        target_format = low_level::PixelFormat_L8;
        break;
      case PixelFormat_A8:
        target_format = low_level::PixelFormat_A8;
        break;
      case PixelFormat_LA8:
        target_format = low_level::PixelFormat_LA8;
        break;
      case PixelFormat_D16:
        target_format = low_level::PixelFormat_D16;
        break;
      case PixelFormat_D24X8:
        target_format = low_level::PixelFormat_D24X8;
        break;
      case PixelFormat_D24S8:
        target_format = low_level::PixelFormat_D24S8;
        break;
      default:
        throw xtl::make_argument_exception ("", "format", format);
    }
    
    switch (dimension)
    {
      case TextureDimension_2D:
        target_dimension = low_level::TextureDimension_2D;
        break;
      case TextureDimension_3D:
        target_dimension = low_level::TextureDimension_3D;
        break;
      case TextureDimension_Cubemap:
        target_dimension = low_level::TextureDimension_Cubemap;
        break;
      default:
        throw xtl::make_argument_exception ("", "dimension", dimension);
    }

      //создание текстуры
      
    low_level::TextureDesc desc;
    
    memset (&desc, 0, sizeof desc);
    
    desc.dimension            = target_dimension;
    desc.width                = width;
    desc.height               = height;
    desc.layers               = depth;
    desc.format               = target_format;
    desc.generate_mips_enable = generate_mips_enable;
    desc.access_flags         = low_level::AccessFlag_ReadWrite;
    desc.bind_flags           = BindFlag_Texture | BindFlag_RenderTarget;
    desc.usage_mode           = UsageMode_Static;

    impl = new Impl (device_manager, dimension, format, target_format, name);

    impl->width   = width;
    impl->height  = height;
    impl->depth   = depth;
    impl->texture = LowLevelTexturePtr (device_manager->Device ().CreateTexture (desc), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::TextureImpl(const DeviceManagerPtr&,render::manager::TextureDimension,size_t,size_t,size_t,render::manager::PixelFormat,bool,const char*)");
    throw;
  }
}

TextureImpl::TextureImpl 
 (const DeviceManagerPtr&       device_manager,
  const low_level::TextureDesc& desc,
  const char*                   name)
{
  try
  {
      //преобразование аргументов
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    PixelFormat      format;
    TextureDimension dimension;
    
    switch (desc.format)
    {
      case low_level::PixelFormat_RGB8:
        format = PixelFormat_RGB8;
        break;
      case low_level::PixelFormat_RGBA8:
        format = PixelFormat_RGBA8;
        break;
      case low_level::PixelFormat_L8:
        format = PixelFormat_L8;
        break;
      case low_level::PixelFormat_A8:
        format = PixelFormat_A8;
        break;
      case low_level::PixelFormat_LA8:
        format = PixelFormat_LA8;
        break;
      case low_level::PixelFormat_D16:
        format = PixelFormat_D16;
        break;
      case low_level::PixelFormat_D24X8:
        format = PixelFormat_D24X8;
        break;
      case low_level::PixelFormat_D24S8:
        format = PixelFormat_D24S8;
        break;
      case low_level::PixelFormat_D32:
        format = PixelFormat_D32;
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.format", desc.format);
    }
    
    switch (desc.dimension)
    {
      case low_level::TextureDimension_2D:
        dimension = TextureDimension_2D;
        break;
      case low_level::TextureDimension_3D:
        dimension = TextureDimension_3D;
        break;
      case low_level::TextureDimension_Cubemap:
        dimension = TextureDimension_Cubemap;
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.dimension", desc.dimension);
    }

      //создание текстуры
      
    impl = new Impl (device_manager, dimension, format, desc.format, name);

    impl->width   = desc.width;
    impl->height  = desc.height;
    impl->depth   = desc.layers;
    impl->texture = LowLevelTexturePtr (device_manager->Device ().CreateTexture (desc), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::TextureImpl(const DeviceManagerPtr&,const render::low_level::TextureDesc&,const char*)");
    throw;
  }
}

TextureImpl::TextureImpl (const DeviceManagerPtr& device_manager, render::manager::TextureDimension dimension, const media::CompressedImage& image, const char* name)
{
  try
  {
      //преобразование аргументов
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    low_level::PixelFormat      target_format = get_compressed_pixel_format (image.Format ());
    low_level::TextureDimension target_dimension;

    switch (dimension)
    {
      case TextureDimension_2D:
        target_dimension = low_level::TextureDimension_2D;
        break;
      case TextureDimension_3D:
        target_dimension = low_level::TextureDimension_3D;
        break;
      case TextureDimension_Cubemap:
        target_dimension = low_level::TextureDimension_Cubemap;
        break;
      default:
        throw xtl::make_argument_exception ("", "dimension", dimension);
    }

      //создание текстуры
      
    low_level::TextureDesc desc;
    
    memset (&desc, 0, sizeof desc);
    
    desc.dimension            = target_dimension;
    desc.width                = image.Width ();
    desc.height               = image.Height ();
    desc.layers               = image.LayersCount ();
    desc.format               = target_format;
    desc.generate_mips_enable = false;
    desc.access_flags         = low_level::AccessFlag_ReadWrite;
    desc.bind_flags           = BindFlag_Texture | BindFlag_RenderTarget;
    desc.usage_mode           = UsageMode_Static;
    
    stl::vector<size_t> sizes (image.BlocksCount ());
    const media::CompressedImageBlockDesc* blocks = image.Blocks ();

    for (size_t i=0, count=sizes.size (); i<count; i++)
      sizes [i] = blocks [i].size;

    TextureData data;

    memset (&data, 0, sizeof (TextureData));

    data.data  = image.Data ();
    data.sizes = &sizes [0];    
    
    PixelFormat format;
    
    switch (target_format)
    {
      case render::low_level::PixelFormat_DXT1:                            format = PixelFormat_DXT1; break;
      case render::low_level::PixelFormat_DXT3:                            format = PixelFormat_DXT3; break;
      case render::low_level::PixelFormat_DXT5:                            format = PixelFormat_DXT5; break;
      case render::low_level::PixelFormat_ATC_RGB_AMD:                     format = PixelFormat_ATC_RGB_AMD; break;
      case render::low_level::PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:     format = PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD; break;
      case render::low_level::PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD: format = PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD; break;
      case render::low_level::PixelFormat_RGB_PVRTC2:                      format = PixelFormat_RGB_PVRTC2; break;
      case render::low_level::PixelFormat_RGB_PVRTC4:                      format = PixelFormat_RGB_PVRTC4; break;
      case render::low_level::PixelFormat_RGBA_PVRTC2:                     format = PixelFormat_RGBA_PVRTC2; break;
      case render::low_level::PixelFormat_RGBA_PVRTC4:                     format = PixelFormat_RGBA_PVRTC4; break;
      default:
        throw xtl::format_operation_exception ("", "Wrong compressed image pixel format %s", get_name (target_format));
    }    

    impl = new Impl (device_manager, dimension, format, target_format, name);
        
    impl->width   = desc.width;
    impl->height  = desc.height;
    impl->depth   = desc.layers;
    impl->texture = LowLevelTexturePtr (device_manager->Device ().CreateTexture (desc, data), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::TextureImpl(const DeviceManagerPtr&,render::manager::TextureDimension,const media::CompressedImage&,const char*)");
    throw;
  }
}

TextureImpl::~TextureImpl ()
{
}

/*
    »м€ текстуры
*/

const char* TextureImpl::Name ()
{
  return impl->name.c_str ();
}

void TextureImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::TextureImpl::SetName", "name");
    
  if (impl->device_manager->Settings ().HasDebugLog ())
    Log ().Printf ("Texture '%s' name changed to '%s' (id=%u)", impl->name.c_str (), name, impl->Id ());  

  impl->name = name;
}

/*
    Ќизкоуровневый объект текстуры
*/

LowLevelTexturePtr TextureImpl::DeviceTexture ()
{
  return impl->texture;
}

/*
    –азмерность
*/

render::manager::TextureDimension TextureImpl::Dimension ()
{
  return impl->dimension;
}

/*
    ‘ормат и размеры
*/

render::manager::PixelFormat TextureImpl::Format ()
{
  return impl->format;
}

size_t TextureImpl::Width ()
{
  return impl->width;
}

size_t TextureImpl::Height ()
{
  return impl->height;
}

size_t TextureImpl::Depth ()
{
  return impl->depth;
}

/*
    ѕолучение цели рендеринга
*/

RenderTargetPtr TextureImpl::RenderTarget (size_t layer, size_t mip_level)
{
  try
  {
    return impl->GetRenderTarget (layer, mip_level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::RenderTarget");
    throw;
  }
}

/*
    ќбновление образа
*/

void TextureImpl::Update (const media::Image& image)
{
  try
  {
      //проверка возможности обновлени€
      
    if (image.Width () != impl->width)
      throw xtl::format_operation_exception ("", "Image width %u mismatch texture width %u", image.Width (), impl->width);
      
    if (image.Height () != impl->height)
      throw xtl::format_operation_exception ("", "Image height %u mismatch texture height %u", image.Height (), impl->height);
      
    if (image.Depth () != impl->depth)
      throw xtl::format_operation_exception ("", "Image depth %u mismatch texture depth %u", image.Depth (), impl->depth);
      
    low_level::PixelFormat source_format;

    switch (image.Format ())
    {
      case media::PixelFormat_RGB8:
        source_format = low_level::PixelFormat_RGB8;
        break;
      case media::PixelFormat_RGBA8:
        source_format = low_level::PixelFormat_RGBA8;
        break;     
      case media::PixelFormat_L8:
        source_format = low_level::PixelFormat_L8;
        break;
      case media::PixelFormat_A8:
        source_format = low_level::PixelFormat_A8;
        break;
      case media::PixelFormat_LA8:
        source_format = low_level::PixelFormat_LA8;
        break;
      case media::PixelFormat_RGB16:
      case media::PixelFormat_BGR8:
      case media::PixelFormat_RGBA16:
      case media::PixelFormat_BGRA8:
      {
        media::PixelFormat convertion_format;
        
        switch (image.Format ())
        {
          case media::PixelFormat_RGB16:
            convertion_format = media::PixelFormat_RGB8;
            break;
          case media::PixelFormat_BGR8:
            convertion_format = media::PixelFormat_RGB8;
            break;
          case media::PixelFormat_RGBA16:
            convertion_format = media::PixelFormat_RGBA8;
            break;
          case media::PixelFormat_BGRA8:
            convertion_format = media::PixelFormat_RGBA8;
            break;
          default:
            throw xtl::format_not_supported_exception ("", "Unsupported image format '%s'", media::get_format_name (image.Format ()));
        }

        media::Image converted_image (image, convertion_format);

        Update (converted_image);

        return;
      }
      default:
        throw xtl::format_not_supported_exception ("", "Unsupported image format '%s'", media::get_format_name (image.Format ()));
    }
    
      //обновление данных

    for (size_t i=0; i<impl->depth; i++)
      impl->texture->SetData (i, 0, 0, 0, impl->width, impl->height, source_format, image.Bitmap (i));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::Update");
    throw;
  }
}

/*
    «ахват образа
*/

namespace
{

//получение формата пикселей дл€ изображени€
media::PixelFormat get_format (render::manager::PixelFormat format)
{
  switch (format)
  {
    case render::manager::PixelFormat_RGB8:  return media::PixelFormat_RGB8;
    case render::manager::PixelFormat_RGBA8: return media::PixelFormat_RGBA8;
    case render::manager::PixelFormat_L8:    return media::PixelFormat_L8;
    case render::manager::PixelFormat_A8:    return media::PixelFormat_A8;
    case render::manager::PixelFormat_LA8:   return media::PixelFormat_LA8;
    default:
      throw xtl::format_not_supported_exception ("render::manager::get_format(render::manager::PixelFormat)", "Format '%s' not supported", get_name (format));
  }
}

}

void TextureImpl::Capture (size_t layer, size_t mip_level, media::Image& image)
{
  try
  {
    media::PixelFormat image_format = get_format (impl->format);

    media::Image result_image (impl->width, impl->height, 1, image_format);

    impl->texture->GetData (0, 0, 0, 0, impl->width, impl->height, impl->target_format, result_image.Bitmap ());

    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::Capture(size_t,size_t,media::Image&)");
    throw;
  }
}

void TextureImpl::Capture (size_t mip_level, media::Image& image)
{
  try
  {
    media::PixelFormat image_format = get_format (impl->format);
    
    size_t width = impl->width, height = impl->height, depth = impl->depth;
    
    for (size_t i=0; i<mip_level; i++)
    {
      width  = stl::max (width / 2, (size_t)1);
      height = stl::max (height / 2, (size_t)1);
      depth  = impl->dimension == TextureDimension_Cubemap ? depth : stl::max (depth / 2, (size_t)1);
    }

    media::Image result_image (width, height, depth, image_format);

    for (size_t i=0; i<depth; i++)
      impl->texture->GetData (i, mip_level, 0, 0, width, height, impl->target_format, result_image.Bitmap (i));

    result_image.Swap (image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureImpl::Capture(size_t,media::Image&)");
    throw;
  }
}
