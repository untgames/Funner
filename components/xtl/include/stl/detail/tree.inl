/*
    Минимальный / максимальный узлы в поддеревьях
*/

inline rbtree_node_base* rbtree_node_base::min () const
{
  const Node* x = this;  
  
  for (;x->left;x=x->left);
  
  return (Node*)x;
}

inline rbtree_node_base* rbtree_node_base::max () const
{
  const Node* x = this;
  
  for (;x->right;x=x->right);
  
  return (Node*)x;  
}

/*
      Следующий / предыдущий узел по порядку следования
*/

inline rbtree_node_base* rbtree_node_base::next () const
{
  if (right) 
  {
    const Node* x;  
  
    for (x=right;x->left;x=x->left);
    
    return (Node*)x;
  }
  else 
  {   
    const Node *x,*y;
   
    for (x=this,y=parent;x==y->right;x=y,y=y->parent);

    return x->right == y ? (Node*)x : (Node*)y;
  }
}

inline rbtree_node_base* rbtree_node_base::prev () const
{
  if      (color == RED && parent->parent == this) return right;
  else if (left) 
  {
    const Node* y;
    
    for (y=left;y->right;y=y->right);

    return (Node*)y;
  }
  else 
  { 
    const Node *x,*y;
    
    for (y=parent,x=this;x==y->left;x=y,y=y->parent);

    return (Node*)y;
  }  
}

/*
    Вращения
*/

inline void rbtree_node_base::rotate_left (Node*& root)
{ 
  Node* y = right;  
  right   = y->left;
  
  if (y->left)
    y->left->parent = this;
    
  y->parent = parent;

  if      (this == root)         root          = y;
  else if (this == parent->left) parent->left  = y;
  else                           parent->right = y;
  
  y->left = this;
  parent  = y;  
}

inline void rbtree_node_base::rotate_right (Node*& root)
{
  Node* y = left;
  left    = y->right;
  
  if (y->right)
    y->right->parent = this;
    
  y->parent = parent;

  if      (this == root)          root          = y;
  else if (this == parent->right) parent->right = y;
  else                            parent->left  = y;
  
  y->right = this;
  parent   = y;
}

/*
    Перебалансировка
*/

inline void rbtree_node_base::rebalance (Node*& root)
{
  color = RED;
  
  for (Node* x=this;x!=root && x->parent->color==RED;)
  {  
    if (x->parent == x->parent->parent->left) 
    {
      Node* y = x->parent->parent->right;
      
      if (y && y->color == RED) 
      {
        x->parent->color         = BLACK;
        y->color                 = BLACK;
        x->parent->parent->color = RED;
        x                        = x->parent->parent;
      }
      else 
      {
        if (x == x->parent->right) 
        {
          x = x->parent;
          x->rotate_left (root);
        }
        
        x->parent->color         = BLACK;
        x->parent->parent->color = RED;
        
        x->parent->parent->rotate_right (root);
      }
    }
    else 
    {
      Node* y = x->parent->parent->left;
      
      if (y && y->color == RED) 
      {
        x->parent->color         = BLACK;
        y->color                 = BLACK;
        x->parent->parent->color = RED;
        x                        = x->parent->parent;
      }
      else 
      {
        if (x == x->parent->left) 
        {
          x = x->parent;
          x->rotate_right (root);
        }
        
        x->parent->color         = BLACK;
        x->parent->parent->color = RED;
        
        x->parent->parent->rotate_left (root);
      }
    }
  }
  
  root->color = BLACK;
}

