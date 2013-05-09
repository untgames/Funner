#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    QueryManagerContextState
===================================================================================================
*/

/*
    Описание реализации состояния контекста менеджера запросов
*/

struct QueryManagerContextState::Impl: public DeviceObject
{
  QueryPtr predicate;       //предикат отрисовки
  bool     predicate_value; //значение предиката
  bool     is_dirty;        //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , predicate_value ()
    , is_dirty (true)
  {
  }

/// Деструктор
  virtual ~Impl ()
  {
  }

/// Обновление состояния
  void UpdateNotify ()
  {
    is_dirty = true;
  }
};

/*
    Конструктор / деструктор
*/

QueryManagerContextState::QueryManagerContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

QueryManagerContextState::QueryManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

QueryManagerContextState::~QueryManagerContextState ()
{
}

/*
    Реализация
*/

QueryManagerContextState::Impl& QueryManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    Управление предикатами отрисовки
*/

void QueryManagerContextState::SetPredication (IPredicate* in_predicate, bool predicate_value)
{
  try
  {
    Query* query = cast_object<Query> (*impl, in_predicate, "", "predicate");

    if (query && query->GetType () != QueryType_OcclusionPredicate)
      throw xtl::format_operation_exception ("", "Query with type %s can used as predicate", get_name (query->GetType ()));

    if (query == impl->predicate && predicate_value == impl->predicate_value)
      return;

    impl->predicate       = query;
    impl->predicate_value = predicate_value;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContextState::SetPredication");
    throw;
  }
}

IPredicate* QueryManagerContextState::GetPredicate ()
{
  return impl->predicate.get ();
}

bool QueryManagerContextState::GetPredicateValue ()
{  
  return impl->predicate_value;
}

/*
    Копирование состояния
*/

void QueryManagerContextState::CopyTo (const StateBlockMask& mask, QueryManagerContextState& dst_state) const
{
  if (!mask.predication)
    return;
  
  if (dst_state.impl->predicate == impl->predicate && dst_state.impl->predicate_value == impl->predicate_value)
    return;

  dst_state.impl->predicate       = impl->predicate;
  dst_state.impl->predicate_value = impl->predicate_value;

  impl->UpdateNotify ();
}

/*
===================================================================================================
    QueryManagerContextState
===================================================================================================
*/

/*
    Описание реализации контекста менеджера запросов
*/

struct QueryManagerContext::Impl: public QueryManagerContextState::Impl
{
  DxContextPtr context; //контекст отрисовки

/// Конструктор
  Impl (const DeviceManager& manager, const DxContextPtr& in_context)
    : QueryManagerContextState::Impl (manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::QueryManagerContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

QueryManagerContext::QueryManagerContext (const DeviceManager& manager, const DxContextPtr& context)
  : QueryManagerContextState (new Impl (manager, context))
{
}

QueryManagerContext::~QueryManagerContext ()
{
}

/*
    Реализация
*/

QueryManagerContext::Impl& QueryManagerContext::GetImpl () const
{
  return static_cast<Impl&> (QueryManagerContextState::GetImpl ());
}

/*
    Указание границ запроса
*/

void QueryManagerContext::Begin (IQuery* async)
{
  try
  {
    Impl& impl = GetImpl ();

    Query* query = cast_object<Query> (impl, async, "", "async");

    if (!query)
      throw xtl::make_null_argument_exception ("", "async");

    impl.context->Begin (&query->GetHandle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::Begin");
    throw;
  }
}

void QueryManagerContext::End (IQuery* async)
{
  try
  {
    Impl& impl = GetImpl ();

    Query* query = cast_object<Query> (impl, async, "", "async");

    if (!query)
      throw xtl::make_null_argument_exception ("", "async");

    impl.context->End (&query->GetHandle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::End");
    throw;
  }
}

/*
    Установка состояния уровня в контекст
*/

void QueryManagerContext::Bind ()
{
  try
  {
      //проверка флага "грязности"

    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

    impl.context->SetPredication (impl.predicate ? static_cast<ID3D11Predicate*> (&impl.predicate->GetHandle ()) : (ID3D11Predicate*)0, impl.predicate_value);

      //очистка флага "грязности"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::QueryManagerContext::Bind");
    throw;
  }
}
