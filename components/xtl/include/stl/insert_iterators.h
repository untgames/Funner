#ifndef __MYSTL_INSERT_ITERATORS__
#define __MYSTL_INSERT_ITERATORS__

#include <stl/iterator_traits.h>

namespace stl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///»тератор вставки в хвост контейнера
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Container>
class back_insert_iterator 
{
  public:
    typedef Container           container_type;
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    explicit back_insert_iterator (container_type&);

    back_insert_iterator& operator = (const typename container_type::value_type&);

    back_insert_iterator& operator  * ()    { return *this; }
    back_insert_iterator& operator ++ ()    { return *this; }
    back_insert_iterator& operator ++ (int) { return *this; }

  protected:
    container_type* container;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///»тератор вставки в начало контейнера
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Container>
class front_insert_iterator 
{
  public:
    typedef Container           container_type;
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    explicit front_insert_iterator (container_type&);

    front_insert_iterator& operator = (const typename container_type::value_type&);

    front_insert_iterator& operator  * ()    { return *this; }
    front_insert_iterator& operator ++ ()    { return *this; }
    front_insert_iterator& operator ++ (int) { return *this; }

  protected:
    container_type* container;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///»тератор вставки в произвольную позицию контейнера
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Container>
class insert_iterator 
{
  public:
    typedef Container           container_type;
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;

    explicit insert_iterator (container_type&,typename container_type::iterator pos);

    insert_iterator& operator = (const typename container_type::value_type&);

    insert_iterator& operator  * ()    { return *this; }
    insert_iterator& operator ++ ()    { return *this; }
    insert_iterator& operator ++ (int) { return *this; }

  protected:
    container_type* container;
    typename container_type::iterator pos;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕолучение итераторов вставки
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Container> 
back_insert_iterator<Container> back_inserter (Container&);

template <class Container> 
front_insert_iterator<Container> front_inserter (Container&);

template <class Container,class Iter> 
insert_iterator<Container> inserter (Container&,Iter pos);

#include <stl/detail/insert_iterators.inl>

}

#endif
