namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class shared_counter
{
  public:
            shared_counter  () : use_counter (1), weak_counter (1) { }
    virtual ~shared_counter () { }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� use_count ���������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void dispose () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� weak_count ���������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void destroy () { delete this; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �� ��������� ������ ���� ��� ��� ��������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    virtual void* get_deleter (const std::type_info&) = 0;
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/���������� ����� ������
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
///����������/���������� weak_count
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

}
