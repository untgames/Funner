namespace detail
{

/*
    Обработчики удаления объектов пары trackable-trackable
*/

class dual_trackable_handler;

template <size_t selector>
class dual_trackable_handler_wrapper
{
  public: 
    dual_trackable_handler_wrapper (dual_trackable_handler& in_handler) : handler (in_handler) {}

    void operator () () const;

  private:
    dual_trackable_handler& handler;
};

#ifdef _MSC_VER
  #pragma warning (push) //'this' : used in base member initializer list
  #pragma warning (disable : 4355)
#endif

class dual_trackable_handler: public connection_impl
{ 
  public:
    dual_trackable_handler (const trackable::function_type& in_handler, trackable& primary, trackable& secondary) :
      handler (in_handler),
      primary_slot (dual_trackable_handler_wrapper<1> (*this)),
      secondary_slot (dual_trackable_handler_wrapper<2> (*this)),
      is_blocked (false),
      is_connected (true)
    {
      primary.connect_tracker (primary_slot);
      secondary.connect_tracker (secondary_slot);
    }

    void notify (const dual_trackable_handler_wrapper<1>&)
    {    
      if (!is_blocked)
      {
        try
        {
          handler ();
        }
        catch (...)
        {
        }
      }

      dual_trackable_handler::disconnect ();
    }
    
    void notify (const dual_trackable_handler_wrapper<2>&)
    {
      dual_trackable_handler::disconnect ();
    }
    
    void block   () { is_blocked = true; }
    void unblock () { is_blocked = false; }
    bool blocked () { return is_blocked; }

    bool connected  () { return is_connected; }

    void disconnect ()
    {
      if (!is_connected)
        return;

      primary_slot.disconnect ();
      secondary_slot.disconnect ();
      handler.clear ();

      is_connected = false;

      release ();
    }

  private:
    trackable::function_type handler;
    trackable::slot_type     primary_slot, secondary_slot;
    bool                     is_blocked;
    bool                     is_connected;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

template <size_t selector>
inline void dual_trackable_handler_wrapper<selector>::operator () () const
{
  handler.notify (*this);
}

/*
    Дуальное соединение между двумя trackable
*/

struct trackable_connection: public connection
{
  public:
    trackable_connection (connection_impl* impl) : connection (impl) {}
};

/*
    Функтор разрыва соединения
*/

struct connection_disconnector
{
  connection_disconnector (const connection& in_c) : c (in_c) {}
  
  void operator () () { c.disconnect (); }

  connection c;
};

}

/*
    Деструктор
*/

inline trackable::~trackable ()
{
  try
  {
    sig ();
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Добавление обработчиков удаления объекта
*/

inline connection trackable::connect_tracker (const function_type& fn)
{
  return sig.connect (fn);
}

inline connection trackable::connect_tracker (slot_type& s)
{
  return sig.connect (s);
}

inline connection trackable::connect_tracker (const function_type& fn, trackable& owner)
{
  return detail::trackable_connection (new detail::dual_trackable_handler (fn, *this, owner));
}

//разрыв соединения при удалении объекта
inline connection trackable::connect_tracker (const connection& c)
{
  return connect_tracker (function_type (detail::connection_disconnector (c)));
}

template <class Fn>
inline void trackable::disconnect_tracker (Fn fn)
{
  sig.disconnect (fn);
}

inline void trackable::disconnect_all_trackers ()
{
  sig.disconnect_all ();
}

/*
    Количество обработчиков
*/

inline size_t trackable::num_trackers () const
{
  return sig.num_slots ();
}