inline rbtree_node_base* rbtree_node_base::rebalance_for_erase (Node*& root,Node*& leftmost,Node*& rightmost)
{
  Node *y = this, *x = 0, *x_parent = 0;

  if      (!y->left)  x = y->right; //this has at most one non-null child. y == this. x might be null.             
  else if (!y->right) x = y->left;  //this has exactly one non-null child. y == this. x is not null.
  else 
  {            
    for (y=y->right;y->left;y=y->left); //this has two non-null children.  Set y to this's successor. x might be null.
    x = y->right;
  }
    
  if (y != this) //relink y in place of this. y is this's successor
  {          
    left->parent = y; 
    y->left      = left;
    
    if (y != right) 
    {
      x_parent = y->parent;
      
      if (x) x->parent = y->parent;
        
      y->parent->left  = x;        //y must be a child of left
      y->right         = right;
      right->parent    = y;
    }
    else x_parent = y;  
    
    if      (root == this)         root          = y;
    else if (parent->left == this) parent->left  = y;
    else                           parent->right = y;
    
    y->parent = parent;
    
    stl::swap (y->color,color);
    
    y = this; //y now points to node to be actually deleted    
  }
  else //y == this
  {                        
    x_parent = y->parent;
    
    if (x) 
      x->parent = y->parent;   
    
    if      (root == this)         root          = x;
    else if (parent->left == this) parent->left  = x;
    else                           parent->right = x;
    
    if (leftmost == this)  leftmost  = right ? x->min () : parent;
    if (rightmost == this) rightmost = left ? x->max () : parent;
  }
  
  if (y->color != RED) 
  {
    while (x != root && (!x || x->color == BLACK))
      if (x == x_parent->left) 
      {
        Node* w = x_parent->right;
        
        if (w->color == RED) 
        {
          w->color        = BLACK;
          x_parent->color = RED;
          
          x_parent->rotate_left (root);
          
          w = x_parent->right;
        }
        
        if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK))
        {
          w->color = RED;
          x        = x_parent;
          x_parent = x_parent->parent;
        } 
        else 
        {
          if (!w->right || w->right->color == BLACK) 
          {
            if (w->left) 
              w->left->color = BLACK;
              
            w->color = RED;
            
            w->rotate_right (root);
            
            w = x_parent->right;
          }
          
          w->color        = x_parent->color;          
          x_parent->color = BLACK;
          
          if (w->right) 
            w->right->color = BLACK;
          
          x_parent->rotate_left (root);
          
          break;
        }
      } 
      else //same as above, with right <-> left.
      {                  
        Node* w = x_parent->left;
        
        if (w->color == RED) 
        {
          w->color        = BLACK;
          x_parent->color = RED;
          
          x_parent->rotate_right (root);
          
          w = x_parent->left;
        }
        
        if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) 
        {
          w->color = RED;
          x        = x_parent;
          x_parent = x_parent->parent;
        } 
        else 
        {
          if (!w->left || w->left->color == BLACK) 
          {
            if (w->right) 
              w->right->color = BLACK;
            
            w->color = RED;
            
            w->rotate_left (root);
            
            w = x_parent->left;
          }
          
          w->color        = x_parent->color;
          x_parent->color = BLACK;
          
          if (w->left) 
            w->left->color = BLACK;
          
          x_parent->rotate_right (root);
          
          break;
        }
      }
      
    if (x) 
      x->color = BLACK;
  }
  
  return y;
}

/*
    Итератор красно-чёрного дерева
*/

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr>::rbtree_iterator ()
  : node (0)
{ }

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr>::rbtree_iterator (link_type link)
  : node (link)
{ }

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr>::rbtree_iterator (const iterator& i)
  : node (((rbtree_iterator&)i).node)
{ }
    
template <class Val,class Ref,class Ptr>
inline typename rbtree_iterator<Val,Ref,Ptr>::reference rbtree_iterator<Val,Ref,Ptr>::operator * () const
{
  return ((rbtree_node<Val>*)node)->value;
}

template <class Val,class Ref,class Ptr>
inline typename rbtree_iterator<Val,Ref,Ptr>::pointer rbtree_iterator<Val,Ref,Ptr>::operator -> () const
{
  return &((rbtree_node<Val>*)node)->value;
}

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr>& rbtree_iterator<Val,Ref,Ptr>::operator ++ ()
{
  return node = node->next (), *this;
}

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr>& rbtree_iterator<Val,Ref,Ptr>::operator -- ()
{
  return node = node->prev (), *this;
}

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr> rbtree_iterator<Val,Ref,Ptr>::operator ++ (int)
{
  rbtree_iterator t = *this;
  
  node = node->next ();
  
  return t;
}

template <class Val,class Ref,class Ptr>
inline rbtree_iterator<Val,Ref,Ptr> rbtree_iterator<Val,Ref,Ptr>::operator -- (int)
{
  rbtree_iterator t = *this;
  
  node = node->prev ();
  
  return t;
}

template <class Val,class Ref,class Ptr>
inline bool rbtree_iterator<Val,Ref,Ptr>::operator == (const rbtree_iterator& i) const
{
  return i.node == node;
}

