#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Исключение: внутренняя ошибка OpenGL
*/

struct OpenGLExceptionTag;

typedef DerivedException<Exception, OpenGLExceptionTag> OpenGLException;

/*
    Описание реализации контекста OpenGL
*/

typedef size_t ContextData [ContextDataTable_Num][CONTEXT_DATA_TABLE_SIZE];

struct ContextImpl: public xtl::reference_counter
{
  Context     context; //контекст OpenGL
  ContextData data;    //локальные данные контекста 

  ContextImpl (ISwapChain* swap_chain) : context (swap_chain)
  {
    memset (data, 0, sizeof (data));
  }

  ContextImpl (ISwapChain* swap_chain, const Context& shared_context) : context (swap_chain, shared_context)
  {
    memset (data, 0, sizeof (data));
  }  
};

typedef xtl::intrusive_ptr<ContextImpl> ContextImplPtr;  //указатель на реализацию контекста

struct ContextBind
{
  ContextImplPtr          context_impl;          //указатель на реализацию контекста
  xtl::auto_slot<void ()> on_destroy_swap_chain; //слот обработчика события удаления соединения
  
  ContextBind (const ContextImplPtr& in_context_impl, xtl::slot<void ()>& destroy_handler) :
    context_impl (in_context_impl), on_destroy_swap_chain (destroy_handler) {}
};

typedef stl::hash_map<ISwapChain*, ContextBind> ContextMap;   //карта соответствия контекста OpenGL и цепочки обмена
typedef stl::vector<ContextImpl*>               ContextArray; //массив контекстов

}

/*
    Описание реализации ContextManager
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  LogHandler   log_handler;      //обработчик протоколирования
  ContextArray contexts;         //контексты
  ContextMap   context_map;      //карта отображения цепочки обмена на контекст
  ContextImpl* selected_context; //выбранный контекст
  ISwapChain*  draw_swap_chain;  //цепочка обмена для рисования
  ISwapChain*  read_swap_chain;  //цепочка обмена для чтения
  
  Impl (const LogHandler& in_log_handler) : log_handler (in_log_handler), selected_context (0), draw_swap_chain (0), read_swap_chain (0) {}
  
  ContextImpl* GetContext (ISwapChain* swap_chain, bool create_new_context=true)
  {
    ContextMap::iterator iter = context_map.find (swap_chain);
    
      //проверка: есть ли контекст соответствующий данной цепочке обмена
    
    if (iter != context_map.end ())
      return &*iter->second.context_impl;

    Trackable* trackable = cast_object<Trackable> (swap_chain, "render::low_level::opengl::ContextManager::Impl", "swap_chain");

    Trackable::DestroyHandler destroy_handler (xtl::bind (&Impl::OnDestroySwapChain, this, swap_chain));    

      //попытка найти контекст для данной цепочки обмена

    for (ContextArray::iterator i=contexts.begin (), end=contexts.end (); i!=end; ++i)
      if ((*i)->context.IsCompatible (swap_chain))
      {
        context_map.insert_pair (swap_chain, ContextBind (*i, destroy_handler));
        
        trackable->RegisterDestroyHandler (destroy_handler);

        return *i;
      }
      
    if (!create_new_context)
      return 0;
      
      //создание нового контекста
      
    ContextImplPtr new_context;
    
    if (contexts.empty ())
    {
      new_context = ContextImplPtr (new ContextImpl (swap_chain), false);
    }
    else
    {
      new_context = ContextImplPtr (new ContextImpl (swap_chain, contexts.front ()->context), false);
    }

    contexts.push_back (get_pointer (new_context));
    
    try
    {
      context_map.insert_pair (swap_chain, ContextBind (new_context, destroy_handler));

      trackable->RegisterDestroyHandler (destroy_handler);
    }
    catch (...)
    {
      contexts.pop_back ();
      throw;
    }    

    return get_pointer (new_context);
  }
  
  ContextImpl* GetContext (ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
  {
    ContextImpl *draw_context = GetContext (draw_swap_chain),
                *read_context = GetContext (read_swap_chain, false);

    if (draw_context != read_context)
      RaiseInvalidOperation ("render::low_level::opengl::ContextManager::Impl::GetContext", "Swap chains are incompatible");

    return draw_context;
  }
  
  void OnDestroySwapChain (ISwapChain* swap_chain)
  {
    ContextMap::iterator iter = context_map.find (swap_chain);

    if (iter == context_map.end ())
      return;

    ContextImpl* context = &*iter->second.context_impl;
    bool         need_context_delete = false;
    
    if (context->use_count () == 1)
      need_context_delete = true;
    
      //удаление ассоциации цепочки обмена и контекста
    
    context_map.erase (iter);    
    
      //удаление последней ссылки на контекст - удаление контекста

    if (need_context_delete)
    {
      contexts.erase (stl::remove (contexts.begin (), contexts.end (), context), contexts.end ());

      if (selected_context == context)
        selected_context = 0;
    }

      //очистка ссылок

    if (swap_chain == draw_swap_chain)
      draw_swap_chain = 0;

    if (swap_chain == read_swap_chain)
      read_swap_chain = 0;
      
      //восстановление контекста
      
    if (!selected_context)
    {
      if (contexts.empty ())
        return; //восстановление невозможно
      
      selected_context = contexts.front ();
    }

    if (!draw_swap_chain)
    {
      for (ContextMap::iterator iter=context_map.begin (), end=context_map.end (); iter!=end; ++iter)
        if (iter->second.context_impl == selected_context)
        {
          draw_swap_chain = iter->first;
          break;
        }
    }

    if (!read_swap_chain)
      read_swap_chain = draw_swap_chain;
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

ContextManager::ContextManager (const LogHandler& log_handler)
  : impl (new Impl (log_handler))
{
}

ContextManager::ContextManager (const ContextManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ContextManager::~ContextManager ()
{
  release (impl);
}

ContextManager& ContextManager::operator = (const ContextManager& manager)
{  
  addref (manager.impl);
  release (impl);

  impl = manager.impl;

  return *this;
}

/*
    Выбор активных цепочек обмена
*/

