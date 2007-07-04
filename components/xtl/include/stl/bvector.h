#ifndef __MYSTL_BIT_VECTOR__
#define __MYSTL_BIT_VECTOR__

#include <climits>
#include <stl/bitref.h>
#include <stl/vector>

namespace stl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Битовый итератор
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class WordT>
class bit_iterator_base
{
  public:
    typedef WordT      word_type;  
    typedef ptrdiff_t  difference_type;  
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Разность двух "итераторов"
///////////////////////////////////////////////////////////////////////////////////////////////////
    difference_type operator - (const bit_iterator_base&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const bit_iterator_base&) const;
    bool operator != (const bit_iterator_base&) const;
    bool operator <  (const bit_iterator_base&) const;
    bool operator >  (const bit_iterator_base&) const;
    bool operator <= (const bit_iterator_base&) const;
    bool operator >= (const bit_iterator_base&) const;    

  protected:
    bit_iterator_base ();
    bit_iterator_base (word_type* pos,word_type offset);
    
    enum { BITS_PER_WORD = sizeof (word_type) * CHAR_BIT }; //количество битов на слово
      
  protected:  
    word_type* pos, offset;  
};

template <class WordT,class Reference>
class bit_iterator: public bit_iterator_base<WordT>
{
  template <class Allocator> friend class bit_vector;
  typedef bit_iterator_base<WordT> base;
  public:  
    typedef bool                                          value_type;
    typedef size_t                                        size_type;
    typedef typename base::difference_type                difference_type;
    typedef typename base::word_type                      word_type;
    typedef Reference                                     reference;
    typedef bool                                          const_reference;
    typedef Reference*                                    pointer;
    typedef const bool*                                   const_pointer;
    typedef bit_iterator<WordT,bit_reference<word_type> > iterator;
    typedef bit_iterator<WordT,bool>                      const_iterator;    
    typedef random_access_iterator_tag                    iterator_category;

    bit_iterator () {}
    bit_iterator (const base&);
     
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к данным
///////////////////////////////////////////////////////////////////////////////////////////////////                            
    reference operator  * () const;
    reference operator [] (difference_type) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Арифметические операции
///////////////////////////////////////////////////////////////////////////////////////////////////                                
    bit_iterator&   operator ++ ();
    bit_iterator    operator ++ (int);
    bit_iterator&   operator -- ();
    bit_iterator    operator -- (int);
    
    bit_iterator&   operator += (difference_type);
    bit_iterator&   operator -= (difference_type);
    bit_iterator    operator  + (difference_type) const;
    bit_iterator    operator  - (difference_type) const;
    difference_type operator  - (const base&) const;

  private:
    bit_iterator (word_type* ptr,word_type offset);
    
    void next ();
    void prev ();
    
  private:
    using base::pos;
    using base::offset;
    using base::BITS_PER_WORD;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Битовый вектор
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Allocator=typename default_allocator<bool>::allocator_type>
class bit_vector
{
  public:
    typedef Allocator                                allocator_type;
    typedef bool                                     value_type;
    typedef unsigned int                             word_type;
    typedef size_t                                   size_type;
    typedef ptrdiff_t                                difference_type;
    typedef bit_reference<word_type>                 reference;
    typedef bool                                     const_reference;
    typedef reference*                               pointer;
    typedef const bool*                              const_pointer;
    typedef bit_iterator<word_type,reference>        iterator;
    typedef bit_iterator<word_type,const_reference>  const_iterator;
    typedef reverse_iterator<const_iterator>         const_reverse_iterator;
    typedef reverse_iterator<iterator>               reverse_iterator;
    
/////////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
/////////////////////////////////////////////////////////////////////////////////////////////////////    
    explicit bit_vector  (const allocator_type& = allocator_type::create ());
    explicit bit_vector  (size_type n,bool value = false,const allocator_type& = allocator_type::create ());