template <class Val,class Ref,class Ptr>
inline bool rbtree_iterator<Val,Ref,Ptr>::operator != (const rbtree_iterator& i) const
{
  return i.node != node;
}

/*
    Получение unqualified итератора
*/

template <class Val,class Ref,class Ptr>
inline typename rbtree_iterator<Val,Ref,Ptr>::iterator rbtree_iterator<Val,Ref,Ptr>::get_unqualified_iterator () const
{
  return iterator (node);
}


/*
    Конструкторы
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::empty_init ()
{
  header.color  = RED; //used to distinguish header from root, in iterator.operator++
  header.parent = 0;
  header.left   = header.right = &header;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rbtree (const allocator_type& _allocator)
  : allocator (_allocator), node_count (0)
{
  empty_init ();  
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rbtree (const Compare& cmp,const allocator_type& _allocator)
  : allocator (_allocator), node_count (0), compare (cmp)
{
  empty_init ();
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rbtree (const rbtree& x)
  : allocator (x.allocator)
{
  if (x.header.parent)
  {
    header.color  = RED;    
    header.parent = copy (x.header.parent,&header);
    header.left   = header.parent->min ();
    header.right  = header.parent->max ();
  }
  else empty_init ();
  
  node_count = x.node_count;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline rbtree<Key,Value,KeyOfValue,Compare,Allocator>::~rbtree ()
{
  clear ();
}

/*
    Аллокатор
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::allocator_type
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::get_allocator () const
{
  return allocator;
}

/*
    Функции внутреннего использования
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::allocate_node ()
{
  return allocator.allocate (1);
} 

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::deallocate_node (link_type p)
{
  allocator.deallocate ((node_type*)p,1);
} 

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::create_node (const value_type& x)
{
  node_type* tmp = (node_type*)allocate_node ();
  
  try 
  {
    construct (&tmp->value,x);
  }
  catch (...)
  {
    deallocate_node (tmp);
  }

  return tmp;
} 

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type 
inline rbtree<Key,Value,KeyOfValue,Compare,Allocator>::clone_node (link_type x)
{
  link_type tmp = create_node (((node_type*)x)->value);
  
  tmp->color = x->color;
  tmp->left  = tmp->right = 0;

  return tmp;
} 

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::destroy_node (link_type p)
{
  destroy (&((node_type*)p)->value);
  deallocate_node (p);
}
  
template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::copy (link_type x,link_type p)
{
  link_type top = clone_node (x);
  
  top->parent = p;
 
  try 
  {
    if (x->right)
      top->right = copy (x->right,top);
      
    p = top;
    x = x->left;

    while (x) 
    {
      link_type y = clone_node (x);
      
      p->left   = y;
      y->parent = p;
      
      if (x->right)
        y->right = copy (x->right,y);
        
      p = y;
      x = x->left;
    }
  }
  catch (...)
  {
    erase (top);
    throw;
  }

  return top;
} 

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline const Key& rbtree<Key,Value,KeyOfValue,Compare,Allocator>::key (const value_type& x)
{
  return KeyOfValue () (x);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline const Key& rbtree<Key,Value,KeyOfValue,Compare,Allocator>::key (const_link_type x)
{
  return KeyOfValue () (((const node_type*)x)->value);
}

/*
    Функции общего назначения
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::size_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::size () const
{
  return node_count;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::size_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::max_size () const
{
  return allocator.max_size ();
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::empty () const
{
  return node_count == 0;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline Compare rbtree<Key,Value,KeyOfValue,Compare,Allocator>::key_comp () const
{
  return compare;
}

/*
    Итераторы
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::begin ()
{
  return header.left;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::begin () const
{
  return header.left;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::end ()
{
  return &header;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::end () const
{
  return (link_type)&header;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::reverse_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_reverse_iterator rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rbegin () const
{
  return const_reverse_iterator (end ());
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::reverse_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rend ()
{
  return reverse_iterator (begin ());
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_reverse_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::rend () const
{
  return const_reverse_iterator (begin ());
} 

/*
    Копирование
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
rbtree<Key,Value,KeyOfValue,Compare,Allocator>& 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator = (const rbtree<Key,Value,KeyOfValue,Compare,Allocator>& x)
{
  if (this == &x)
    return *this;

  clear();
  
  node_count = 0;
  compare    = x.compare;
  
  if (!x.header.parent) 
  {
    header.parent = 0;
    header.left   = header.right = &header;
  }
  else 
  {
    header.parent = copy (x.header.parent,&header);
    header.left   = header.parent->min ();
    header.right  = header.parent->max ();
    node_count     = x.node_count;
  }
    
  return *this;
}

/*
    Вставка
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::_insert (link_type x,link_type y,const value_type& v)
{
  link_type z = create_node (v);  

  if (y == &header || x || compare (key (v),key (y))) //вставка слева от y
  {    
    y->left = z;
                                      
    if      (y == &header)     y->right    = y->parent = z; //z становится корнем дерева
    else if (y == header.left) header.left = z; //добавлен новый минимальный элемент (z < min)
  }
  else  //вставка справа от y
  {
    y->right = z;   

    if (y == header.right)
      header.right = z; //добавлен новый максимальный элемент (z > max)
  }

  z->parent = y;
  z->left   = z->right = 0;
    
  z->rebalance (header.parent); //перебалансировка дерева
  
  ++node_count;
  
  return z;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_equal (const value_type& v)
{
  link_type x,y;
  
  for (y=&header,x=header.parent;x;y=x,x=compare (key (v),key (x))?x->left:x->right); 
  
  return _insert (x,y,v); //x always NULL
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_equal (iterator position,const value_type& v)
{
  if (position.node == header.left) //вставка в начало
  {
    return node_count && compare (key (v),key (position.node)) ? _insert (position.node,position.node,v) : 
                                                                 insert_equal (v);
  } 
  else if (position.node == &header) //вставка в конец
  {
    return !compare (key (v),key (header.right)) ? _insert (0,header.right,v) : insert_equal (v);
  }
  else //попытка вставить в произвольную позицию дерева
  {
    iterator before = position;
    
    --before;
    
    if (!compare (key (v),key (before.node)) && !compare (key (position.node),key (v)))
    {
      return !before.node->right ? _insert (0,before.node,v) : _insert (position.node,position.node,v);
    } 
    else return insert_equal (v);
  }
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
pair<typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator,bool>
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_unique (const value_type& v)
{
  link_type y = &header, x = y->parent;
  
  bool comp = true;
  
  while (x) 
  {
    y    = x;
    comp = compare (key (v),key (x));
    x    = comp ? x->left : x->right;
  }
  
  iterator j = y;
  
  if (comp)
  {
    if (j == begin ()) return pair<iterator,bool> (_insert (x,y,v),true);
    else               --j;
  }
          
  if (compare (key (j.node),key (v)))
    return pair<iterator,bool> (_insert (x,y,v),true);
    
  return pair<iterator,bool> (j,false);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_unique (iterator position,const value_type& v)
{
  if (position.node == header.left) //вставка в начало
  { 
    return node_count && compare (key (v), key (position.node)) ? _insert (position.node,position.node,v) :
                                                                  insert_unique (v).first;
  } 
  else if (position.node == &header) //вставка в конец
  { 
    return compare (key (header.right),key (v)) ? _insert (0,header.right,v) : insert_unique (v).first;
  } 
  else  //вставка в произвольную позицию
  {
    iterator before = position;
    
    --before;
    
    if (compare (key (before.node),key (v)) && compare (key (v),key (position.node))) 
    {
      return !before.node->right ? _insert (0,before.node,v) : _insert (position.node,position.node,v);
    }
    else return insert_unique (v).first;
  }
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator> template <class Iter>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_equal (Iter first,Iter last)
{
  for (;first!=last;++first) insert_equal (*first);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator> template <class Iter>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::insert_unique (Iter first,Iter last)
{
  for (;first!=last;++first) insert_unique (*first);
}

/*
    Удаление
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::_erase (link_type x) //удаление без перебалансировки
{                                
  while (x) 
  {
    _erase (x->right); //расписать рекурсию!!!
    
    link_type y = x->left;
    
    destroy_node (x);
    
    x = y;
  }
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::erase (iterator first,iterator last)
{
  if (first == begin () && last == end ()) 
  {
    clear ();
    return;
  }  
  
  while (first != last) erase (first++);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::erase (const key_type* first,const key_type* last)
{
  while (first != last) erase (*first++);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::erase (iterator position)
{
  destroy_node ((link_type)position.node->rebalance_for_erase (header.parent,header.left,header.right));  
  --node_count;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::size_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::erase (const key_type& x)
{
  pair<iterator,iterator> p = equal_range (x);
    
  if (p.first == begin () && p.second == end ()) 
  {
    size_type n = node_count;
    clear ();
    return n;
  } 
  
  size_type n = 0;
  
  for (iterator i=p.first;i!=p.second;n++) erase (i++);
  
  return n;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::clear() 
{
  if (!node_count)
    return;    

  _erase (header.parent);
  
  header.parent = 0;
  header.right  = header.left = &header;
  node_count    = 0;
}      

/*
    Поиск
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::_find (const key_type& k) const
{
  link_type y = (link_type)&header,       //последний узел не меньший k
            x = header.parent; //текущий узел
            
  while (x) 
    if (!compare (key (x),k)) y = x, x = x->left;
    else                      x = x->right;
    
  return y == &header || compare (k,key (y)) ? (link_type)&header : y;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::_lower_bound (const key_type& k) const
{
  link_type y = (link_type)&header,       //последний узел не меньший k
            x = header.parent; //текущий узел

  while (x) 
    if (!compare (key (x),k)) y = x, x = x->left;
    else                      x = x->right;

  return y;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::link_type
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::_upper_bound (const key_type& k) const
{
  link_type y = (link_type)&header,       //последний узел не меньший k
            x = header.parent; //текущий узел

   while (x) 
     if (compare (k,key (x))) y = x, x = x->left;
     else                     x = x->right;

  return y;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::find (const key_type& k)
{
  return _find (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::find (const key_type& k) const
{
  return _find (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::size_type 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::count (const key_type& k) const
{
  pair<const_iterator,const_iterator> p = equal_range (k);
  
  return distance (p.first,p.second);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::lower_bound (const key_type& k)
{
  return _lower_bound (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::lower_bound (const key_type& k) const
{
  return _lower_bound (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::upper_bound (const key_type& k)
{
  return _upper_bound (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator 
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::upper_bound (const key_type& k) const
{
  return _upper_bound (k);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline pair<typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator,
            typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::iterator>
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::equal_range (const key_type& k)
{
  return pair<iterator,iterator> (lower_bound (k),upper_bound (k));
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline pair<typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator,
            typename rbtree<Key,Value,KeyOfValue,Compare,Allocator>::const_iterator>
rbtree<Key,Value,KeyOfValue,Compare,Allocator>::equal_range (const key_type& k) const
{
  return pair<const_iterator,const_iterator> (lower_bound (k),upper_bound (k));
}

/*
    Обмен содержимого двух деревьев
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
void rbtree<Key,Value,KeyOfValue,Compare,Allocator>::swap (rbtree& t)
{   
  if (allocator == t.allocator)
  {
    if (node_count && t.node_count)
    {  
      stl::swap (header,t.header);    
      stl::swap (header.parent->parent,t.header.parent->parent);   
    }
    else if (node_count)
    { 
      t.header = header;
      t.header.parent->parent = &t.header;
      
      empty_init ();
    }
    else if (t.node_count)
    {  
      header = t.header;
      header.parent->parent = &header;
      
      t.empty_init ();  
    }

    stl::swap (node_count,t.node_count);
    stl::swap (compare,t.compare);      
  }
  else
  {
    rbtree tmp = *this;
    *this      = t;
    t          = tmp;
  }
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline void swap 
 (rbtree<Key,Value,KeyOfValue,Compare,Allocator>& a,
  rbtree<Key,Value,KeyOfValue,Compare,Allocator>& b)
{
  a.swap (b);
}

/*
    Операции сравнения
*/

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator == (const rbtree& x) const
{
  return size () == x.size () && equal (begin (),end (),x.begin ());
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator < (const rbtree& x) const
{
  return lexicographical_compare (begin (),end (),x.begin (),x.end ());
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator != (const rbtree& x) const
{
  return !(x == *this);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator > (const rbtree& x) const
{
  return x < *this;
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator <= (const rbtree& x) const
{
  return !(x < *this);
}

template <class Key,class Value,class KeyOfValue,class Compare,class Allocator>
inline bool rbtree<Key,Value,KeyOfValue,Compare,Allocator>::operator >= (const rbtree& x) const
{
  return !(*this < x);
}
