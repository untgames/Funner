#if 0 
#include "shared.h"

using namespace xtl;
using namespace common;
using namespace input::low_level::direct_input_driver;
using namespace syslib;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

const size_t MESSAGE_BUFFER_SIZE  = 32;

const size_t SLIDERS_COUNT = 2;
const size_t POVS_COUNT    = 4;
const size_t BUTTONS_COUNT = 32;

const char*  X_AXIS_NAME          = "X axis";
const char*  Y_AXIS_NAME          = "Y axis";
const char*  Z_AXIS_NAME          = "Z axis";
const char*  X_AXIS_ROTATION_NAME = "X_Rotation axis";
const char*  Y_AXIS_ROTATION_NAME = "Y_Rotation axis";
const char*  Z_AXIS_ROTATION_NAME = "Z_Rotation axis";
const char*  SLIDER_NAME          = "Slider";
const char*  POV_NAME             = "POV";
const char*  BUTTON_NAME          = "Button";

void default_event_handler (const char*)
{
}

}

/*
   Конструктор/деструктор
*/

GameControlDevice::GameControlDevice (Window* window, const char* in_name, IDirectInputDevice8* in_direct_input_device_interface)
  : event_handler (&default_event_handler), name (in_name), device_interface (in_direct_input_device_interface, false), 
    poll_timer (xtl::bind (&GameControlDevice::PollDevice, this), 10)
{
  static const char* METHOD_NAME = "input::low_level::direct_input_driver::GameControlDevice::GameControlDevice";

  HRESULT operation_result;

  operation_result = device_interface->SetDataFormat (&c_dfDIJoystick);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't setup device data format, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->SetCooperativeLevel ((HWND)window->Handle (), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->Acquire ();

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));

/*   DIPROPRANGE diprg;
   diprg.diph.dwSize = sizeof( DIPROPRANGE );
   diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
   diprg.diph.dwHow = DIPH_BYID;
   diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
   diprg.lMin = -1000;
   diprg.lMax = +1000;

   // Set the range for the axis
   if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
       return DIENUM_STOP;

  device_interface->SetProperty (DIPROP_AXISMODE, );*/

  device_interface->Poll ();
  device_interface->GetDeviceState (sizeof(current_joy_state), &current_joy_state);
}

GameControlDevice::~GameControlDevice ()
{
  device_interface->Unacquire ();

  Release ();
}

/*
   Получение данных
*/

void GameControlDevice::PollDevice ()
{
  static const char* METHOD_NAME = "input::low_level::direct_input_driver::GameControlDevice::PollDevice";

  HRESULT    operation_result;
  DIJOYSTATE joy_state;        //Direct Input joystick state 

  operation_result = device_interface->Poll ();

/*  if (operation_result == DI_NOEFFECT)
  {
    DIDEVICEOBJECTDATA device_object_data[DEVICE_OBJECTS_COUNT];

    operation_result = device_interface->GetDeviceData (sizeof (DIDEVICEOBJECTDATA), device_object_data, (LPDWORD)&DEVICE_OBJECTS_COUNT, 0);

    if (operation_result != DI_OK) //DI_BUFFEROVERFLOW???
      printf ("%s: Can't get device buffered data, error %s\n", METHOD_NAME, get_direct_input_error_name (operation_result));

    return;
  }*/

  if ((operation_result != DI_OK) && (operation_result != DI_NOEFFECT))
    printf ("%s: Can't poll device data, error '%s'\n", METHOD_NAME, get_direct_input_error_name (operation_result));
//    throw xtl::format_operation_exception (METHOD_NAME, "Can't poll device data, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->GetDeviceState (sizeof(joy_state), &joy_state);

  if (operation_result != DI_OK)
    printf ("%s: Can't get device state, error '%s'\n", METHOD_NAME, get_direct_input_error_name (operation_result));
//    throw xtl::format_operation_exception (METHOD_NAME, "Can't get device state, error '%s'", get_direct_input_error_name (operation_result));

  static char message[MESSAGE_BUFFER_SIZE];

  if (current_joy_state.lX != joy_state.lX)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", X_AXIS_NAME, joy_state.lX);
    event_handler (message);
  }
  if (current_joy_state.lY != joy_state.lY)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", Y_AXIS_NAME, joy_state.lY);
    event_handler (message);
  }
  if (current_joy_state.lZ != joy_state.lZ)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", Z_AXIS_NAME, joy_state.lZ);
    event_handler (message);
  }
  if (current_joy_state.lRx != joy_state.lRx)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", X_AXIS_ROTATION_NAME, joy_state.lRx);
    event_handler (message);
  }
  if (current_joy_state.lRy != joy_state.lRy)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", Y_AXIS_ROTATION_NAME, joy_state.lRy);
    event_handler (message);
  }
  if (current_joy_state.lRz != joy_state.lRz)
  {
    xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s %ld", Z_AXIS_ROTATION_NAME, joy_state.lRz);
    event_handler (message);
  }

  for (size_t i = 0; i < SLIDERS_COUNT; i++)
    if (current_joy_state.rglSlider[i] != joy_state.rglSlider[i])
    {
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s%u axis %ld", SLIDER_NAME, i, joy_state.rglSlider[i]);
      event_handler (message);
    }

  for (size_t i = 0; i < POVS_COUNT; i++)
    if (current_joy_state.rgdwPOV[i] != joy_state.rgdwPOV[i])
    {
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s%u pov %d", POV_NAME, i, joy_state.rgdwPOV[i]);
      event_handler (message);
    }

  for (size_t i = 0; i < BUTTONS_COUNT; i++)
    if (current_joy_state.rgbButtons[i] != joy_state.rgbButtons[i])
    {
      xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s %u' %s", BUTTON_NAME, i, (joy_state.rgbButtons[i] & 0x80) ? "down" : "up");
      event_handler (message);
    }

  current_joy_state = joy_state;
}

#endif
