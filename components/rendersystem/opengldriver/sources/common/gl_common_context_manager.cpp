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

struct ContextImpl;

typedef size_t                                ContextData [ContextDataTable_Num][CONTEXT_DATA_TABLE_SIZE];
typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;   //указатель на цепочку обмена
typedef xtl::intrusive_ptr<ContextImpl>       ContextImplPtr; //указатель на реализацию контекста
typedef stl::hash_map<size_t, ContextImplPtr> ContextMap;     //карта соответствия контекста OpenGL и цепочки обмена

struct ContextImpl: public xtl::reference_counter
{
  Context      context;           //контекст OpenGL
  SwapChainPtr master_swap_chain; //главная цепочка обмена, связанная с контекстом
  ContextData  data;              //локальные данные контекста 

  ContextImpl (ISwapChain* swap_chain) : context (swap_chain), master_swap_chain (swap_chain)
  {
    Init ();
  }

  ContextImpl (ISwapChain* swap_chain, const Context& shared_context) : context (swap_chain, shared_context), master_swap_chain (swap_chain)
  {
    Init ();
  }
  
    //инициализация
  void Init ()
  {
    memset (data, 0, sizeof (data));
  }
  
    //установка локальных данных
  void SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::SetContextData";
    
    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);
      
    if (element_id >= CONTEXT_DATA_TABLE_SIZE)
      RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);
      
    data [table_id][element_id] = value;
  }

    //чтение локальных данных
  size_t GetContextData (ContextDataTable table_id, size_t element_id)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::GetContextData";

    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);

    if (element_id >= CONTEXT_DATA_TABLE_SIZE)
      RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);

    return data [table_id][element_id];
  }
  
    //очистка таблицы локальных данных
  void ClearContextData (ContextDataTable table_id)
  {
    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument ("render::low_level::opengl::ContextManager::ClearContextData", "table_id", table_id);

    memset (data [table_id], 0, sizeof (data [table_id]));
  }
  
    //очистка всех таблиц локальных данных
  void ClearContextData ()
  {
    memset (data, 0, sizeof (data));
  }
};

}

