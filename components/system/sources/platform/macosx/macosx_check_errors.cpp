#include "shared.h"

namespace
{

const char* get_event_manager_error_name (OSStatus error)
{
  switch (error)
  {
    case noErr:                           return "noErr";
    case eventAlreadyPostedErr:           return "eventAlreadyPostedErr";
    case eventTargetBusyErr:              return "eventTargetBusyErr";
    case eventClassInvalidErr:            return "eventClassInvalidErr";
    case eventClassIncorrectErr:          return "eventClassIncorrectErr";
    case eventHandlerAlreadyInstalledErr: return "eventHandlerAlreadyInstalledErr";
    case eventInternalErr:                return "eventInternalErr";
    case eventKindIncorrectErr:           return "eventKindIncorrectErr";
    case eventParameterNotFoundErr:       return "eventParameterNotFoundErr";
    case eventNotHandledErr:              return "eventNotHandledErr";
    case eventLoopTimedOutErr:            return "eventLoopTimedOutErr";
    case eventLoopQuitErr:                return "eventLoopQuitErr";
    case eventNotInQueueErr:              return "eventNotInQueueErr";
    case eventHotKeyExistsErr:            return "eventHotKeyExistsErr";
    case eventHotKeyInvalidErr:           return "eventHotKeyInvalidErr";
    default:                              return "Unknown error";
  }
}

const char* get_window_manager_error_name (OSStatus error)
{
  switch (error)
  {
    case noErr:                                  return "noErr";
    case errInvalidWindowRef:                    return "errInvalidWindowRef";
    case errUnsupportedWindowAttributesForClass: return "errUnsupportedWindowAttributesForClass";
    case errWindowDoesNotHaveProxy:              return "errWindowDoesNotHaveProxy";
    case errInvalidWindowProperty:               return "errInvalidWindowProperty";
    case errWindowPropertyNotFound:              return "errWindowPropertyNotFound";
    case errUnrecognizedWindowClass:             return "errUnrecognizedWindowClass";
    case errCorruptWindowDescription:            return "errCorruptWindowDescription";
    case errUserWantsToDragWindow:               return "errUserWantsToDragWindow";
    case errWindowsAlreadyInitialized:           return "errWindowsAlreadyInitialized";
    case errFloatingWindowsNotInitialized:       return "errFloatingWindowsNotInitialized";
    case errWindowNotFound:                      return "errWindowNotFound";
    case errWindowDoesNotFitOnscreen:            return "errWindowDoesNotFitOnscreen";
    case windowAttributeImmutableErr:            return "windowAttributeImmutableErr";
    case windowAttributesConflictErr:            return "windowAttributesConflictErr";
    case windowManagerInternalErr:               return "windowManagerInternalErr";
    case windowWrongStateErr:                    return "windowWrongStateErr";
    case windowGroupInvalidErr:                  return "windowGroupInvalidErr";
    case windowAppModalStateAlreadyExistsErr:    return "windowAppModalStateAlreadyExistsErr";
    case windowNoAppModalStateErr:               return "windowNoAppModalStateErr";
    case errWindowDoesntSupportFocus:            return "errWindowDoesntSupportFocus";
    case errWindowRegionCodeInvalid:             return "errWindowRegionCodeInvalid";
    case errNeedsCompositedWindow:               return "errNeedsCompositedWindow";
    default:                                     return "Unknown error";
  }
}

const char* get_quartz_error_name (CGError error)
{
  static const int kCGErrorNameTooLong    = 1005; //gone after 10.8 SDK
  static const int kCGErrorNoCurrentPoint = 1009; //gone after 10.8 SDK

  switch (error)
  {
    case kCGErrorSuccess:           return "kCGErrorSuccess";
    case kCGErrorFailure:           return "kCGErrorFailure";
    case kCGErrorIllegalArgument:   return "kCGErrorIllegalArgument";
    case kCGErrorInvalidConnection: return "kCGErrorInvalidConnection";
    case kCGErrorInvalidContext:    return "kCGErrorInvalidContext";
    case kCGErrorCannotComplete:    return "kCGErrorCannotComplete";
    case kCGErrorNameTooLong:       return "kCGErrorNameTooLong";
    case kCGErrorNotImplemented:    return "kCGErrorNotImplemented";
    case kCGErrorRangeCheck:        return "kCGErrorRangeCheck";
    case kCGErrorTypeCheck:         return "kCGErrorTypeCheck";
    case kCGErrorNoCurrentPoint:    return "kCGErrorNoCurrentPoint";
    case kCGErrorInvalidOperation:  return "kCGErrorInvalidOperation";
    case kCGErrorNoneAvailable:     return "kCGErrorNoneAvailable";
    default:                        return "Unknown error";
  }
}

}

namespace syslib
{

namespace macosx
{

void check_event_manager_error (OSStatus error_code, const char* source, const char* message)
{
  if (error_code != noErr)
    throw xtl::format_operation_exception (source, "%s. Carbon error: %s (code %d)", message,
                                           get_event_manager_error_name (error_code), error_code);
}

void check_window_manager_error (OSStatus error_code, const char* source, const char* message)
{
  if (error_code != noErr)
    throw xtl::format_operation_exception (source, "%s. Carbon error: %s (code %d)", message,
                                           get_window_manager_error_name (error_code), error_code);
}

void check_quartz_error (CGError error_code, const char* source, const char* message)
{
  if (error_code != kCGErrorSuccess)
    throw xtl::format_operation_exception (source, "%s. Quartz error: %s (code %d)", message,
                                           get_quartz_error_name (error_code), error_code);
}

}

}