void ContextManager::SetSwapChains (ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
{
  if (draw_swap_chain == impl->draw_swap_chain && read_swap_chain == impl->read_swap_chain)
    return;

  if (!draw_swap_chain)
    RaiseNullArgument ("render::low_level::opengl::ContextManager::SetSwapChains", "draw_swap_chain");

  if (!read_swap_chain)
    RaiseNullArgument ("render::low_level::opengl::ContextManager::SetSwapChains", "read_swap_chain");
    
  try
  {
    ContextImpl* context = impl->GetContext (draw_swap_chain, read_swap_chain);
  
    impl->draw_swap_chain  = draw_swap_chain;
    impl->read_swap_chain  = read_swap_chain;
    impl->selected_context = context;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::ContextManager::SetSwapChains");
    
    throw;
  }
}

ISwapChain* ContextManager::GetDrawSwapChain () const
{
  return impl->draw_swap_chain;
}

ISwapChain* ContextManager::GetReadSwapChain () const
{
  return impl->read_swap_chain;
}

/*
    Активация текущего контекста
*/

void ContextManager::MakeContextCurrent () const
{
  if (!impl->selected_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::MakeContextCurrent", "Null active context");

  try
  {
    impl->selected_context->context.MakeCurrent (impl->draw_swap_chain, impl->read_swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::ContextManager::MakeContextCurrent");
    
    throw;
  }
}

bool ContextManager::IsContextCurrent () const
{
  if (!impl->selected_context)
    return false;

  return impl->selected_context->context.IsCurrent (impl->draw_swap_chain, impl->read_swap_chain);
}

/*
    Работа с таблицей локальных данных контекста
*/

void ContextManager::SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
{
  if (!impl->selected_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::SetContextData", "Null active context");
    
  if (table_id < 0 || table_id >= ContextDataTable_Num)
    RaiseInvalidArgument ("render::low_level::opengl::ContextManager::SetContextData", "table_id", table_id);
    
  if (element_id >= CONTEXT_DATA_TABLE_SIZE)
    RaiseOutOfRange ("render::low_level::opengl::ContextManager::SetContextData", "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);
    
  impl->selected_context->data [table_id][element_id] = value;
}

size_t ContextManager::GetContextData (ContextDataTable table_id, size_t element_id) const
{
  if (!impl->selected_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetContextData", "Null active context");
    
  if (table_id < 0 || table_id >= ContextDataTable_Num)
    RaiseInvalidArgument ("render::low_level::opengl::ContextManager::GetContextData", "table_id", table_id);
    
  if (element_id >= CONTEXT_DATA_TABLE_SIZE)
    RaiseOutOfRange ("render::low_level::opengl::ContextManager::GetContextData", "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);
    
  return impl->selected_context->data [table_id][element_id];
}

void ContextManager::ClearContextData (ContextDataTable table_id)
{
  if (!impl->selected_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");
    
  if (table_id < 0 || table_id >= ContextDataTable_Num)
    RaiseInvalidArgument ("render::low_level::opengl::ContextManager::ClearContextData", "table_id", table_id);
    
  memset (impl->selected_context->data [table_id], 0, sizeof (impl->selected_context->data [table_id]));
}

void ContextManager::ClearContextData ()
{
  if (!impl->selected_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");

  memset (impl->selected_context->data, 0, sizeof (impl->selected_context->data));
}

/*
    Получение строки со списком расширений, зависящих от активной цепочки обмена
*/

const char* ContextManager::GetSwapChainsExtensionString () const
{
  MakeContextCurrent ();

  return Context::GetSwapChainExtensionString ();
}

/*
    Проверка совместимости менеджеров контекстов
*/

bool ContextManager::IsCompatible (const ContextManager& manager) const
{
  return impl == manager.impl;
}

/*
    Протоколирование
*/

void ContextManager::LogPrintf (const char* format, ...) const
{
  va_list list;
  
  va_start   (list, format);  
  LogVPrintf (format, list);
}

void ContextManager::LogVPrintf (const char* format, va_list args) const
{
  if (!format)
    return;

  try
  {
    impl->log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Проверка ошибок OpenGL
*/

void ContextManager::CheckErrors (const char* source) const
{
  if (!source)
    source = "render::low_level::ContextManager::CheckErrors";
    
  MakeContextCurrent ();
  
  GLenum error = glGetError ();
  
  switch (error)
  {
    case GL_NO_ERROR:
      break;
    case GL_INVALID_ENUM:
      Raise<OpenGLException> (source, "OpenGL error: invalid enum");
      break;
    case GL_INVALID_VALUE:
      Raise<OpenGLException> (source, "OpenGL error: invalid value");
      break;
    case GL_INVALID_OPERATION:
      Raise<OpenGLException> (source, "OpenGL error: invalid operation");
      break;
    case GL_STACK_OVERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack overflow");
      break;
    case GL_STACK_UNDERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack underflow");
      break;
    case GL_OUT_OF_MEMORY:
      Raise<OpenGLException> (source, "OpenGL error: out of memory");
      break;
    default:
      Raise<OpenGLException> (source, "OpenGL error: code=%d", error);
      break;
  }
}

void ContextManager::RaiseError (const char* source) const
{
  if (!source)
    return;
    
  CheckErrors (source);
  
  RaiseInvalidOperation (source, "Invalid operation");
}
