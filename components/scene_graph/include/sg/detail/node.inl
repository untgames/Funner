/*
    NodeUpdateLock
*/

inline NodeUpdateLock::NodeUpdateLock ()
  : node (0)
  {}

inline NodeUpdateLock::NodeUpdateLock (Node& in_node)
  : node (&in_node)
{
  node->AddRef ();
  node->BeginUpdate ();
}

inline NodeUpdateLock::NodeUpdateLock (const NodeUpdateLock& lock)
  : node (lock.node)
{
  if (!node)
    return;

  node->AddRef ();
  node->BeginUpdate ();
}

inline NodeUpdateLock::~NodeUpdateLock ()
{
  if (!node)
    return;
    
  node->EndUpdate ();
  node->Release ();
}
    
inline NodeUpdateLock& NodeUpdateLock::operator = (const NodeUpdateLock& lock)
{
  if (&lock != this)
    NodeUpdateLock (lock).Swap (*this);
  
  return *this;
}

inline void NodeUpdateLock::Swap (NodeUpdateLock& lock)
{
  Node* tmp = node;
  node      = lock.node;
  lock.node = tmp;
}

inline void swap (NodeUpdateLock& a, NodeUpdateLock& b)
{
  a.Swap (b);
}

/*
    Попытка посещения объекта
*/

namespace detail
{

struct touch_on_default_action
{
  touch_on_default_action (bool& in_flag) : flag (&in_flag)  {}
  
  template <class T> void operator () (T&, Node::Visitor&) { *flag = true; }
  
  bool* flag;
};

}

template <class T>
inline bool Node::TryAccept (T& visited, Visitor& visitor)
{
  bool is_not_processed = false;
  
  visitor (visited, detail::touch_on_default_action (is_not_processed));

  return !is_not_processed;
}

/*
    Перебор контроллеров
*/

template <class T>
inline xtl::com_ptr<T> Node::FirstController ()
{
  for (xtl::com_ptr<Controller> i=FirstController (); i; i=i->NextController ())
    if (T* casted_controller = dynamic_cast<T*> (i.get ()))
      return casted_controller;
      
  return xtl::com_ptr<T> ();
}

template <class T>
inline xtl::com_ptr<T> Node::LastController ()
{
  for (xtl::com_ptr<Controller> i=LastController (); i; i=i->PrevController ())
    if (T* casted_controller = dynamic_cast<T*> (i.get ()))
      return casted_controller;
      
  return xtl::com_ptr<T> ();
}

template <class T>
inline xtl::com_ptr<const T> Node::FirstController () const
{
  return const_cast<Node&> (*this).FirstController<T> ();
}

template <class T>
inline xtl::com_ptr<const T> Node::LastController () const
{
  return const_cast<Node&> (*this).LastController<T> ();
}
