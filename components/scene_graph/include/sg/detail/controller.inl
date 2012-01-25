/*
    Перебор контроллеров
*/

template <class T> inline
xtl::com_ptr<T> Controller::NextController ()
{
  for (xtl::com_ptr<Controller> i=NextController (); i; i=i->NextController ())
    if (T* casted_controller = dynamic_cast<T*> (&*i))
      return casted_controller;
      
  return xtl::com_ptr<T> ();
}

template <class T>
inline xtl::com_ptr<T> Controller::PrevController ()
{
  for (xtl::com_ptr<Controller> i=PrevController (); i; i=i->PrevController ())
    if (T* casted_controller = dynamic_cast<T*> (&*i))
      return casted_controller;
      
  return xtl::com_ptr<T> ();      
}

template <class T>
inline xtl::com_ptr<const T> Controller::NextController () const
{
  return const_cast<Controller&> (*this).NextController<T> ();
}

template <class T>
inline xtl::com_ptr<const T> Controller::PrevController () const
{
  return const_cast<Controller&> (*this).PrevController<T> ();
}
