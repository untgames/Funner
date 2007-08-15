namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void addref () { ref_count++; }
    
    void release ()
    {
      if (!--ref_count)
        delete this;
    }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка факта наличя соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected () const { return is_connected; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокирование соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void block   ()       { is_blocked = true;  }
    void unblock ()       { is_blocked = false; }
    bool blocked () const { return is_blocked; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокирование разрыва соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void lock ()
    { 
      lock_count++;
    }

    void unlock ()
    {
      if (!--lock_count && wait_disconnect)
        disconnect ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void disconnect ()
    {
      if (!is_connected)
        return;

      if (lock_count)
      {
        wait_disconnect = true;
        return;
      }

      is_connected    = false;
      wait_disconnect = false;

      do_disconnect (); //nothrow      
    }

  protected:
    connection_impl  (bool in_connected=true) 
       : ref_count (1), lock_count (0), is_connected (in_connected), is_blocked (false), wait_disconnect (false) {}
       
    virtual ~connection_impl () {}

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настоящий разрыв соединения реализуется в потомках
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void do_disconnect () = 0;

  private:
    size_t ref_count;
    size_t lock_count;
    bool   is_connected;    
    bool   is_blocked;
    bool   wait_disconnect;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пустое соединение
///////////////////////////////////////////////////////////////////////////////////////////////////
class empty_connection_impl: public connection_impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение единственного экземпляра пустого соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static connection_impl* instance_ptr () {
      static char buffer [sizeof (empty_connection_impl)];
      static connection_impl* empty = new (buffer) empty_connection_impl;

      return empty;
    }

  private:
    empty_connection_impl () : connection_impl (false) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрыв соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void do_disconnect () {}
};

}

/*
    connection
*/

inline connection::connection ()
  : impl (detail::empty_connection_impl::instance_ptr ())
{
  impl->addref ();
}

inline connection::connection (detail::connection_impl* in_impl)
  : impl (in_impl)
{
  impl->addref ();
}
  
inline connection::connection (const connection& c)
  : impl (c.impl)
{
  impl->addref ();  
}

inline connection::~connection ()
{
  impl->release ();
}

inline connection& connection::operator = (const connection& c)
{
  connection (c).swap (*this);

  return *this;
}

inline bool connection::connected () const
{
  return impl->connected ();
}

inline void connection::disconnect ()
{
  impl->disconnect ();
}

inline void connection::block ()
{
  impl->block ();
}

inline void connection::unblock ()
{
  impl->unblock ();
}

inline bool connection::blocked () const
{
  return impl->blocked ();
}

inline bool connection::operator == (const connection& c) const
{
  return impl == c.impl;
}

inline bool connection::operator != (const connection& c) const
{
  return !(*this == c);
}

inline void connection::swap (connection& c)
{
  detail::connection_impl* tmp = impl;
  impl                         = c.impl;
  c.impl                       = tmp;
}

inline void swap (connection& a, connection& b)
{
  a.swap (b);
}

/*
    auto_conection
*/

inline auto_connection::auto_connection (const connection& c)
  : connection (c)
  {}

inline auto_connection::~auto_connection ()
{
  disconnect ();
}
   
inline auto_connection& auto_connection::operator = (const connection& c)
{
  auto_connection (c).swap (*this);
  
  return *this;
}
