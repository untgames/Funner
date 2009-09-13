#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    Состояние менеджера запросов
*/

class QueryManagerState: public IStageState
{
  public:
///Конструктор
    QueryManagerState (QueryManagerState* in_main_state = 0) : main_state (in_main_state) {}

///Установка предиката отрисовки
    void SetPredication (IAsyncPredicate* in_predicate, bool in_predicate_value)
    {
      predicate       = in_predicate;
      predicate_value = in_predicate_value;
    }

///Получение предиката отрисовки
    IAsyncPredicate* GetPredicate () { return predicate.get (); }

///Получение сравниваемого значения предиката отрисовки
    bool GetPredicateValue () { return predicate_value; }

///Получение результата предиката
    bool GetPredicateAsyncResult ()
    {
      if (!(predicate && predicate->IsResultAvailable ()))
        return true;

      return predicate->GetResult () == predicate_value;
    }

///Захват состояния
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }

///Восстановление состояния
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:
///Копирование состояния
    void Copy (QueryManagerState& source, const StateBlockMask& mask)
    {
      if (mask.predication)
      {
        predicate       = source.GetPredicate ();
        predicate_value = source.GetPredicateValue ();
      }
    }

  private:
    typedef xtl::trackable_ptr<QueryManagerState> QueryManagerStatePtr;
    typedef xtl::trackable_ptr<IAsyncPredicate>   AsyncPredicatePtr;

  private:
    QueryManagerStatePtr main_state;       //основное состояние уровня
    AsyncPredicatePtr    predicate;        //предикат
    bool                 predicate_value;  //сравниваемое значение
};

}

/*
    Описание реализации менеджера запросов
*/

struct QueryManager::Impl: public ContextObject, public QueryManagerState
{
  public:
///Конструктор
    Impl (const ContextManager& manager) : ContextObject (manager)
    {
      SetContextCacheValue (CacheEntry_IsInQueryRanges, 0);
    }

///Создание предикатов
    IPredicate* CreatePredicate ()
    {
      static const char* METHOD_NAME = "render::low_level::opengl::QueryManager::Impl::CreatePredicate";

        //проверка наличия необходимых расширений

      if (!GetCaps ().has_arb_occlusion_query)
        return new NullPredicate;

        //установка текущего контекста

      MakeContextCurrent ();
      
#ifndef OPENGL_ES_SUPPORT

        //получение количества битов в результате запроса

      GLint query_counter_bits = 0;

      glGetQueryiv (GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_counter_bits);

      CheckErrors (METHOD_NAME);

        //в случае невозможности получения корректного результата - создаём эмуляцию

      if (!query_counter_bits)
        return new NullPredicate;

        //создание предиката

      return new AsyncPredicate (GetContextManager ());
#else
      return new NullPredicate;
#endif
    }
};

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
    Создание объекта состояния уровня
*/

IStageState* QueryManager::CreateStageState ()
{
  return new QueryManagerState (static_cast<QueryManagerState*> (&*impl));
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
  static const char* METHOD_NAME = "render::low_level::opengl::QueryManager::SetPredication";

  IAsyncPredicate* casted_predicate = cast_object<IAsyncPredicate> (predicate, METHOD_NAME, "predicate");
  ContextObject*   casted_object    = dynamic_cast<ContextObject*> (predicate);

  if (casted_object && !casted_object->IsCompatible (impl->GetContextManager ()))
    throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Argument 'predicate' is incompatible with target IDevice");

  impl->SetPredication (casted_predicate, predicate_value);
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
