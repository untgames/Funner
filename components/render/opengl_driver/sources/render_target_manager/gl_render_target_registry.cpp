#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
===================================================================================================
    Утилиты
===================================================================================================
*/

/*
    Фабрика буфера кадра
*/

struct FrameBufferFactory
{
  FrameBufferChecker checker; //функтор, проверяющий возможность создания буфера кадра из двух отображений
  FrameBufferCreater creater; //функтор, создающий буфер кадра по двум отображениям
  
  FrameBufferFactory (const FrameBufferChecker& in_checker, const FrameBufferCreater& in_creater) : checker (in_checker), creater (in_creater) {}
};

/*
    Хранилище буфера кадра
*/

typedef stl::auto_ptr<IFrameBuffer> FrameBufferPtr;

struct FrameBufferHolder: public xtl::reference_counter
{
  FrameBufferPtr frame_buffer;

  FrameBufferHolder (FrameBufferPtr& in_frame_buffer) : frame_buffer (in_frame_buffer) {}
};

/*
    Целевые отображения (ключ поиска в карте буферов кадра)
*/

struct RenderTargetViews
{
  View* render_target_view;
  View* depth_stencil_view;

  RenderTargetViews (View* in_render_target_view, View* in_depth_stencil_view) :
    render_target_view (in_render_target_view), depth_stencil_view (in_depth_stencil_view) {}
    
  bool HasView (View* view) const { return render_target_view == view || depth_stencil_view == view; }

  bool operator == (const RenderTargetViews& views) const
  {
    return render_target_view == views.render_target_view && depth_stencil_view == views.depth_stencil_view;
  }
};

inline size_t hash (const RenderTargetViews& views)
{
  return stl::hash (views.render_target_view, stl::hash (views.depth_stencil_view));
}

//получение строки с конфигурацией буфера кадра
inline void get_configuration_name (View* color_view, View* depth_stencil_view, stl::string& out_name)
{
  format ("color-view=%s, depth-stencil-view=%s", color_view ? color_view->GetTextureTypeName () : "null",
          depth_stencil_view ? depth_stencil_view->GetTextureTypeName () : "null").swap (out_name);
}

}

/*
===================================================================================================
     RenderTargetRegistry
===================================================================================================
*/

/*
    Описание реализации менеджера буферов кадра
*/

typedef stl::list<FrameBufferFactory>                          FrameBufferFactoryList;
typedef xtl::intrusive_ptr<FrameBufferHolder>                  FrameBufferHolderPtr;
typedef stl::hash_map<RenderTargetViews, FrameBufferHolderPtr> FrameBufferMap;

struct RenderTargetRegistry::Impl
{
  FrameBufferFactoryList    frame_buffer_factories;       //список фабрик буферов кадра
  RenderBufferCreater       render_buffer_creater;        //функтор создания буфера рендеринга
  ColorBufferCreater        color_buffer_creater;         //функтор создания буфера цвета
  DepthStencilBufferCreater depth_stencil_buffer_creater; //функтор создания буфера попиксельного отсечения
  FrameBufferMap            frame_buffers;                //буферы кадра
};

/*
    Конструктор / деструктор
*/

RenderTargetRegistry::RenderTargetRegistry ()
  : impl (new Impl)
{
}

RenderTargetRegistry::~RenderTargetRegistry ()
{
}

/*
    Регистрация обработчиков создания буферов кадра / буферов отрисовки
*/

void RenderTargetRegistry::RegisterCreater (const FrameBufferChecker& checker, const FrameBufferCreater& creater)
{
  impl->frame_buffer_factories.push_front (FrameBufferFactory (checker, creater));
}

void RenderTargetRegistry::RegisterCreater (const RenderBufferCreater& creater)
{
  impl->render_buffer_creater = creater;
}

void RenderTargetRegistry::RegisterCreater (const ColorBufferCreater& creater)
{
  impl->color_buffer_creater = creater;
}

void RenderTargetRegistry::RegisterCreater (const DepthStencilBufferCreater& creater)
{
  impl->depth_stencil_buffer_creater = creater;
}

/*
    Создание буферов рендеринга
*/

ITexture* RenderTargetRegistry::CreateRenderBuffer (const TextureDesc& desc)
{
  if (!impl->render_buffer_creater)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::RenderTargetRegistry::CreateRenderBuffer", "Render buffers not supported");

  return impl->render_buffer_creater (desc);
}

ITexture* RenderTargetRegistry::CreateColorBuffer (ISwapChain* swap_chain, size_t index)
{
  if (!impl->color_buffer_creater)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::RenderTargetRegistry::CreateColorBuffer", "Color buffers not supported");

  return impl->color_buffer_creater (swap_chain, index);
}

ITexture* RenderTargetRegistry::CreateDepthStencilBuffer (ISwapChain* swap_chain)
{
  if (!impl->color_buffer_creater)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::RenderTargetRegistry::CreateDepthStencilBuffer", "Depth-stencil buffers not supported");

  return impl->depth_stencil_buffer_creater (swap_chain);
}

/*
    Получение буфера кадра
*/

