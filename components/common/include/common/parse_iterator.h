#ifndef COMMONLIB_PARSE_ITERATOR_HEADER
#define COMMONLIB_PARSE_ITERATOR_HEADER

#include <common/parse_tree.h>

namespace common
{

namespace detail
{

//froward declarations
struct NextParseNodeFn;
struct NextNamesakeParseNodeFn;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор обхода узлов дерева разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class NextFun>
class BasicParseIterator
{
  typedef bool (BasicParseIterator::*UnspecifiedBoolType)() const;
  public: 
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicParseIterator ();
    BasicParseIterator (const ParseNode&);
    
    template <class NextFun1>
    BasicParseIterator (const BasicParseIterator<NextFun1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////    
    BasicParseIterator& operator = (const ParseNode&);

    template <class NextFun1> 
    BasicParseIterator& operator = (const BasicParseIterator<NextFun1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переход по уровню иерархии
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicParseIterator& operator ++ ();
    BasicParseIterator  operator ++ (int);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение текущего узла дерева разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
          ParseNode* operator -> ();
    const ParseNode* operator -> () const;
          ParseNode& operator *  ();    
    const ParseNode& operator *  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на 0
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool     operator !          () const;
    operator UnspecifiedBoolType () const { return node.IsEmpty () ? 0 : &BasicParseIterator::operator !; }

  private:
    ParseNode node;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef BasicParseIterator<detail::NextParseNodeFn>         ParseIterator;
typedef BasicParseIterator<detail::NextNamesakeParseNodeFn> ParseNamesakeIterator;

#include <common/detail/parse_iterator.inl>

}

#endif
