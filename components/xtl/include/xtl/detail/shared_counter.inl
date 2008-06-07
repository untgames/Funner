namespace detail
{

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
