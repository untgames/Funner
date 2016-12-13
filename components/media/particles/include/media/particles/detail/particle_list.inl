/*
    Particle
*/

inline ParticleNode::ParticleNode ()
  : prev ()
  , next ()
  //pool_link should not be initialized
 { }

inline Particle::Particle ()
  : creation_time (0, 0)
  , lifetime ()
  , animation_frame ()
  , animation_frame_offset ()
 { }

/*
    Particle iterator
*/

template <class T>
inline ParticleIterator<T>::ParticleIterator ()
  : node ()
  { }

template <class T>
inline ParticleIterator<T>::ParticleIterator (ParticleNode* inNode)
  : node (inNode)
 { }  
 
template <class T>
ParticleIterator<T>::ParticleIterator (const iterator& i)
  : node (const_cast<ParticleNode*> (i.node))
 { }

template <class T>
ParticleIterator<T>& ParticleIterator<T>::operator = (const iterator& i)
{ 
  node = const_cast<ParticleNode*> (i.node);
  return *this;
}
    
template <class T> 
inline typename ParticleIterator<T>::reference ParticleIterator<T>::operator * () const
{
  return *static_cast<T*> (node);
}

template <class T> 
inline typename ParticleIterator<T>::pointer ParticleIterator<T>::operator -> () const
{
  return &**this;
}

template <class T> 
inline ParticleIterator<T>& ParticleIterator<T>::operator ++ ()
{
  return node = node->next, *this;
}

template <class T> 
inline ParticleIterator<T>& ParticleIterator<T>::operator -- ()
{
  return node = node->prev, *this;
}

template <class T> 
inline ParticleIterator<T> ParticleIterator<T>::operator ++ (int)
{
  ParticleIterator t = *this;
  
  node = node->next;
  
  return t;   
}

template <class T> 
inline ParticleIterator<T> ParticleIterator<T>::operator -- (int)
{
  ParticleIterator t = *this;
  
  node = node->prev;
  
  return t;
}
    
template <class T> 
inline bool ParticleIterator<T>::operator == (const ParticleIterator& i) const
{
  return node == i.node;
}

template <class T> 
inline bool ParticleIterator<T>::operator != (const ParticleIterator& i) const
{
  return !(*this == i);
}  

template <class T>
inline typename ParticleIterator<T>::iterator ParticleIterator<T>::get_unqualified_iterator () const
{
  return iterator (node);
}

template <class T>
inline bool ParticleIterator<T>::empty () const
{
  return node == 0;
}
