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
      use_counter++;
    }

    void release ()
    {
      if (!--use_counter)
      {
        dispose();
        weak_release();
      }
    }

    bool add_ref_lock() // true on success
    {
      bool result = use_counter != 0;

      if (result)
        ++use_counter;

      return result;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Увеличение/уменьшение weak_count
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void weak_add_ref ()
    {
      weak_counter++;
    }

    void weak_release ()
    {
      if (!--weak_counter)
        destroy ();
    }

    long use_count () const
    {
      return use_counter;
    }

  private:
    long use_counter;
    long weak_counter;
};

}
