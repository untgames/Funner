#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Описание реализации менеджера буферов кадра
*/

struct FrameBufferFactory
{
  FrameBufferChecker checker; //функтор, проверяющий возможность создания буфера кадра из двух отображений
  FrameBufferCreater creater; //функтор, создающий буфер кадра по двум отображениям
  
  FrameBufferFactory (const FrameBufferChecker& in_checker, const FrameBufferCreater& in_creater) :
    checker (in_checker), creater (in_creater) {}
};

typedef xtl::com_ptr<ISwapChain>      SwapChainPtr;
typedef stl::list<FrameBufferFactory> FrameBufferFactoryList;

struct FrameBufferManager::Impl: public ContextObject
{
  SwapChainPtr              default_swap_chain;             //цепочка обмена по умолчанию
  FrameBufferFactoryList    frame_buffer_factories;         //список фабрик буферов кадра
  RenderBufferCreater       render_buffer_creater;          //функтор создания буфера рендеринга
  ColorBufferCreater        color_buffer_creater;           //функтор создания буфера цвета
  DepthStencilBufferCreater depth_stencil_buffer_creater;   //функтор создания буфера попиксельного отсечения
  bool                      is_active_color_buffer;         //активен ли буфер цвета
  bool                      is_active_depth_stencil_buffer; //активен ли буфера попиксельного отсечения

  Impl (const ContextManager& manager, ISwapChain* in_default_swap_chain) :
    ContextObject (manager), default_swap_chain (in_default_swap_chain), is_active_color_buffer (false), is_active_depth_stencil_buffer (false) {}
};

namespace
{

/*
    Получение строки с конфигурацией буфера кадра
*/

void get_configuration_name (View* color_view, View* depth_stencil_view, stl::string& out_name)
{
  format ("color-view=%s, depth-stencil-view=%s", color_view ? color_view->GetTextureTypeName () : "null",
          depth_stencil_view ? depth_stencil_view->GetTextureTypeName () : "null").swap (out_name);
}

}

/*
    Конструктор / деструктор
*/

FrameBufferManager::FrameBufferManager (const ContextManager& context_manager, ISwapChain* default_swap_chain)
  : impl (new Impl (context_manager, default_swap_chain), false)
{
}

FrameBufferManager::FrameBufferManager (const FrameBufferManager& manager)
  : impl (manager.impl)
{
}

FrameBufferManager::~FrameBufferManager ()
{
}

/*
    Получение менеджера контекстов
*/

const ContextManager& FrameBufferManager::GetContextManager () const
{
  return impl->GetContextManager ();
}

ContextManager& FrameBufferManager::GetContextManager ()
{
  return impl->GetContextManager ();
}

/*
    Получение цепочки обмена по умолчанию
*/

ISwapChain* FrameBufferManager::GetDefaultSwapChain () const
{
  return impl->default_swap_chain.get ();
}

/*
    Регистрация обработчиков создания буферов кадра / буферов отрисовки
*/

void FrameBufferManager::RegisterCreater  (const FrameBufferChecker& checker, const FrameBufferCreater& creater)
{
  impl->frame_buffer_factories.push_front (FrameBufferFactory (checker, creater));
}

void FrameBufferManager::RegisterCreater (const RenderBufferCreater& creater)
{
  impl->render_buffer_creater = creater;
}

void FrameBufferManager::RegisterCreater (const ColorBufferCreater& creater)
{
  impl->color_buffer_creater = creater;
}

void FrameBufferManager::RegisterCreater (const DepthStencilBufferCreater& creater)
{
  impl->depth_stencil_buffer_creater = creater;
}

void FrameBufferManager::UnregisterAllCreaters ()
{
  impl->frame_buffer_factories.clear ();
  impl->render_buffer_creater.clear ();
  impl->color_buffer_creater.clear ();
  impl->depth_stencil_buffer_creater.clear ();
}

/*
    Создание буфера кадра / буфера рендеринга
*/

IFrameBuffer* FrameBufferManager::CreateFrameBuffer (View* color_view, View* depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::FrameBufferManager::CreateFrameBuffer";

    //поиск фабрики, создающей буфер кадра указанной конфигурации

  for (FrameBufferFactoryList::iterator iter=impl->frame_buffer_factories.begin (), end=impl->frame_buffer_factories.end (); iter!=end; ++iter)
  {
    try
    {    
        //попытка создания буфера кадра

      if (iter->checker (color_view, depth_stencil_view))
        return iter->creater (color_view, depth_stencil_view);
    }
    catch (common::Exception& exception)
    {
      stl::string cfg_name;

      get_configuration_name (color_view, depth_stencil_view, cfg_name);

      exception.Touch ("%s(%s)", METHOD_NAME, cfg_name.c_str ());

      throw;      
    }
  }

    //буфер не может быть создан, поскольку заданная конфигурация не поддерживается

  stl::string cfg_name;

  get_configuration_name (color_view, depth_stencil_view, cfg_name);    

  RaiseNotSupported (METHOD_NAME, "Frame buffer configuration not supported. %s", cfg_name.c_str ());

  return 0;
}

