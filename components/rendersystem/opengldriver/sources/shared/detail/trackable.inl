/*
    Получение trackable
*/

inline const xtl::trackable& Trackable::GetTrackable () const
{
  return *this;
}

inline xtl::trackable& Trackable::GetTrackable ()
{
  return *this;
}

inline xtl::trackable& get_trackable (Trackable& t)
{
  return t.GetTrackable ();
}

/*
    Регистрация обработчика события удаления объекта
*/

inline xtl::connection Trackable::RegisterDestroyHandler (SlotType& handler)
{
  return xtl::trackable::connect_tracker (handler);
}

inline xtl::connection Trackable::RegisterDestroyHandler (FunctionType& handler)
{
  return xtl::trackable::connect_tracker (handler);
}
