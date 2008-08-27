#include "shared.h"

using namespace render;

namespace
{

typedef RenderTargetImpl::Pointer RenderTargetPtr;

/*
    Реализация запроса рендеринга
*/

class RenderQueryImpl: public IRenderQuery, public xtl::reference_counter
{
  public:
///Конструктор
    RenderQueryImpl (const RenderTargetPtr& in_render_target) : render_target (in_render_target) {}

///Обновление
    void Update () { render_target->Update (); }

///Подсчёт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    RenderTargetPtr render_target; //цель рендеринга
};

/*
    Обработчик запроса
*/

class QueryHandlerEntry: public xtl::reference_counter
{
  public:
    typedef QueryManager::QueryFunction QueryFunction;
  
///Конструктор
    QueryHandlerEntry (const char* in_query_string_mask, const QueryFunction& in_handler)
      : query_string_mask (in_query_string_mask),
        handler (in_handler)
    {
    }
    
///Маска строки запроса
    const char* QueryStringMask () { return query_string_mask.c_str (); }

///Проверка возможности обработки
    bool IsMatch (const char* query_string) { return common::wcmatch (query_string, query_string_mask.c_str ()); }
    
///Создание объекта запроса
    IRenderQuery* CreateQuery (const RenderTargetPtr& render_target, const char* query_string)
    {
      ConstructableRenderTarget render_target_wrapper (*render_target);

      handler (render_target_wrapper, query_string);

      return new RenderQueryImpl (render_target);
    }

  private:
    stl::string   query_string_mask; //маска строки запроса
    QueryFunction handler;           //обработчик запроса
};

}

/*
    Описание реализации менеджера запросов
*/

typedef xtl::intrusive_ptr<QueryHandlerEntry> EntryPtr;
typedef stl::list<EntryPtr>                   QueryHandlerList;

struct QueryManager::Impl
{
  QueryHandlerList handlers; //обработчики запросов
};

/*
    Конструктор / деструктор
*/

QueryManager::QueryManager ()
  : impl (new Impl)
{
}

QueryManager::~QueryManager ()
{
}

/*
    Регистрация функций обработки запросов рендеринга (дочерний рендеринг)
*/

void QueryManager::RegisterQueryHandler (const char* query_string_mask, const QueryFunction& handler)
{
  static const char* METHOD_NAME = "render::QueryManager::RegisterQueryHandler";  

  if (!query_string_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "query_string_mask");

  impl->handlers.push_front (EntryPtr (new QueryHandlerEntry (query_string_mask, handler), false));
}

void QueryManager::UnregisterQueryHandler (const char* query_string_mask)
{
  if (!query_string_mask)
    return;
    
  for (QueryHandlerList::iterator iter=impl->handlers.begin (), end=impl->handlers.end (); iter!=end;)
    if (xtl::xstrcmp (query_string_mask, (*iter)->QueryStringMask ()))
    {
      QueryHandlerList::iterator next = iter;

      ++next;

      impl->handlers.erase (iter);

      iter = next;
    }
    else ++iter;
}

void QueryManager::UnregisterAllQueryHandlers ()
{
  impl->handlers.clear ();
}

/*
   Создание запроса
*/

IRenderQuery* QueryManager::CreateQuery
 (mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment,
  const char*               query_string,
  RenderManager&            render_manager)
{
  try
  {
    if (!query_string)
      throw xtl::make_null_argument_exception ("", "query_string");
      
      //поиск подходящего обработчика
      
    for (QueryHandlerList::iterator iter=impl->handlers.begin (), end=impl->handlers.end (); iter!=end; ++iter)
    {
      if (!(*iter)->IsMatch (query_string))
        continue;

        //создание запроса

      return (*iter)->CreateQuery (RenderTargetImpl::Create (render_manager, color_attachment, depth_stencil_attachment), query_string);
    }

      //создание пустого запроса

    return new RenderQueryImpl (RenderTargetImpl::Create (render_manager, color_attachment, depth_stencil_attachment));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::QueryManager::CreateQuery");
    throw;
  }
}
