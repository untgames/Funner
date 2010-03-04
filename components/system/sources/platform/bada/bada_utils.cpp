#include "shared.h"

namespace syslib
{

namespace bada
{

void raise (const char* source, result r)
{
  throw xtl::format_operation_exception (source, "%s (result=%04x)", GetErrorMessage (r), r);
}

}

}
