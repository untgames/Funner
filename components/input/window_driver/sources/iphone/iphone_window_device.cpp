#include <syslib/platform/iphone.h>

#include <core/shared.h>

using namespace syslib::iphone;
using namespace input::low_level::window;

namespace
{

const size_t MESSAGE_BUFFER_SIZE = 64;

}

struct IPhoneDevice::Impl : public IWindowListener
{
  IPhoneDevice*        device;                        //устройство
  xtl::auto_connection window_destroy_connection;     //соединение удаления окна
  syslib::Window       *window;                       //окно
  char                 message [MESSAGE_BUFFER_SIZE]; //сообщение

  ///Конструктор / деструктор
  Impl (IPhoneDevice* in_device, syslib::Window* in_window)
    : device (in_device)
    , window (in_window)
  {
    try
    {
      window_destroy_connection = window->RegisterEventHandler (syslib::WindowEvent_OnDestroy, xtl::bind (&IPhoneDevice::Impl::OnWindowDestroy, this));
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("input::low_level::window::IPhoneDevice::IPhoneDevice");
      throw;
    }

    WindowManager::AttachWindowListener (*window, this);
  }

  ~Impl ()
  {
    WindowManager::DetachWindowListener (*window, this);
  }

  ///События разворота
  void OnInterfaceOrientationWillChange (InterfaceOrientation old_orientation, InterfaceOrientation new_orientation, float duration)
  {
    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "OrientationChange began %s %s %f", OrientationName (old_orientation),
                    OrientationName (new_orientation), duration);
    device->Notify (message);
  }

  void OnInterfaceOrientationChanged (InterfaceOrientation old_orientation, InterfaceOrientation new_orientation)
  {
    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "OrientationChange ended %s %s",
                    OrientationName (old_orientation), OrientationName (new_orientation));
    device->Notify (message);
  }

  ///События движения
  void OnShakeMotionBegan ()
  {
    device->Notify ("ShakeMotion began");
  }

  void OnShakeMotionEnded ()
  {
    device->Notify ("ShakeMotion ended");
  }

  void OnWindowDestroy ()
  {
    WindowManager::DetachWindowListener (*window, this);
  }

  const char* OrientationName (InterfaceOrientation orientation)
  {
    switch (orientation)
    {
      case InterfaceOrientation_Portrait:           return "Portrait";
      case InterfaceOrientation_PortraitUpsideDown: return "PortraitUpsideDown";
      case InterfaceOrientation_LandscapeLeft:      return "LandscapeLeft";
      case InterfaceOrientation_LandscapeRight:     return "LandscapeRight";
      default:                                      return "Unknown";
    }
  }
};

/*
   Конструктор/деструктор
*/

IPhoneDevice::IPhoneDevice (syslib::Window* window, const char* name, const char* full_name)
  : DefaultDevice (window, name, full_name), impl (new Impl (this, window))
{
}

IPhoneDevice::~IPhoneDevice ()
{
  delete impl;
}
