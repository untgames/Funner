#ifndef RENDER_DX11_DRIVER_QUERY_MANAGER_HEADER
#define RENDER_DX11_DRIVER_QUERY_MANAGER_HEADER

#include <render/low_level/query.h>

#include <shared/common.h>
#include <shared/device_manager.h>

#include <stl/auto_ptr.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние контекста менеджера выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class QueryManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    QueryManagerContextState  (const DeviceManager&);
    ~QueryManagerContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление предикатами отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPredication    (IPredicate* predicate, bool predicate_value);
    IPredicate* GetPredicate      ();
    bool        GetPredicateValue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование состояния
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (const StateBlockMask&, QueryManagerContextState& dst_state) const;

  protected:
    struct Impl;

    Impl& GetImpl () const;

    QueryManagerContextState (Impl*);

  private:
    QueryManagerContextState (const QueryManagerContextState&); //no impl
    QueryManagerContextState& operator = (const QueryManagerContextState&); //no impl

  private:
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст менеджера выходного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class QueryManagerContext: public QueryManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    QueryManagerContext  (const DeviceManager&, const DxContextPtr&);
    ~QueryManagerContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Указание границ запроса
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Begin (IQuery* async);
    void End   (IQuery* async);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния уровня в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  protected:
    struct Impl;

    Impl& GetImpl () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер запросов
///////////////////////////////////////////////////////////////////////////////////////////////////
class QueryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    QueryManager  (const DeviceManager&);
    ~QueryManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание предикатов
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPredicate* CreatePredicate ();
    IQuery*     CreateQuery     (QueryType);

  private:
    QueryManager (const QueryManager&);              //no impl
    QueryManager& operator = (const QueryManager&);  //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
