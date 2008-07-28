#include "shared.h"

namespace input
{

namespace low_level
{

namespace direct_input_driver
{

const char* get_direct_input_error_name (HRESULT error)
{
  switch (error)
  {
    case DIERR_ACQUIRED:               return "DIERR_ACQUIRED";
    case DIERR_BETADIRECTINPUTVERSION: return "DIERR_BETADIRECTINPUTVERSION";
    case DIERR_DEVICENOTREG:           return "DIERR_DEVICENOTREG";
    case DIERR_INPUTLOST:              return "DIERR_INPUTLOST";
    case DIERR_INVALIDPARAM:           return "DIERR_INVALIDPARAM";
    case DIERR_NOINTERFACE:            return "DIERR_NOINTERFACE";
    case DIERR_NOTACQUIRED:            return "DIERR_NOTACQUIRED";
    case DIERR_NOTBUFFERED:            return "DIERR_NOTBUFFERED";
    case DIERR_NOTEXCLUSIVEACQUIRED:   return "DIERR_NOTEXCLUSIVEACQUIRED";
    case DIERR_NOTINITIALIZED:         return "DIERR_NOTINITIALIZED";
    case DIERR_OBJECTNOTFOUND:         return "DIERR_OBJECTNOTFOUND";
    case DIERR_OLDDIRECTINPUTVERSION:  return "DIERR_OLDDIRECTINPUTVERSION";
    case DIERR_OTHERAPPHASPRIO:        return "DIERR_OTHERAPPHASPRIO";
    case DIERR_OUTOFMEMORY:            return "DIERR_OUTOFMEMORY";
    case DIERR_UNSUPPORTED:            return "DIERR_UNSUPPORTED";
    case E_HANDLE:                     return "E_HANDLE";
    case E_PENDING:                    return "E_PENDING";
    default:                           return "Unknown";
  }
}

}

}

}
