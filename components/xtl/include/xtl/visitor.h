#ifndef XTL_VISITOR_HEADER
#define XTL_VISITOR_HEADER

namespace xtl
{

namespace mpl
{

//implementation forwards
template <class Head, class Tail> struct type_node;
struct null_type;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик посещений объектов неизвестных типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class ResultType> struct default_visitor_action
{
  template <class T, class Visitor> ResultType operator () (T&, Visitor&) const { return ResultType (); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс паттерна "acyclic visitor"
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class ResultType=void, class DefaultVisitorAction=default_visitor_action<ResultType> >
class basic_visitor: private DefaultVisitorAction
{
  public:
    typedef ResultType result_type;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данный деструктор нужен для обеспечения работы механизма RTTI
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual ~basic_visitor () {}    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка посещений объектов разных типов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> result_type operator () (T&);
    template <class T> result_type operator () (T&) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Посетитель для определённого типа данных
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Ret> class visitor_node
{
  public:
    virtual Ret visit (T&) const = 0;
    virtual Ret visit (T& object) { return const_cast<const visitor_node&> (*this).visit (object); }
};

//специализация для списка типов
template <class Head, class Tail, class Ret>
class visitor_node<mpl::type_node<Head, Tail>, Ret>: public visitor_node<Head, Ret>, public visitor_node<Tail, Ret> {};

template <class Ret> class visitor_node<mpl::null_type, Ret> {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Visitor для объектов конкретных типов (в качестве каждого из параметров может быть список типов)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class BasicVisitor, class T1=mpl::null_type, class T2=mpl::null_type, class T3=mpl::null_type, class T4=mpl::null_type,
          class T5=mpl::null_type, class T6=mpl::null_type, class T7=mpl::null_type, class T8=mpl::null_type, class T9=mpl::null_type, class T10=mpl::null_type>
class visitor: public visitor<BasicVisitor, T2, T3, T4, T5, T6, T7, T8, T9>,
               public visitor_node<T1, typename BasicVisitor::result_type> {};
               
template <class BasicVisitor> class visitor<BasicVisitor>: public BasicVisitor {};

#include <xtl/detail/visitor.inl>

}

#endif