ITexture* FrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  if (!impl->render_buffer_creater)
    RaiseNotSupported ("render::low_level::opengl::FrameBufferManager::CreateRenderBuffer", "Render buffers not supported");

  return impl->render_buffer_creater (desc);
}

ITexture* FrameBufferManager::CreateColorBuffer (ISwapChain* swap_chain, size_t index)
{
  if (!impl->color_buffer_creater)
    RaiseNotSupported ("render::low_level::opengl::FrameBufferManager::CreateColorBuffer", "Color buffers not supported");

  return impl->color_buffer_creater (swap_chain, index);
}

ITexture* FrameBufferManager::CreateDepthStencilBuffer (ISwapChain* swap_chain)
{
  if (!impl->color_buffer_creater)
    RaiseNotSupported ("render::low_level::opengl::FrameBufferManager::CreateDepthStencilBuffer", "Depth-stencil buffers not supported");

  return impl->depth_stencil_buffer_creater (swap_chain);
}

/*
    Установка активного буфера кадра
*/

void FrameBufferManager::SetFrameBuffer
 (size_t      context_id,
  ISwapChain* draw_swap_chain,
  GLenum      draw_buffer_type,
  ISwapChain* read_swap_chain,
  GLenum      read_buffer_type)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::SetFrameBuffer(size_t,ISwapChain*,...)";
  
  if (!draw_swap_chain)
    draw_buffer_type = GL_NONE;
    
  if (!read_swap_chain)
    read_buffer_type = GL_NONE;
    
    //установка активного контекста

  ContextManager& context_manager = impl->GetContextManager ();        

  context_manager.SetContext (context_id, draw_swap_chain, read_swap_chain);    

    //проверка необходимости переустановки буфера

  ContextDataTable &state_cache              = context_manager.GetContextDataTable (Stage_Output);
  size_t           &current_draw_buffer_type = state_cache [OutputStageCache_DrawBufferAttachment],
                   &current_read_buffer_type = state_cache [OutputStageCache_ReadBufferAttachment],
                   &current_fbo              = state_cache [OutputStageCache_FrameBufferId];

  if (current_draw_buffer_type == draw_buffer_type && current_read_buffer_type == read_buffer_type && !current_fbo)
    return;

    //установка текущего контекста OpenGL

  context_manager.MakeContextCurrent ();
  
    //установка буфера кадра по умолчанию
    
  if (context_manager.GetCaps().has_ext_framebuffer_object && current_fbo)
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    //установка текущего буфера чтения и отрисовки
    
  if (current_draw_buffer_type != draw_buffer_type)
    glDrawBuffer (draw_buffer_type);
    
  if (current_read_buffer_type != read_buffer_type)
    glReadBuffer (read_buffer_type);    

    //проверка ошибок

  context_manager.CheckErrors (METHOD_NAME);

    //установка значений кэш-переменных  
    
  current_fbo              = 0;
  current_draw_buffer_type = draw_buffer_type;
  current_read_buffer_type = read_buffer_type;
}

void FrameBufferManager::SetFrameBuffer (size_t context_id, ISwapChain* swap_chain, GLenum buffer_type)
{
  SetFrameBuffer (context_id, swap_chain, buffer_type, swap_chain, buffer_type);
}

void FrameBufferManager::SetFrameBuffer (size_t fbo_id, size_t cache_id)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::SetFrameBuffer(size_t)";

    //выбор активного контекста
    
  ContextManager& context_manager = impl->GetContextManager ();
  
    //проверка необходимости переустановки буфера

  ContextDataTable &state_cache = context_manager.GetContextDataTable (Stage_Output);
  size_t           &current_id  = state_cache [OutputStageCache_FrameBufferId];
  
  if (current_id == cache_id)
    return;

    //установка текущего контекста  

  context_manager.MakeContextCurrent ();

    //проверка наличия необходимого расширения

  if (!context_manager.GetCaps ().has_ext_framebuffer_object)
    RaiseNotSupported (METHOD_NAME, "GL_EXT_framebuffer_object not supported");

    //установка буфера в контекст OpenGL

  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo_id);

    //проверка ошибок

  context_manager.CheckErrors (METHOD_NAME);

    //установка значений кэш-переменных

  current_id = cache_id;
}

/*
    Проверки наличия активных буферов цвета и попиксельного отсечения
*/

void FrameBufferManager::SetFrameBufferActivity (bool color_buffer_state, bool depth_stencil_buffer_state)
{
  impl->is_active_color_buffer         = color_buffer_state;
  impl->is_active_depth_stencil_buffer = depth_stencil_buffer_state;
}

bool FrameBufferManager::IsActiveColorBuffer () const
{
  return impl->is_active_color_buffer;
}

bool FrameBufferManager::IsActiveDepthStencilBuffer () const
{
  return impl->is_active_depth_stencil_buffer;
}