    template <class InIter> bit_vector (InIter first,InIter last,const allocator_type& = allocator_type::create ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аллокатор
///////////////////////////////////////////////////////////////////////////////////////////////////    
    allocator_type get_allocator () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер массива / макс. возможный размер / объём / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_type size     () const;
    size_type max_size () const;
    size_type capacity () const;
    bool      empty    () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение итераторов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    iterator               begin  ();
    iterator               end    ();
    reverse_iterator       rbegin ();
    reverse_iterator       rend   ();
    const_iterator         begin  () const;
    const_iterator         end    () const;
    const_reverse_iterator rbegin () const;    
    const_reverse_iterator rend   () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции доступа
///////////////////////////////////////////////////////////////////////////////////////////////////        
    reference       operator [] (size_type);
    const_reference operator [] (size_type) const;

    reference       at    (size_type); //[] with range check
    const_reference at    (size_type) const; //[] with range check
    reference       front ();
    const_reference front () const;
    reference       back  ();
    const_reference back  () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////       
    template <class InIter> void assign (InIter first,InIter last);
                            void assign (size_type count,bool value);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера вектора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void resize  (size_type new_size,bool value = false);
    void reserve (size_type size);                                                   

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / извлечение элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void push_back (bool value);
    void pop_back  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator insert (iterator position,bool value = false);
    void     insert (iterator position,size_type n,bool value);
  
    template <class InIter> void insert (iterator position,InIter first,InIter last);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление элементов
///////////////////////////////////////////////////////////////////////////////////////////////////                                              
    iterator erase (iterator);
    iterator erase (iterator first,iterator last);
    void     clear ();  
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Инверсия битов
///////////////////////////////////////////////////////////////////////////////////////////////////                                                      
    void flip ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////                                                  
    void swap (bit_vector&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////                                                  
    bool operator == (const bit_vector&) const;
    bool operator != (const bit_vector&) const;
    bool operator <  (const bit_vector&) const;
    bool operator >  (const bit_vector&) const;
    bool operator <= (const bit_vector&) const;
    bool operator >= (const bit_vector&) const;
    
  private:
    enum { BITS_PER_WORD = sizeof (word_type) * CHAR_BIT };
    
    iterator  prepare_room (iterator,size_type);
    void      trim         (size_type);
    bool      is_valid     (iterator) const;
    bool      is_valid     (iterator,iterator) const;
        
    static size_type words_count (size_type);
    
    template <class Iter> void _init   (Iter,Iter,int_iterator_tag);
    template <class Iter> void _init   (Iter,Iter,input_iterator_tag);
    template <class Iter> void _insert (iterator,Iter,Iter,int_iterator_tag);
    template <class Iter> void _insert (iterator,Iter,Iter,input_iterator_tag);
    template <class Iter> void _insert (iterator,Iter,Iter,forward_iterator_tag);
    template <class Iter> void _assign (Iter,Iter,int_iterator_tag);
    template <class Iter> void _assign (Iter,Iter,input_iterator_tag);
    template <class Iter> void _assign (Iter,Iter,forward_iterator_tag);
    
  private:
    typedef vector<word_type,allocator_type> storage_vector;
   
    size_type      seq_size;
    storage_vector vec;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация для vector<bool>
///////////////////////////////////////////////////////////////////////////////////////////////////                                                  
template <class Allocator> 
class vector<bool,Allocator>: public bit_vector<Allocator>
{
  typedef bit_vector<Allocator> base;
  public:
    typedef typename base::allocator_type allocator_type;  
    typedef typename base::size_type      size_type;  
    typedef typename base::value_type     value_type;      
  
    explicit vector (const allocator_type& = allocator_type::create ());
    explicit vector (size_type count,value_type value = false,const allocator_type& = allocator_type::create ());

    template <class InIter> vector (InIter first,InIter last,const allocator_type& = allocator_type::create ());
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух битовых векторов
///////////////////////////////////////////////////////////////////////////////////////////////////                                                  
template <class Allocator>
void swap(bit_vector<Allocator>&,bit_vector<Allocator>&);

template <class Allocator>
void swap(vector<bool,Allocator>&,vector<bool,Allocator>&);

#include <stl/detail/stl/bvector.inl>

}

#endif
