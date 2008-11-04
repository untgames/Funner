#include "shared.h"

namespace script
{

/*
    Объект, оповещающий об удалении IInterpreter
*/

xtl::trackable& get_trackable (IInterpreter& interpreter)
{
  return interpreter.GetTrackable ();
}

}
