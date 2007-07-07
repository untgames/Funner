#ifndef __MYSTL_RBTREE__
#define __MYSTL_RBTREE__

#include <stl/iterator>
#include <stl/alloc.h>

#ifdef _MSC_VER 
  #pragma warning (disable:4512) //'class' : assignment operator could not be generated
#endif

namespace stl
{

//forward decalration
template <class Key,class Value,class KeyOfValue,class Compare,class Allocator> class rbtree;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Узел красно-чёрного дерева
///////////////////////////////////////////////////////////////////////////////////////////////////////////
struct rbtree_node_base
{ 
  typedef rbtree_node_base Node;

  enum Color { RED = 0, BLACK = 1 };

  bool             color; 
  rbtree_node_base *parent,*left,*right;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Минимальный / максимальный узлы в поддеревьях
///////////////////////////////////////////////////////////////////////////////////////////////////////////
  Node* min () const;
  Node* max () const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Следующий / предыдущий узел по порядку следования
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
  Node* next () const;
  Node* prev () const;  
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вращения и перебалансировка
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
  void  rotate_left  (Node*& root);
  void  rotate_right (Node*& root);
  void  rebalance    (Node*& root);  
  Node* rebalance_for_erase (Node*& root,Node*& leftmost,Node*& rightmost);
};

template <class T> struct rbtree_node: public rbtree_node_base { T value; };

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор красно-чёрного дерева
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
template <class Value,class Reference,class Pointer>
class rbtree_iterator
{
  template <class Key,class T,class KeyOfValue,class Compare,class Allocator> friend class rbtree;
  public:
    typedef ptrdiff_t  difference_type;  
    typedef Value      value_type;
    typedef Reference  reference;
    typedef Pointer    pointer;
    
    typedef rbtree_iterator<Value,Value&,Value*>             iterator;
    typedef rbtree_iterator<Value,const Value&,const Value*> const_iterator;
    typedef bidirectional_iterator_tag                       iterator_category;

    rbtree_iterator () {}
    rbtree_iterator (const iterator&);
    
    reference         operator *  () const;
    pointer           operator -> () const;
    rbtree_iterator&  operator ++ ();
    rbtree_iterator   operator ++ (int);
    rbtree_iterator&  operator -- ();
    rbtree_iterator   operator -- (int);
    bool              operator == (const rbtree_iterator&) const;
    bool              operator != (const rbtree_iterator&) const;

  private:        
    typedef rbtree_node_base* link_type;
  
    rbtree_iterator (link_type);

  private:    
    link_type node;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Красно-чёрное дерево
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
class rbtree
{
  typedef rbtree_node<Value> node_type;
  public:
    typedef typename Allocator::template rebind<node_type>::other allocator_type;
    typedef size_t                                                size_type;
    typedef ptrdiff_t                                             difference_type;
    typedef Key                                                   key_type;
    typedef Value                                                 value_type;
    typedef value_type*                                           pointer;
    typedef const value_type*                                     const_pointer;
    typedef value_type&                                           reference;
    typedef const value_type&                                     const_reference;
    
    typedef rbtree_iterator<value_type,reference,pointer>             iterator;
    typedef rbtree_iterator<value_type,const_reference,const_pointer> const_iterator;
    typedef stl::reverse_iterator<iterator>                           reverse_iterator;
    typedef stl::reverse_iterator<const_iterator>                     const_reverse_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////////////                
    explicit rbtree  (const allocator_type& = allocator_type::create ());
             rbtree  (const Compare&,const allocator_type& = allocator_type::create ());
             rbtree  (const rbtree&);
             ~rbtree ();
             
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Аллокатор
///////////////////////////////////////////////////////////////////////////////////////////////////////////                     
    allocator_type get_allocator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////////////        
    rbtree& operator = (const rbtree&);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Функции общего назначения
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    size_type size     () const;
    size_type max_size () const; 
    bool      empty    () const;    
    Compare   key_comp () const;

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
///Вставка
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    pair<iterator,bool> insert_unique (const value_type&);
    iterator            insert_equal  (const value_type&);
    iterator            insert_unique (iterator position,const value_type&);
    iterator            insert_equal  (iterator position,const value_type&);
   
    template <class Iter> void insert_unique (Iter first,Iter last);
    template <class Iter> void insert_equal  (Iter first,Iter last);
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
    void      erase (iterator);
    size_type erase (const key_type&);
    void      erase (iterator first,iterator last);
    void      erase (const key_type* first,const key_type* last);
    void      clear ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    iterator       find        (const key_type&);
    const_iterator find        (const key_type&) const;
    size_type      count       (const key_type&) const;
    iterator       lower_bound (const key_type&);
    const_iterator lower_bound (const key_type&) const;
    iterator       upper_bound (const key_type&);
    const_iterator upper_bound (const key_type&) const;
    
    pair<iterator,iterator>             equal_range (const key_type&);
    pair<const_iterator,const_iterator> equal_range (const key_type&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух деревьев
///////////////////////////////////////////////////////////////////////////////////////////////////////////                  
    void swap (rbtree&);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Операции сравнения
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const rbtree&) const;
    bool operator != (const rbtree&) const;
    bool operator <  (const rbtree&) const;
    bool operator >  (const rbtree&) const;
    bool operator <= (const rbtree&) const;
    bool operator >= (const rbtree&) const;        
  
  private:  
    typedef rbtree_node_base*        link_type;    
    typedef const rbtree_node_base*  const_link_type;
    
    enum Color { RED = rbtree_node_base::RED, BLACK = rbtree_node_base::BLACK };
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение / освобождение памяти под узлы
///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    link_type allocate_node   ();
    void      deallocate_node (link_type);
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие ключа
///////////////////////////////////////////////////////////////////////////////////////////////////////////      
    static const key_type& key (const value_type&);
    static const key_type& key (const_link_type);
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / копирование / удаление узлов
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
    link_type create_node  (const value_type&);
    link_type clone_node   (link_type);
    void      destroy_node (link_type);
    link_type copy         (link_type,link_type);
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Инициализация дерева
///////////////////////////////////////////////////////////////////////////////////////////////////////////           
    void empty_init ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вставка / удаление 
///////////////////////////////////////////////////////////////////////////////////////////////////////////                   
    iterator _insert (link_type x,link_type y,const value_type& v);
    void     _erase  (link_type x);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    link_type _find        (const key_type&) const;
    link_type _lower_bound (const key_type&) const;
    link_type _upper_bound (const key_type&) const;
    
  private:    
    allocator_type   allocator;
    size_type        node_count;
    Compare          compare;
    rbtree_node_base header;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен содержимого двух деревьев
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key,class Value,class KeyOfValue,class Compare,class Allocator> 
void swap (rbtree<Key,Value,KeyOfValue,Compare,Allocator>&,rbtree<Key,Value,KeyOfValue,Compare,Allocator>&);

#include <stl/detail/tree.inl>

#ifdef _MSC_VER 
  #pragma warning (default:4512) //'class' : assignment operator could not be generated
#endif

}

#endif