/*
    Описание реализации ContextManager
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  LogHandler          log_handler;                //обработчик протоколирования
  ContextMap          context_map;                //карта отображения цепочки обмена на контекст
  ContextImpl*        current_context;            //текущий контекст
  ISwapChain*         current_draw_swap_chain;    //цепочка обмена для рисования
  ISwapChain*         current_read_swap_chain;    //цепочка обмена для чтения
  size_t              current_context_id;         //идентификатор текущего контекста (0 - отсутствует)
  size_t              next_context_id;            //номер следующего создаваемого контекста
  Trackable::SlotType on_destroy_draw_swap_chain; //обработчик удаления цепочки обмена для рисования 
  Trackable::SlotType on_destroy_read_swap_chain; //обработчик удаления цепочки обмена для чтения
  
  Impl (const LogHandler& in_log_handler) :
    log_handler (in_log_handler),
    current_context (0),
    current_draw_swap_chain (0),
    current_read_swap_chain (0),
    current_context_id (0),
    next_context_id (1),
    on_destroy_draw_swap_chain (xtl::bind (&Impl::RestoreContext, this)),
    on_destroy_read_swap_chain (xtl::bind (&Impl::RestoreContext, this))
  {}
 
    //создание контекста
  size_t CreateContext (ISwapChain* swap_chain)
  {
    if (!swap_chain)
      RaiseNullArgument ("render::low_level::opengl::ContextManager::CreateContext", "swap_chain");

    ContextImplPtr new_context;

    if (context_map.empty ()) new_context = ContextImplPtr (new ContextImpl (swap_chain), false);
    else                      new_context = ContextImplPtr (new ContextImpl (swap_chain, context_map.begin ()->second->context), false);

    size_t new_context_id = next_context_id;

    context_map.insert_pair (new_context_id, new_context);

    next_context_id++;

    return new_context_id;
  }
  
    //удаление контекста
  void DeleteContext (size_t context_id)
  {
    context_map.erase (context_id);

    if (context_id == current_context_id)
      RestoreContext ();
  }
  
    //установка текущего контекста
  void SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::SetContext";
    
    if (context_id == current_context_id && draw_swap_chain == current_draw_swap_chain && read_swap_chain == current_read_swap_chain)
      return;      

      //поиск контекста
      
    ContextMap::iterator iter = context_map.find (context_id);
    
    if (iter == context_map.end ())
      RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

    ContextImplPtr context = iter->second;

      //проверка совместимости контекста с цепочками обмена

    if (!context->context.IsCompatible (draw_swap_chain))
      RaiseInvalidArgument (METHOD_NAME, "draw_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

    if (draw_swap_chain != read_swap_chain && !context->context.IsCompatible (read_swap_chain))
      RaiseInvalidArgument (METHOD_NAME, "read_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

      //регистрация обработчиков удаления цепочек обмена

    Trackable *draw_swap_chain_trackable = cast_object<Trackable> (draw_swap_chain, METHOD_NAME, "draw_swap_chain"),
              *read_swap_chain_trackable = cast_object<Trackable> (read_swap_chain, METHOD_NAME, "read_swap_chain");

    draw_swap_chain_trackable->RegisterDestroyHandler (on_destroy_draw_swap_chain);
    read_swap_chain_trackable->RegisterDestroyHandler (on_destroy_read_swap_chain);

      //обновление данных

    current_context         = &*iter->second;
    current_draw_swap_chain = draw_swap_chain;
    current_read_swap_chain = read_swap_chain;
    current_context_id      = context_id;
  }

    //восставление контекста (после удаления контекста или цепочки обмена)
  void RestoreContext ()
  {
      //если текущий контекст не удалён - устанавливаем в качестве текущих цепочек обмена основную цепочку обмена контекста
    
    ContextMap::iterator iter = context_map.find (current_context_id);

    if (iter != context_map.end ())
    {
      current_draw_swap_chain = current_read_swap_chain = &*current_context->master_swap_chain;

      return;
    }
    
      //если был удалён текущий контекст
      
    if (context_map.empty ())
    {
        //если удалены все контексты - восставление не возможно

      current_draw_swap_chain = 0;
      current_read_swap_chain = 0;
      current_context         = 0;
      current_context_id      = 0;

      return;
    }

      //выбираем в качестве текущего первый контекст из списка созданных вместе с его основной цепочкой обмена

    ContextImplPtr context = context_map.begin ()->second;

    current_context         = &*context;
    current_read_swap_chain = &*context->master_swap_chain;
    current_draw_swap_chain = current_read_swap_chain;
    current_context_id      = context_map.begin ()->first;
  }
  
    //активация текущего контекста
  void MakeContextCurrent ()
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::MakeContextCurrent";

    if (!current_context)
      RaiseInvalidOperation (METHOD_NAME, "Null active context");

    try
    {
      current_context->context.MakeCurrent (current_draw_swap_chain, current_read_swap_chain);
    }
    catch (common::Exception& exception)
    {
      exception.Touch (METHOD_NAME);

      throw;
    }
  }
  
    //проверка является ли текущий контекст активным
  bool IsContextCurrent ()
  {
    return current_context && current_context->context.IsCurrent (current_draw_swap_chain, current_read_swap_chain);
  }
  
    //создание цепочки обмена совместимой с контекстом
  ISwapChain* CreateCompatibleSwapChain (size_t context_id)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateCompatibleSwapChain";

      //поиск контекста

    ContextMap::iterator iter = context_map.find (context_id);

    if (iter == context_map.end ())
      RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

    ContextImplPtr context = iter->second;
    
      //создание цепочки обмена
      
    return SwapChainManager::CreatePBuffer (context->master_swap_chain.get ());
  }
  
    //проверка совместимости контекста и цепочки обмена
  bool IsCompatible (size_t context_id, ISwapChain* swap_chain)
  {
      //поиск контекста

    ContextMap::iterator iter = context_map.find (context_id);

    if (iter == context_map.end ())
      return false;

    return iter->second->context.IsCompatible (swap_chain);
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
    Создание / удаление контекста
*/

size_t ContextManager::CreateContext (ISwapChain* swap_chain)
{
  return impl->CreateContext (swap_chain);
}

void ContextManager::DeleteContext (size_t context_id)
{
  impl->DeleteContext (context_id);
}

/*
    Создание цепочки обмена совместимой с контекстом
*/

ISwapChain* ContextManager::CreateCompatibleSwapChain (size_t context_id)
{
  return impl->CreateCompatibleSwapChain (context_id);
}

/*
    Выбор текущего контекста
*/

void ContextManager::SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
{
  impl->SetContext (context_id, draw_swap_chain, read_swap_chain);
}

ISwapChain* ContextManager::GetDrawSwapChain () const
{
  return impl->current_draw_swap_chain;
}

ISwapChain* ContextManager::GetReadSwapChain () const
{
  return impl->current_read_swap_chain;
}

size_t ContextManager::GetContextId () const
{
  return impl->current_context_id;
}

/*
    Активация текущего контекста
*/

void ContextManager::MakeContextCurrent () const
{
  impl->MakeContextCurrent ();
}

bool ContextManager::IsContextCurrent () const
{
  return impl->IsContextCurrent ();
}

/*
    Работа с таблицей локальных данных контекста
*/

void ContextManager::SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::SetContextData", "Null active context");

  impl->current_context->SetContextData (table_id, element_id, value);
}

size_t ContextManager::GetContextData (ContextDataTable table_id, size_t element_id) const
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetContextData", "Null active context");

  return impl->current_context->GetContextData (table_id, element_id);
}

void ContextManager::ClearContextData (ContextDataTable table_id)
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");
    
  impl->current_context->ClearContextData (table_id);
}

void ContextManager::ClearContextData ()
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");

  impl->current_context->ClearContextData ();
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
    Проверка совместимости контекста и цепочки обмена
*/

bool ContextManager::IsCompatible (size_t context_id, ISwapChain* swap_chain) const
{
  return impl->IsCompatible (context_id, swap_chain);
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
