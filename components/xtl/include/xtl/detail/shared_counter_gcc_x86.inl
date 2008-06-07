namespace detail
{

/*
    from boost
*/

inline int atomic_exchange_and_add( int * pw, int dv )
{
    // int r = *pw;
    // *pw += dv;
    // return r;

    int r;

    __asm__ __volatile__
    (
        "lock\n\t"
        "xadd %1, %0":
        "=m"( *pw ), "=r"( r ): // outputs (%0, %1)
        "m"( *pw ), "1"( dv ): // inputs (%2, %3 == %1)
        "memory", "cc" // clobbers
    );

    return r;
}

inline void atomic_increment( int * pw )
{
    //atomic_exchange_and_add( pw, 1 );

    __asm__
    (
        "lock\n\t"
        "incl %0":
        "=m"( *pw ): // output (%0)
        "m"( *pw ): // input (%1)
        "cc" // clobbers
    );
}

inline int atomic_conditional_increment( int * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) ++*pw;
    // return rv;

    int rv, tmp;

    __asm__
    (
        "movl %0, %%eax\n\t"
        "0:\n\t"
        "test %%eax, %%eax\n\t"
        "je 1f\n\t"
        "movl %%eax, %2\n\t"
        "incl %2\n\t"
        "lock\n\t"
        "cmpxchgl %2, %0\n\t"
        "jne 0b\n\t"
        "1:":
        "=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
        "m"( *pw ): // input (%3)
        "cc" // clobbers
    );

    return rv;
}

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
      atomic_increment (&use_counter);      
    }

    void release ()
    {
      if (atomic_exchange_and_add (&use_counter, -1) == 1)
      {
        dispose();
        weak_release();
      }
    }

    bool add_ref_lock() // true on success
    {
      return atomic_conditional_increment (&use_counter) != 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Увеличение/уменьшение weak_count
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void weak_add_ref ()
    {
      atomic_increment (&weak_counter);      
    }

    void weak_release ()
    {
      if (atomic_exchange_and_add (&weak_counter, -1) == 1)
        destroy ();
    }

    long use_count () const
    {
      return static_cast<int const volatile &> (use_counter);  
    }

  private:
    int use_counter;
    int weak_counter;
};

}
