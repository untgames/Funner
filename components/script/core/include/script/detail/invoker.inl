namespace detail
{

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4180) //qualifier applied to a function will be ignored
#endif

template <class Fn> struct SimpleInvokerWrapper: public SimpleInvoker
{
  SimpleInvokerWrapper (const Fn& in_fn) : fn (in_fn) {}

  size_t operator () (IStack& stack)
  {
    return fn (stack);
  }
  
  Fn fn;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

}

template <class Fn>
Invoker::Invoker (const Fn& first_overload)
{
  Init (new detail::SimpleInvokerWrapper<Fn> (first_overload));
}
