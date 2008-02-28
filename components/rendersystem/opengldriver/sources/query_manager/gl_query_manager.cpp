#include <xtl/trackable_ptr.h>

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

struct QueryManager::Impl : public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание предикатов
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPredicate* CreatePredicate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPredication    (IPredicate* in_predicate, bool in_predicate_value);
    IPredicate* GetPredicate      ();
    bool        GetPredicateValue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение результата предиката
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetPredicateAsyncResult ();

  private:
    xtl::trackable_ptr<AsyncPredicate> predicate;
    bool                               predicate_value;
};

/*
   Конструктор
*/

QueryManager::Impl::Impl (const ContextManager& manager) 
  : ContextObject (manager), predicate (0), predicate_value (true)
{
  GetContextDataTable (Stage_QueryManager)[QueryManagerCache_IsInRanges] = 0;
}

/*
   Создание предикатов
*/

IPredicate* QueryManager::Impl::CreatePredicate ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::QueryManager::Impl::CreatePredicate";

  if (!GetCaps ().has_arb_occlusion_query)
    return new NullPredicate (GetContextManager ());

  int query_counter_bits = 0;

  MakeContextCurrent ();
                   
  glGetQueryiv (GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_counter_bits);

  CheckErrors (METHOD_NAME);

  if (!query_counter_bits)
    return new NullPredicate (GetContextManager ());

  return new AsyncPredicate (GetContextManager ());
}

/*
   Управление предикатами отрисовки
*/

void QueryManager::Impl::SetPredication (IPredicate* in_predicate, bool in_predicate_value)
{
  predicate       = cast_object<AsyncPredicate> (GetContextManager (), in_predicate, "render::low_level::opengl::QueryManager::Impl::SetPredication", "predicate");
  predicate_value = in_predicate_value;
}

IPredicate* QueryManager::Impl::GetPredicate ()
{
  return predicate.get ();
}

bool QueryManager::Impl::GetPredicateValue ()
{
  return predicate_value;
}

/*
   Получение результата предиката
*/

bool QueryManager::Impl::GetPredicateAsyncResult ()
{
  if (!(predicate && predicate->IsResultAvailable ()))
    return true;

  return predicate->GetResult () == predicate_value;
}

/*
   Конструктор / деструктор
*/

QueryManager::QueryManager (const ContextManager& manager)
  : impl (new Impl (manager))
  {}

QueryManager::~QueryManager ()
{
}

/*
   Создание предикатов
*/

IPredicate* QueryManager::CreatePredicate ()
{
  return impl->CreatePredicate ();
}

/*
   Управление предикатами отрисовки
*/

void QueryManager::SetPredication (IPredicate* predicate, bool predicate_value)
{
  impl->SetPredication (predicate, predicate_value);
}

IPredicate* QueryManager::GetPredicate ()
{
  return impl->GetPredicate ();
}

bool QueryManager::GetPredicateValue ()
{
  return impl->GetPredicateValue ();
}

/*
   Получение результата предиката
*/

bool QueryManager::GetPredicateAsyncResult ()
{
  return impl->GetPredicateAsyncResult ();
}
