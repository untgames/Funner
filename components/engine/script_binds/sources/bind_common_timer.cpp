#include "shared.h"

#include <common/time.h>

using namespace script;
using namespace common;

namespace
{

const char* COMMON_TIMER_LIBRARY = "Common.Timer";

}

Timer create_timer ()
{
  return Timer ();
}

Timer create_timer (const Timer& source)
{
  return Timer (source, true);
}

void set_started (Timer& timer, bool state)
{
  if (state) timer.Start ();
  else       timer.Stop ();
}

namespace engine
{

void bind_common_timer (script::Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_TIMER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (xtl::implicit_cast<Timer (*)(const Timer&)> (&create_timer)),
                                        make_invoker (xtl::implicit_cast<Timer (*)()> (&create_timer))
               ));

    //регистрация операций

  lib.Register ("set_Time",       make_invoker (&Timer::SetTime));
  lib.Register ("get_Time",       make_invoker (&Timer::Time));
  lib.Register ("Update",         make_invoker (&Timer::Update));
  lib.Register ("set_AutoUpdate", make_invoker (&Timer::SetAutoUpdate));
  lib.Register ("get_AutoUpdate", make_invoker (&Timer::IsAutoUpdate));
  lib.Register ("get_Started",    make_invoker (&Timer::IsStarted));
  lib.Register ("set_Started",    make_invoker (&set_started));
  lib.Register ("Start",          make_invoker (&Timer::Start));
  lib.Register ("Stop",           make_invoker (&Timer::Stop));
  lib.Register ("Reset",          make_invoker (&Timer::Reset));

    //регистрация типов данных

  environment.RegisterType<Timer> (COMMON_TIMER_LIBRARY);
}

}