IFrameBuffer& RenderTargetRegistry::GetFrameBuffer (View* render_target_view, View* depth_stencil_view)
{
    //поиск буфера в списке уже созданных

  RenderTargetViews render_target_views (render_target_view, depth_stencil_view);

  FrameBufferMap::iterator iter = impl->frame_buffers.find (render_target_views);
  
  if (iter != impl->frame_buffers.end ())
    return *iter->second->frame_buffer;

    //создание нового буфера кадра

  return *CreateFrameBuffer (render_target_view, depth_stencil_view);
}

/*
    Создание буфера кадра
*/

IFrameBuffer* RenderTargetRegistry::CreateFrameBuffer (View* render_target_view, View* depth_stencil_view)
{
  try
  {
      //проверка корректности конфигурации

    if (render_target_view)
    {
      if (!(render_target_view->GetBindFlags () & BindFlag_RenderTarget))
        throw xtl::format_exception<xtl::bad_argument> ("", "Render-target view has wrong bind flags %s", get_name ((BindFlag)render_target_view->GetBindFlags ()));

        //проверка корректности формата пикселей

      ITexture* texture = render_target_view->GetTexture ();

      TextureDesc desc;
      
      texture->GetDesc (desc);
      
      switch (desc.format)
      {
        case PixelFormat_RGB8:
        case PixelFormat_RGBA8:
        case PixelFormat_L8:
        case PixelFormat_A8:
        case PixelFormat_LA8:
          break;
        case PixelFormat_DXT1:
        case PixelFormat_DXT3:
        case PixelFormat_DXT5:
        case PixelFormat_RGB_PVRTC2:
        case PixelFormat_RGB_PVRTC4:
        case PixelFormat_RGBA_PVRTC2:
        case PixelFormat_RGBA_PVRTC4:
        case PixelFormat_D16:
        case PixelFormat_D24X8:
        case PixelFormat_D24S8:
        case PixelFormat_S8:
          throw xtl::format_not_supported_exception ("", "Unsupported render-target view texture format=%s", get_name (desc.format));
        default:
          throw xtl::make_argument_exception ("", "texture_desc.format", desc.format);
      }
    }

    if (depth_stencil_view)
    {
      if (!(depth_stencil_view->GetBindFlags () & BindFlag_DepthStencil))
        throw xtl::format_exception<xtl::bad_argument> ("", "Depth-stencil view has wrong bind flags %s", get_name ((BindFlag)depth_stencil_view->GetBindFlags ()));

        //проверка корректности формата пикселей

      ITexture* texture = depth_stencil_view->GetTexture ();

      TextureDesc desc;

      texture->GetDesc (desc);

      switch (desc.format)
      {
        case PixelFormat_RGB8:
        case PixelFormat_RGBA8:
        case PixelFormat_L8:
        case PixelFormat_A8:
        case PixelFormat_LA8:
        case PixelFormat_DXT1:
        case PixelFormat_DXT3:
        case PixelFormat_DXT5:
        case PixelFormat_RGB_PVRTC2:
        case PixelFormat_RGB_PVRTC4:
        case PixelFormat_RGBA_PVRTC2:
        case PixelFormat_RGBA_PVRTC4:
          throw xtl::format_not_supported_exception ("", "Unsupported depth-stencil view texture format=%s", get_name (desc.format));
        case PixelFormat_D16:
        case PixelFormat_D24X8:
        case PixelFormat_D24S8:
        case PixelFormat_S8:
          break;
        default:
          throw xtl::make_argument_exception ("", "texture_desc.format", desc.format);
      }
    }
    
      //поиск фабрики, создающей буфер кадра указанной конфигурации

    for (FrameBufferFactoryList::iterator iter=impl->frame_buffer_factories.begin (), end=impl->frame_buffer_factories.end (); iter!=end; ++iter)
    {
        //провера возможности создания буфера
      
      if (!iter->checker (render_target_view, depth_stencil_view))
        continue;
      
      try
      {
          //создание буфера

        FrameBufferPtr frame_buffer (iter->creater (render_target_view, depth_stencil_view));

        FrameBufferHolderPtr holder (new FrameBufferHolder (frame_buffer), false);
        
          //регистрация буфера

        impl->frame_buffers.insert_pair (RenderTargetViews (render_target_view, depth_stencil_view), holder);

        return &*holder->frame_buffer;
      }
      catch (xtl::exception& exception)
      {
        stl::string cfg_name;

        get_configuration_name (render_target_view, depth_stencil_view, cfg_name);

        exception.touch ("at create frame_buffer (%s)", cfg_name.c_str ());

        throw;      
      }
    }

      //буфер не может быть создан, поскольку заданная конфигурация не поддерживается

    stl::string cfg_name;

    get_configuration_name (render_target_view, depth_stencil_view, cfg_name);    

    throw xtl::format_not_supported_exception ("", "Frame buffer configuration not supported. %s", cfg_name.c_str ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::RenderTargetRegistry::CreateFrameBuffer");
    throw;
  }
}

/*
    Освобождение ресурсов, связанных с отображением
*/

void RenderTargetRegistry::ReleaseResources (View* view)
{
    //удаление всех буферов кадра, в которых присутствует указанное отображение

  for (FrameBufferMap::iterator iter=impl->frame_buffers.begin (), end=impl->frame_buffers.end (); iter!=end;)
  {
    if (iter->first.HasView (view))
    {
      FrameBufferMap::iterator next = iter;

      ++next;

      impl->frame_buffers.erase (iter);

      iter = next;
    }
    else ++iter;
  }
}
