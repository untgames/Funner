#include "shared.h"

using namespace syslib;

struct Platform::semaphore_handle
{
  MPSemaphoreID semaphore;
};


namespace
{

const char* get_multiprocessing_services_error_name (OSStatus error)
{
  switch (error)
  {
    case kMPIterationEndErr:          return "kMPIterationEndErr";
    case kMPPrivilegedErr:            return "kMPPrivilegedErr";
    case kMPProcessCreatedErr:        return "kMPProcessCreatedErr";
    case kMPProcessTerminatedErr:     return "kMPProcessTerminatedErr";
    case kMPTaskCreatedErr:           return "kMPTaskCreatedErr";
    case kMPTaskBlockedErr:           return "The desired task is blocked.";
    case kMPTaskStoppedErr:           return "The desired task is stopped.";
    case kMPDeletedErr:               return "The desired notification the function was waiting upon was deleted.";
    case kMPTimeoutErr:               return "The designated timeout interval passed before the function could take action.";
    case kMPInsufficientResourcesErr: return "Could not complete task due to unavailable Multiprocessing Services resources.";
    case kMPInvalidIDErr:             return "Invalid ID value.";
    default:                          return "Unknown error";
  }
}

void check_multiprocessing_services_error (OSStatus error_code, const char* source, const char* message)
{
  if (error_code != noErr)
    throw xtl::format_operation_exception (source, "%s. Carbon error: %s (code %d)", message,
                                           get_multiprocessing_services_error_name (error_code), error_code);
}

void wait_semaphore (Platform::semaphore_t handle, Duration duration)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("", "handle");

  OSStatus status = MPWaitOnSemaphore (handle->semaphore, duration);

  if (status != kMPDeletedErr && status != kMPTimeoutErr)
    check_multiprocessing_services_error (status, "::MPWaitOnSemaphore", "Wait semaphore failed");
}

}

//создание семафора
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  try
  {
    stl::auto_ptr<semaphore_handle> handle (new semaphore_handle);

    check_multiprocessing_services_error (MPCreateSemaphore (ULONG_MAX, initial_value, &handle->semaphore),
                                          "::MPCreateSemaphore", "Can't create semaphore");

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonPlatform::CreateSemaphore");
    throw;
  }
}

//уничтожение семафора
void Platform::DestroySemaphore (semaphore_t handle)
{
  if (!handle || !handle->semaphore)
    return;

  MPDeleteSemaphore (handle->semaphore);

  delete handle;
}

//ожидание следующей задачи
void Platform::WaitSemaphore (semaphore_t handle)
{
  try
  {
    wait_semaphore (handle, kDurationForever);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonPlatform::WaitSemaphore (semaphore_t)");
    throw;
  }
}

//ожидание следующей задачи с таймаутом
void Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  try
  {
    wait_semaphore (handle, kDurationMillisecond * wait_in_milliseconds);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonPlatform::WaitSemaphore (semaphore_t, size_t)");
    throw;
  }
}

//попытка ожидания следующей задачи
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    OSStatus status = MPWaitOnSemaphore (handle->semaphore, kDurationImmediate);

    switch (status)
    {
      case noErr:         return true;
      case kMPTimeoutErr: return false;
      default:
        check_multiprocessing_services_error (status, "::MPWaitOnSemaphore", "Wait semaphore failed");
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonPlatform::TryLockSemaphore");
    throw;
  }
}

//посылка следующей задачи
void Platform::PostSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    check_multiprocessing_services_error (MPSignalSemaphore (handle->semaphore), "::MPSignalSemaphore", "Can't post semaphore");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonPlatform::PostSemaphore");
    throw;
  }
}
