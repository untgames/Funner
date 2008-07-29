#include "shared.h"

namespace tools
{

namespace ui
{

namespace windows_forms
{

namespace
{

ref class EventHandlerDispatch
{
  public:
    EventHandlerDispatch (stl::auto_ptr<IEventHandlerProxy>& in_proxy) : proxy (in_proxy.release ()) { }

    ~EventHandlerDispatch ()
    {
      delete proxy;
    }

    void Process (Object^ sender, EventArgs^ args)
    {
      proxy->Process (sender, args);
    }

  private:
    IEventHandlerProxy* proxy;
};

}

System::EventHandler^ make_event_handler (stl::auto_ptr<IEventHandlerProxy>& proxy)
{
  if (!proxy.get ())
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::make_event_handler", "proxy");
    
  return gcnew EventHandler (gcnew EventHandlerDispatch (proxy), &EventHandlerDispatch::Process);
}

}

}

}
