#ifndef __MYSTL_REVERSE_ITERATOR__
#define __MYSTL_REVERSE_ITERATOR__

#include <stl/iterator_traits.h>

namespace stl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор обратного перебора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iterator> class reverse_iterator 
{
  public:
    typedef typename iterator_traits<Iterator>::value_type        value_type;
    typedef typename iterator_traits<Iterator>::difference_type   difference_type;
    typedef typename iterator_traits<Iterator>::pointer           pointer;
    typedef typename iterator_traits<Iterator>::reference         reference;
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef Iterator                                              iterator_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
             reverse_iterator () {}    
    explicit reverse_iterator (iterator_type);
             reverse_iterator (const reverse_iterator&);
             
    template <class Iter> reverse_iterator (const reverse_iterator<Iter>&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Переход к базовому итератору
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator_type base () const { return current; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции доступа и индексирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    reference operator *  () const;  
    reference operator [] (difference_type i) const;
    pointer   operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Арифметические операции
///////////////////////////////////////////////////////////////////////////////////////////////////
    reverse_iterator&      operator ++ ();
    reverse_iterator&      operator -- ();
    reverse_iterator&      operator += (difference_type);
    reverse_iterator&      operator -= (difference_type);                  
    const reverse_iterator operator ++ (int);  
    const reverse_iterator operator -- (int);    
    const reverse_iterator operator +  (difference_type) const;
    const reverse_iterator operator -  (difference_type) const;  
    difference_type        operator -  (const reverse_iterator&) const;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух итераторов обратного перебора
///////////////////////////////////////////////////////////////////////////////////////////////////                  
    bool operator == (const reverse_iterator&) const;
    bool operator != (const reverse_iterator&) const;
    bool operator >= (const reverse_iterator&) const;
    bool operator <= (const reverse_iterator&) const;
    bool operator >  (const reverse_iterator&) const;
    bool operator <  (const reverse_iterator&) const;

  private:
    Iterator current;
};

template <class Iter> 
reverse_iterator<Iter> operator + (typename reverse_iterator<Iter>::difference_type n, const reverse_iterator<Iter>&);

#include <stl/detail/reverse_iterator.inl>

}

#endif
