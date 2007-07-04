#include <common/heap.h>
#include "shared.h"

using namespace common;

//#define NO_BALANCE

/*
    Работа с деревом блоков среднего размера
    Дерево свободных блоков представляет собой красно-чёрное дерево
*/

MediumBlockTree::MediumBlockTree ()
{
  root            = NULL;
  min_block_size  = (size_t)-1;
  max_block_size  = 0;
  nodes_count     = 0;
}

MediumBlock* MediumBlockTree::FindSmallestBlock (size_t size,AllocNode* node)
{
  //root является корректным узлом
  
  MediumBlock* block = root, *result = NULL; 
  size_t       steps = 1;
    
  for (;;steps++)
  {              
    if (block->size == size)
    {
      result = block;
      break;
    }
    
    if (size > block->size)
    {      
      if (!block->right)
        break;

      block = block->right;
    }    
    else
    {
      result = block;
      
      if (!block->left)
        break;
      
      block = block->left;
    }
  }
  
  node->stat.medium_search_count++;
  node->stat.medium_search_steps_count += steps;
    
  if (steps > node->stat.medium_max_search_steps_count)
    node->stat.medium_max_search_steps_count = steps;    
    
  return result;  
}

inline void MediumBlockTree::RotateLeft (MediumBlock* node)
{ 
  MediumBlock *right = node->right, *parent = node->parent;
  
  if      (right->left)          right->left->parent = node;
  if      (node == root)         root                = right;
  else if (node == parent->left) parent->left        = right;
  else                           parent->right       = right;

  node->right   = right->left;
  right->parent = node->parent;      
  right->left   = node;
  node->parent  = right;
}

inline void MediumBlockTree::RotateRight (MediumBlock* node)
{
  MediumBlock *left = node->left, *parent = node->parent;
    
  if      (left->right)           left->right->parent = node;
  if      (node == root)          root                = left;
  else if (node == parent->right) parent->right       = left;
  else                            parent->left        = left;

  node->left   = left->right;        
  left->parent = parent;    
  left->right  = node;
  node->parent = left;  
}

void MediumBlockTree::InsertBlock (MediumBlock* new_node)
{ 
  size_t      size    = new_node->size;
  MediumBlock *node   = root,
              *parent = NULL;              

  while (node)
  {
    if (node->size == size)
    {
      new_node->next_free = node->next_free;
      new_node->prev_free = node;
      
      if (node->next_free)
        node->next_free->prev_free = new_node;

      node->next_free = new_node;      

      return;
    }
      
    parent = node;
    node   = size < node->size ? node->left : node->right;
  }
  
  nodes_count++;
  
  if (size < min_block_size) min_block_size = size;
  if (size > max_block_size) max_block_size = size;

  new_node->parent    = parent;
  new_node->left      = NULL;
  new_node->right     = NULL;
  new_node->next_free = NULL;
  new_node->prev_free = NULL;
  new_node->color     = RED;
  
  if (parent)
  {
    if (size < parent->size) parent->left  = new_node;
    else                     parent->right = new_node;
  }
  else root = new_node; //возможно данный случай следует вынести      
  
#ifdef NO_BALANCE
  return;
#endif
  
  //балансировка дерева
  
  node = new_node;  
  
  while (node != root && node->parent->color == RED)
  {
    parent = node->parent;    
    
    if (parent == parent->parent->left)
    {
      MediumBlock* uncle = parent->parent->right;      
      
      if (uncle && uncle->color == RED)
      {
        parent->color         = BLACK;
        uncle->color          = BLACK;
        parent->parent->color = RED;
        node                  = parent->parent;
      } 
      else 
      {
        if (node == parent->right) 
        {          
          RotateLeft (node=node->parent);
          
          parent = node->parent;          
        }

        parent->color         = BLACK;
        parent->parent->color = RED;
        
        RotateRight (parent->parent);
      }
    } 
    else 
    {
      MediumBlock *uncle = parent->parent->left;      
      
      if (uncle && uncle->color == RED) 
      {
        parent->color         = BLACK;
        uncle->color          = BLACK;
        parent->parent->color = RED;
        node                  = parent->parent;
      } 
      else 
      {
        if (node == parent->left) 
        {          
          RotateRight (node=node->parent);
          
          parent = node->parent;          
        }
        
        parent->color         = BLACK;
        parent->parent->color = RED;
        
        RotateLeft (parent->parent);
      }
    }
  }

  root->color = BLACK;
}

