#include "shared.h"

using namespace render;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const size_t RENDER_PATHS_STRING_RESERVE      = 256; //резервируемый размер строки с имёна путей рендеринга
const size_t FRAME_BUFFER_ARRAY_RESERVE_SIZE  = 8;   //резервируемое число буферов кадра
const size_t RENDER_TARGET_ARRAY_RESERVE_SIZE = 32;  //резервируемое число целей рендеринга

/*
    Путь рендеринга
*/

struct RenderPath
{
  CustomSceneRenderPtr render;  
  stl::string          name;

  RenderPath (const CustomSceneRenderPtr& in_render, const char* in_name) : render (in_render), name (in_name) {}
};

/*
    Буфер кадра
*/

struct FrameBuffer: public xtl::reference_counter
{
  mid_level::IFrameBuffer* frame_buffer;  //указаель на буфер кадра системы рендеринга  
  RenderTargetImpl::Pointer    render_target; //указаель на цель рендеринга

  FrameBuffer (mid_level::IFrameBuffer* in_frame_buffer, RenderManager& manager)
    : frame_buffer (in_frame_buffer),
      render_target (RenderTargetImpl::Create (manager, frame_buffer->GetColorBuffer (), frame_buffer->GetDepthStencilBuffer ()))
  { }
};

}

/*
    Описание реализации менеджера системы рендеринга
*/

