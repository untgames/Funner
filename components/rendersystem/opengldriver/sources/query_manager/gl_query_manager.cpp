#include <xtl/trackable_ptr.h>

#include "shared.h"

using namespace common;
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
      //конструктор
    QueryManagerState (QueryManagerState* in_main_state = 0) : main_state (in_main_state) {}

      //Управление предикатами отрисовки
    void SetPredication (AsyncPredicate* in_predicate, bool in_predicate_value)
    {
      predicate       = in_predicate;
      predicate_value = in_predicate_value;
    }

    AsyncPredicate* GetPredicate () const
    {
      return predicate.get ();
    }

    bool GetPredicateValue () const
    {
      return predicate_value;
    }

      //Получение результата предиката
    bool GetPredicateAsyncResult () const
    {
      if (!(predicate && predicate->IsResultAvailable ()))
        return true;

      return predicate->GetResult () == predicate_value;
    }

      //захват состояния
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //восстановление состояния
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }
    
  private:
      //копирование
    void Copy (const QueryManagerState& source, const StateBlockMask& mask)
    {
      if (mask.predication)
      {
        predicate       = source.GetPredicate ();
        predicate_value = source.GetPredicateValue ();
      }
    }

  private:
    typedef xtl::trackable_ptr<QueryManagerState> QueryManagerStatePtr;
    typedef xtl::trackable_ptr<AsyncPredicate>    AsyncPredicatePtr;

  private:
    QueryManagerStatePtr main_state;
    AsyncPredicatePtr    predicate;
    bool                 predicate_value;
};

}

struct QueryManager::Impl : public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение основного состояния уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    QueryManagerState& GetState () { return state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание предикатов
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPredicate* CreatePredicate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetPredication (IPredicate* in_predicate, bool in_predicate_value) 
    {
      state.SetPredication (cast_object<AsyncPredicate> (GetContextManager (), in_predicate, "render::low_level::opengl::QueryManager::Impl::SetPredication", "predicate"), in_predicate_value);
    }

    IPredicate* GetPredicate      () {return state.GetPredicate ();}
    bool        GetPredicateValue () {return state.GetPredicateValue ();}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение результата предиката
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetPredicateAsyncResult () {return state.GetPredicateAsyncResult ();}

  private:
    QueryManagerState state; //состояние уровня
};

/*
   Конструктор
*/

QueryManager::Impl::Impl (const ContextManager& manager) 
  : ContextObject (manager)
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
  return new QueryManagerState (&impl->GetState ());
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