void MediumBlockTree::RemoveBlock (MediumBlock* node)
{
  if (node->prev_free) //удаление данного блока не изменяет структуру дерева свободных блоков
  {
    node->prev_free->next_free = node->next_free;

    if (node->next_free)
      node->next_free->prev_free = node->prev_free;

    return;
  }    
  
  if (node->next_free)
  {
    MediumBlock* new_node = node->next_free;

    new_node->left      = node->left;
    new_node->right     = node->right;
    new_node->parent    = node->parent;
    new_node->prev_free = NULL;
    new_node->color     = node->color;
    
    if (new_node->left)  new_node->left->parent  = new_node;
    if (new_node->right) new_node->right->parent = new_node;   
    
    if (new_node->parent)
    {
      if (new_node->parent->left == node) new_node->parent->left  = new_node;      
      else                                new_node->parent->right = new_node;
    }
    else root = new_node;    
    
    return;
  }
  
  nodes_count--;
  
  if (!nodes_count) //удаление последнего узла в дереве
  {
    min_block_size = (size_t)-1;
    max_block_size = 0;
    root           = NULL;
    
    return;
  }  
  
  if (node->size == min_block_size)
  {    
    if (node->right)
    {
      MediumBlock* min = node->right;

      for (;min->left;min=min->left);      

      min_block_size = min->size;                    
    }
    else  min_block_size = node->parent->size;      
  }
  else if (node->size == max_block_size)
  {    
    if (node->left)
    {
      MediumBlock* max = node->left;

      for (;max->right;max=max->right);

      max_block_size = max->size;
    }
    else max_block_size = node->parent->size;
  }

  MediumBlock *y = node, *x = 0, *x_parent = 0;

  if      (!y->left)  x = y->right; //this has at most one non-null child. y == this. x might be null.             
  else if (!y->right) x = y->left;  //this has exactly one non-null child. y == this. x is not null.
  else
  {              
    for (y=y->right;y->left;y=y->left); //this has two non-null children.  Set y to this's successor. x might be null.
    x = y->right;
  }
    
  if (y != node) //relink y in place of this. y is this's successor
  {              
    node->left->parent = y;
    y->left            = node->left;
    
    if (y != node->right)
    {
      x_parent = y->parent;
      
      if (x)
        x->parent = x_parent;
        
      y->parent->left     = x;        //y must be a child of left
      y->right            = node->right;
      node->right->parent = y;
    }
    else x_parent = y;  
    
    if      (root == node)               root                = y;
    else if (node->parent->left == node) node->parent->left  = y;
    else                                 node->parent->right = y;
    
    y->parent = node->parent;

    TreeColor color = y->color;
    y->color        = node->color;  
    node->color     = color;
    y               = node;
  }
  else  
  {
    x_parent = y->parent;
    
    if (x) 
      x->parent = x_parent;

    if      (root == node)               root                = x;
    else if (node->parent->left == node) node->parent->left  = x;
    else                                 node->parent->right = x;
  }  
  
#ifdef NO_BALANCE
  return;
#endif  
  
  if (y->color == RED)
    return;        

  while (x != root && (!x || x->color == BLACK))
  {    
    if (x == x_parent->left) 
    {
      MediumBlock* w = x_parent->right;
      
      if (w->color == RED) 
      {
        w->color        = BLACK;
        x_parent->color = RED;
        
        RotateLeft (x_parent);
        
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
          
          RotateRight (w);
          
          w = x_parent->right;
        }
        
        w->color        = x_parent->color;          
        x_parent->color = BLACK;
        
        if (w->right)
          w->right->color = BLACK;
        
        RotateLeft (x_parent);
        
        break;
      }
    } 
    else //same as above, with right <-> left.
    {                  
      MediumBlock* w = x_parent->left;
      
      if (w->color == RED) 
      {
        w->color        = BLACK;
        x_parent->color = RED;
        
        RotateRight (x_parent);
        
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
          
          RotateLeft (w);
          
          w = x_parent->left;
        }
        
        w->color        = x_parent->color;
        x_parent->color = BLACK;
        
        if (w->left) 
          w->left->color = BLACK;
        
        RotateRight (x_parent);
        
        break;
      }
    }
  }
    
  if (x)
    x->color = BLACK;    
}