class RenderManager::Impl: private mid_level::IRendererListener
{
  public:
///Конструктор
    Impl
     (RenderManager&       in_owner,
      const char*          driver_name_mask,
      const char*          renderer_name_mask,
      const char*          render_path_masks,
      const LogFunction&   in_log_handler,
      const QueryFunction& query_handler)
        : owner (in_owner),
          log_handler (in_log_handler)
    {
      try
      {
        if (!driver_name_mask)
          throw xtl::make_null_argument_exception ("", "driver_name_mask");
          
        if (!renderer_name_mask)
          throw xtl::make_null_argument_exception ("", "renderer_name_mask");
          
        if (!render_path_masks)
          throw xtl::make_null_argument_exception ("", "render_path_masks");      
          
          //создание системы ренедринга      

        renderer = RendererPtr (mid_level::DriverManager::CreateRenderer (driver_name_mask, renderer_name_mask), false);

          //построение множества имён путей рендеринга
          
        typedef stl::vector<stl::string>   StringArray;
        typedef stl::hash_set<stl::string> StringSet;
          
        StringSet                    path_names;
        StringArray                  path_masks        = common::split (render_path_masks);
        SceneRenderManager::Iterator render_path_begin = SceneRenderManager::CreateIterator ();

        for (StringArray::iterator iter=path_masks.begin (), end=path_masks.end (); iter!=end; ++iter)
        {
          const char* render_path_mask = iter->c_str ();      
          
          if (!strchr (render_path_mask, '*') && !strchr (render_path_mask, '?'))
          {
            path_names.insert (render_path_mask);
          }
          else
          {
            for (SceneRenderManager::Iterator render_path_iter=render_path_begin; render_path_iter; ++render_path_iter)
            {
              if (common::wcmatch (render_path_iter->Name (), render_path_mask))
                path_names.insert (render_path_iter->Name ());
            }
          }
        }

          //создание путей рендеринга

        render_paths_string.reserve (RENDER_PATHS_STRING_RESERVE);
        
        for (StringSet::iterator iter=path_names.begin (), end=path_names.end (); iter!=end; ++iter)
        {
          const char* path_name = iter->c_str ();

            //создание пути рендеринга
            
          CustomSceneRenderPtr render_path = create_render_path (renderer.get (), path_name);
          
            //установка функций обратного вызова

          render_path->SetLogHandler   (log_handler);
          render_path->SetQueryHandler (query_handler);

            //регистрация пути рендеринга
            
          render_paths.insert_pair (path_name, RenderPath (render_path, path_name));

            //модификация строки имён путей рендеринга

          if (!render_paths_string.empty ())
            render_paths_string += ' ';

          render_paths_string += path_name;
        }

          //создание менеджера транзакций отрисовки

        draw_transaction_manager = new render::DrawTransactionManager (*renderer, log_handler);
        
          //резервирование места для массива целей рендеринга

        render_targets.reserve (RENDER_TARGET_ARRAY_RESERVE_SIZE);

          //резервирование места для массива буферов кадра
          
        frame_buffers.reserve (FRAME_BUFFER_ARRAY_RESERVE_SIZE);
        
          //регистрация слушателя системы рендеринга
         
        renderer->AttachListener (this);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderManager::Impl::Impl");
        throw;
      }      
    }

///Деструктор
    ~Impl ()
    {
        //отмена регистрации слушателя системы рендеринга
        
      renderer->DetachListener (this);
      
        //сброс менеджера рендеринга в целях рендеринга

      for (RenderTargetList::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
        (*iter)->SetRenderManager (0);
    }

///Получение системы рендеринга
    mid_level::IRenderer& Renderer () { return *renderer; }
    
///Список доступных путей рендеринга
    const char* RenderPaths () { return render_paths_string.c_str (); }

///Получение менеджера транзакций отрисовки
    render::DrawTransactionManager& DrawTransactionManager () { return *draw_transaction_manager; }

///Проверка наличия пути рендеринга
    bool HasRenderPath (const char* path_name)
    {
      if (!path_name)
        return false;

      RenderPathMap::const_iterator iter = render_paths.find (path_name);

      return iter != render_paths.end ();
    }    

///Получение пути рендеринга
    ICustomSceneRender& GetRenderPath (const char* path_name)
    {
      static const char* METHOD_NAME = "render::RenderManager::Impl::GetRenderPath";

      if (!path_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path_name");

      RenderPathMap::const_iterator iter = render_paths.find (path_name);

      if (iter == render_paths.end ())
        throw xtl::format_not_supported_exception (METHOD_NAME, "Unknown path '%s'", path_name); //путь не найден

      return *iter->second.render;
    }
    
///Количество целей рендеринга
    size_t RenderTargetsCount () { return render_targets.size (); }
    
///Получение цели рендеринга
    RenderTargetImpl& RenderTarget (size_t index)
    {
      if (index >= render_targets.size ())
        throw xtl::make_range_exception ("render::RenderManager::Impl::RenderTarget", "index", index, render_targets.size ());
        
      return *render_targets [index];
    }
    
///Регистрация цели рендеринга
    void RegisterRenderTarget (RenderTargetImpl* render_target)
    {
      if (!render_target)
        return;

      render_targets.push_back (render_target);
    }

///Отмена регистрации
    void UnregisterRenderTarget (RenderTargetImpl* render_target)
    {
      render_targets.erase (stl::remove (render_targets.begin (), render_targets.end (), render_target), render_targets.end ());
    }
    
///Регистрация буфера кадра
    void RegisterFrameBuffer (mid_level::IFrameBuffer* in_frame_buffer)
    {
        //создание буфера кадра
        
      FrameBufferPtr frame_buffer (new FrameBuffer (in_frame_buffer, owner), false);

        //добавление буфера кадра

      frame_buffers.push_back (frame_buffer);      
    }

///Загрузка ресурса
    void LoadResource (const char* tag, const char* file_name)
    {
      static const char* METHOD_NAME = "render::RenderManager::Impl::LoadResource";

      if (!tag)
        throw xtl::make_null_argument_exception (METHOD_NAME, "tag");

      if (!file_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

      for (RenderPathMap::iterator iter=render_paths.begin (), end=render_paths.end (); iter!=end; ++iter)
      {
        RenderPath& render_path = iter->second;

        try
        {
          render_path.render->LoadResource (tag, file_name);
        }
        catch (std::exception& exception)
        {
          log_printf (log_handler, "%s\n    at load resource (file_name='%s', tag='%s', render_path='%s', renderer='%s')"
            "\n    at %s", exception.what (), file_name, tag, render_path.name.c_str (), renderer->GetDescription (), METHOD_NAME);
        }
        catch (...)
        {
          log_printf (log_handler, "Unknown exception\n    at SceneRender::LoadResource (file_name='%s', tag='%s', render_path='%s', renderer='%s')"
            "\n    at %s", file_name, tag, render_path.name.c_str (), renderer->GetDescription (), METHOD_NAME);
        }
      }
    }
    
///Протоколирование
    void LogVPrintf (const char* format, va_list list)
    {
      if (!format)
        return;
        
      try
      {
        log_handler (common::vformat (format, list).c_str ());
      }
      catch (...)
      {
        ///подавление всех исключений
      }
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, RenderPath> RenderPathMap;
    typedef xtl::com_ptr<mid_level::IRenderer>                    RendererPtr;
    typedef stl::vector<render::RenderTargetImpl*>                RenderTargetList;
    typedef xtl::intrusive_ptr<FrameBuffer>                       FrameBufferPtr;
    typedef stl::vector<FrameBufferPtr>                           FrameBufferArray;
    typedef stl::auto_ptr<render::DrawTransactionManager>         DrawTransactionManagerPtr;

  private:
///Получение буфера кадра
    FrameBuffer& GetFrameBuffer (mid_level::IFrameBuffer* frame_buffer)
    {
      for (FrameBufferArray::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
        if ((*iter)->frame_buffer == frame_buffer)
          return **iter;

      throw xtl::format_operation_exception ("render::RenderManager::Impl::GetFrameBuffer", "Invalid frame buffer");
    }
  
///Создан буфер кадра
    void OnFrameBufferCreate (mid_level::IFrameBuffer* frame_buffer)
    {
      try
      {
        RegisterFrameBuffer (frame_buffer);
      }
      catch (std::exception& exception)
      {
        log_printf (log_handler, "%s\n    at render::RenderManager::Impl::OnFrameBufferCreate", exception.what ());
      }
      catch (...)
      {
        log_printf (log_handler, "Unknown exception\n    at render::RenderManager::Impl::OnFrameBufferCreate");
      }
    }

///Буфер кадра удалён
    void OnFrameBufferDestroy (mid_level::IFrameBuffer* frame_buffer)
    {
      for (FrameBufferArray::iterator iter=frame_buffers.begin (); iter!=frame_buffers.end ();)
        if ((*iter)->frame_buffer == frame_buffer) frame_buffers.erase (iter);
        else                                       ++iter;
    }

///Буфер кадра необходимо обновить
    void OnFrameBufferUpdate (mid_level::IFrameBuffer* in_frame_buffer)
    {
      try
      {
        FrameBuffer& frame_buffer = GetFrameBuffer (in_frame_buffer);

        frame_buffer.render_target->Update ();
      }
      catch (std::exception& exception)
      {
        log_printf (log_handler, "%s\n    at render::RenderManager::Impl::OnFrameBufferCreate", exception.what ());
      }
      catch (...)
      {
        log_printf (log_handler, "Unknown exception\n    at render::RenderManager::Impl::OnFrameBufferCreate");
      }
    }

///Размеры буфера кадра изменились
    void OnFrameBufferResize (mid_level::IFrameBuffer* in_frame_buffer, size_t width, size_t height)
    {
      try
      {
        FrameBuffer& frame_buffer = GetFrameBuffer (in_frame_buffer);

        frame_buffer.render_target->SetRenderableArea (Rect (0, 0, width, height));
      }
      catch (std::exception& exception)
      {
        log_printf (log_handler, "%s\n    at render::RenderManager::Impl::OnFrameBufferCreate", exception.what ());
      }
      catch (...)
      {
        log_printf (log_handler, "Unknown exception\n    at render::RenderManager::Impl::OnFrameBufferCreate");
      }
    }

  private:
    RenderManager&             owner;                    //владелец
    RendererPtr                renderer;                 //система рендеринга
    RenderPathMap              render_paths;             //пути рендеринга
    stl::string                render_paths_string;      //строка с именами доступных путей рендеринга
    FrameBufferArray           frame_buffers;            //буферы кадра
    RenderTargetList           render_targets;           //цели рендеринга
    SceneRender::LogFunction   log_handler;              //функция протоколирования    
    DrawTransactionManagerPtr  draw_transaction_manager; //менеджер отрисовки
};

/*
    Конструкторы / деструктор / присваивание
*/

RenderManager::RenderManager
 (const char*          driver_name_mask,
  const char*          renderer_name_mask,
  const char*          render_path_masks,
  const LogFunction&   log_handler,
  const QueryFunction& query_handler)
{
  try
  {
      //создание реализации

    impl = new Impl (*this, driver_name_mask, renderer_name_mask, render_path_masks, log_handler, query_handler);

      //регистрация буферов кадра
      
    mid_level::IRenderer& renderer = impl->Renderer ();    

    for (size_t i=0, count=renderer.GetFrameBuffersCount (); i<count; i++)
      impl->RegisterFrameBuffer (renderer.GetFrameBuffer (i));      
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderManager::RenderManager");
    throw;
  }
}

RenderManager::~RenderManager ()
{
}

/*
    Система рендеринга
*/

mid_level::IRenderer& RenderManager::Renderer ()
{
  return impl->Renderer ();
}

/*
    Список доступных путей рендеринга
*/

const char* RenderManager::RenderPaths ()
{
  return impl->RenderPaths ();
}

/*
    Проверка наличия пути рендеринга / получение пути рендеринга
*/

bool RenderManager::HasRenderPath (const char* path_name)
{
  return impl->HasRenderPath (path_name);
}

ICustomSceneRender& RenderManager::GetRenderPath (const char* path_name)
{
  return impl->GetRenderPath (path_name);
}

/*
    Получение менеджера транзакций отрисовки
*/

DrawTransactionManager& RenderManager::DrawTransactionManager ()
{
  return impl->DrawTransactionManager ();
}

/*
    Загрузка ресурса
*/

void RenderManager::LoadResource (const char* tag, const char* file_name)
{
  impl->LoadResource (tag, file_name);
}

/*
    Протоколирование
*/

void RenderManager::LogPrintf (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  

  impl->LogVPrintf (format, list);
}

/*
    Перебор целей рендеринга
*/

size_t RenderManager::RenderTargetsCount ()
{
  return impl->RenderTargetsCount ();
}

RenderTargetImpl& RenderManager::RenderTarget (size_t index)
{
  return impl->RenderTarget (index);
}

/*
    Регистрация целей рендеринга
*/

void RenderManager::RegisterRenderTarget (RenderTargetImpl* render_target)
{
  impl->RegisterRenderTarget (render_target);
}

void RenderManager::UnregisterRenderTarget (RenderTargetImpl* render_target)
{
  impl->UnregisterRenderTarget (render_target);
}
