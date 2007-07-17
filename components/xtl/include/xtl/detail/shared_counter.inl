namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс для подсчёта ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
class shared_counter
{
  public:
            shared_counter  () : use_counter (1), weak_counter (1) { }
    virtual ~shared_counter () { }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызывается когда use_count становится равным нулю
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void dispose () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызывается когда weak_count становится равным нулю
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void destroy () { delete this; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Возвращает указатель на удаляющий объект если его тип совпадает с аргументом
///////////////////////////////////////////////////////////////////////////////////////////////////        
    virtual void* get_deleter (const std::type_info&) = 0;
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///Увеличение/уменьшение числа ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void add_ref_copy ()
    {
      XTL_INTERLOCKED_INCREMENT (&use_counter);
    }
    
    void release ()
    {
      if (XTL_INTERLOCKED_DECREMENT (&use_counter) == 0)
      {
        dispose      ();
        weak_release ();
      }
    }

    bool add_ref_lock() // true on success
    {
      for( ;; )
      {
        long tmp = static_cast <const volatile long&> (use_counter);
        
        if (tmp == 0)
          return false;

        if (XTL_INTERLOCKED_COMPARE_EXCHANGE (&use_counter, tmp + 1, tmp) == tmp)
          return true;
      }
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Увеличение/уменьшение weak_count
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void weak_add_ref ()
    {
      XTL_INTERLOCKED_INCREMENT (&weak_counter);
    }

    void weak_release ()
    {
      if (XTL_INTERLOCKED_DECREMENT (&weak_counter) == 0)
        destroy ();
    }

    long use_count () const
    {
      return static_cast<const volatile long&> (use_counter);
    }
    
  private:
    long use_counter;
    long weak_counter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация класса для подсчёта ссылок. Удаление стандартно
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class std_shared_counter: public shared_counter
{
  public:
    explicit std_shared_counter (T* in_ptr): ptr (in_ptr) {}

    void  dispose     () { delete ptr; }
    void* get_deleter (const std::type_info&) { return 0; }

  private:
    T* ptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация класса для подсчёта ссылок. Удаление deleter (ptr)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ptr, class Deleter> class user_shared_counter: public shared_counter
{
  public:
    user_shared_counter (Ptr in_ptr, Deleter in_deleter): ptr (in_ptr), deleter (in_deleter) {}

    void  dispose     () { deleter (ptr); }
    void* get_deleter (const std::type_info& ti) { return ti == typeid (Deleter) ? &deleter : 0; }

  private:
    Ptr     ptr;
    Deleter deleter;
};

}
