///////////////////////////////////////////////////////////////////////////////////////////////////
///Посредник, участвующий в диспетчеризации события
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEventHandlerProxy
{
  public:
    virtual ~IEventHandlerProxy () {}
    
    virtual void Process (Object^, EventArgs^) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание делегата к обработчику события
///////////////////////////////////////////////////////////////////////////////////////////////////
System::EventHandler^ make_event_handler (stl::auto_ptr<IEventHandlerProxy>& proxy);

template <class Ptr, class MemFn> System::EventHandler^ make_event_handler (const Ptr& ptr, const MemFn& fn)
{
  struct EventHandlerProxy: public IEventHandlerProxy
  {
    Ptr   ptr;
    MemFn fn;
    
    EventHandlerProxy (const Ptr& in_ptr, const MemFn& in_fn) : ptr (in_ptr), fn (in_fn) {}
       
    void Process (Object^ sender, EventArgs^ args) { ((*ptr).*fn)(sender, args); }
  };
  
  stl::auto_ptr<IEventHandlerProxy> proxy (new EventHandlerProxy (ptr, fn));
  
  return make_event_handler (proxy);
}
