#ifndef __MYSTL_HASH_TABLE__
#define __MYSTL_HASH_TABLE__

#include <stl/list>
#include <stl/vector>
#include <stl/algorithm>

namespace stl
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///Хэш-таблица
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Value,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
class hashtable 
{
  typedef stl::list<Value,Allocator> list_type;
  public:
    typedef size_t        size_type;
    typedef ptrdiff_t     difference_type;  
    typedef Value         value_type;
    typedef Value*        pointer;
    typedef const Value*  const_pointer;
    typedef Value&        reference;
    typedef const Value&  const_reference;
    typedef Key           key_type;
    typedef HashFn        hasher;
    typedef EqualKey      key_equal;

    typedef typename list_type::allocator_type         allocator_type;
    typedef typename list_type::iterator               iterator;
    typedef typename list_type::const_iterator         const_iterator;
    typedef typename list_type::reverse_iterator       reverse_iterator;
    typedef typename list_type::const_reverse_iterator const_reverse_iterator;   
   
    hashtable  (size_type buckets_count,const hasher&,const key_equal&,const allocator_type&);
    hashtable  (const hashtable&);   

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    hashtable& operator = (const hashtable&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Аллокатор
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    allocator_type get_allocator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Функции общего назначения
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    size_type size      () const;
    size_type max_size  () const;
    bool      empty     () const;
    hasher    hash_func () const;
    key_equal key_eq    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Получение итераторов
///////////////////////////////////////////////////////////////////////////////////////////////////////////              
    iterator               begin  ();
    const_iterator         begin  () const;    
    iterator               end    ();
    const_iterator         end    () const;
    reverse_iterator       rbegin ();
    const_reverse_iterator rbegin () const;
    reverse_iterator       rend   ();
    const_reverse_iterator rend   () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о пакетах
///////////////////////////////////////////////////////////////////////////////////////////////////////////           
    size_type bucket_count     () const;
    size_type max_bucket_count () const;
    size_type elems_in_bucket  (size_type bucket) const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка
///////////////////////////////////////////////////////////////////////////////////////////////////////////               
    pair<iterator,bool> insert_unique (const value_type&);
    iterator            insert_equal  (const value_type&);
 
    template <class Iter> void insert_unique (Iter first,Iter last);
    template <class Iter> void insert_equal  (Iter first,Iter last);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление
///////////////////////////////////////////////////////////////////////////////////////////////////////////                       
    size_type erase (const key_type&);
    void      erase (iterator);
    void      erase (iterator first,iterator last);
    void      erase (const key_type* first,const key_type* last);    
    void      clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////////////                   
    reference      find_or_insert (const value_type&);
    iterator       find           (const key_type&);
    const_iterator find           (const key_type&) const;
    size_type      count          (const key_type&) const;
    
    pair<iterator,iterator>             equal_range (const key_type&);
    pair<const_iterator,const_iterator> equal_range (const key_type&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера
///////////////////////////////////////////////////////////////////////////////////////////////////////////                      
    void resize (size_type num_elements);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух таблиц
///////////////////////////////////////////////////////////////////////////////////////////////////////////                  
    void swap (hashtable&);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение (не совсем корректно: проверка производится в лексикографическом порядке)
///////////////////////////////////////////////////////////////////////////////////////////////////////////                              
    bool operator == (const hashtable&) const;
    bool operator != (const hashtable&) const;
    bool operator <  (const hashtable&) const;
    bool operator >  (const hashtable&) const;
    bool operator <= (const hashtable&) const;
    bool operator >= (const hashtable&) const;
    
  private:       
    struct Bucket 
    {
      iterator  first;
      size_type count;

      Bucket (iterator first,size_type count);
    };
  
    void _copy (const hashtable&);
    
    static const key_type& keyof (const value_type&);
    
    size_type next_size   (size_type) const;
    size_type bkt_num_key (const key_type&) const;
    size_type bkt_num     (const value_type&) const;    

    pair<iterator,bool> insert_unique_noresize (const value_type&);
    iterator            insert_equal_noresize  (const value_type&);    
       
  private:
    typedef vector<Bucket,typename allocator_type::template rebind<Bucket>::other> vector_type;
  
    list_type     list;
    vector_type   table;
    hasher        hash;
    key_equal     equals;
    size_type     num_elements;
};

template <class Value,class Key,class HashFn,class KeyOf,class EqualKey,class Allocator>
void swap (hashtable<Value,Key,HashFn,KeyOf,EqualKey,Allocator>&,
           hashtable<Value,Key,HashFn,KeyOf,EqualKey,Allocator>&);

#include <stl/detail/hashtable.inl>

}

#endif
