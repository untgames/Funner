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

void default_event_handler (const char*)
{
}

const char* object_type_name (GUID type)
{
  if (type == GUID_XAxis)   return "GUID_XAxis";
  if (type == GUID_YAxis)   return "GUID_YAxis";
  if (type == GUID_ZAxis)   return "GUID_ZAxis";
  if (type == GUID_RxAxis)  return "GUID_RxAxis";
  if (type == GUID_RyAxis)  return "GUID_RyAxis";
  if (type == GUID_RzAxis)  return "GUID_RzAxis";
  if (type == GUID_Slider)  return "GUID_Slider";
  if (type == GUID_Button)  return "GUID_Button";
  if (type == GUID_Key)     return "GUID_Key";
  if (type == GUID_POV)     return "GUID_POV";

  return "Unkonwn";
}

void print_didft_type (DWORD type)
{
  if (type & DIDFT_ABSAXIS)         printf ("AbsAxis ");
  if (type & DIDFT_RELAXIS)         printf ("'Relative axis' ");
  if (type & DIDFT_AXIS)            printf ("Axis ");

  if (type & DIDFT_PSHBUTTON)       printf ("'Push button' ");
  if (type & DIDFT_TGLBUTTON)       printf ("'Toggle button' ");
  if (type & DIDFT_BUTTON)          printf ("Button ");
  
  if (type & DIDFT_POV)             printf ("POV ");

  if (type & DIDFT_COLLECTION)      printf ("Collection ");
  if (type & DIDFT_NODATA)          printf ("Nodata ");
  if (type & DIDFT_OUTPUT)          printf ("Output ");
  if (type & DIDFT_VENDORDEFINED)   printf ("Vendordefined ");
  if (type & DIDFT_ALIAS)           printf ("Alias ");
  if (type & DIDFT_OPTIONAL)        printf ("Optional ");

  if (type & DIDFT_FFACTUATOR)      printf ("FFActuator ");
  if (type & DIDFT_FFEFFECTTRIGGER) printf ("FFEffectTrigger ");
}

void print_flags (DWORD flags)
{
  if (flags & DIDOI_ASPECTACCEL)     printf ("reports acceleration; ");
  if (flags & DIDOI_ASPECTFORCE)     printf ("reports force; ");
  if (flags & DIDOI_ASPECTPOSITION)  printf ("reports position; ");
  if (flags & DIDOI_ASPECTVELOCITY)  printf ("reports velocity; ");
  if (flags & DIDOI_FFACTUATOR)      printf ("can have force-feedback effects; ");
  if (flags & DIDOI_FFEFFECTTRIGGER) printf ("can trigger playback of force-feedback effects; ");
  if (flags & DIDOI_POLLED)          printf ("require polling; ");
}

ObjectType get_object_type (DWORD type)
{
  if (type & DIDFT_BUTTON)          return ObjectType_Button;
  if (type & DIDFT_POV)             return ObjectType_POV;

  if (type & DIDFT_RELAXIS)         return ObjectType_RelativeAxis;
  if (type & DIDFT_ABSAXIS)         return ObjectType_AbsoluteAxis;

  if (type & DIDFT_AXIS)            return ObjectType_RelativeAxis;  //Ґб«Ё ­Ґв в®з­®© Ё­д®а¬ жЁЁ ® вЁЇҐ ®бЁ

  if (type & DIDFT_PSHBUTTON)       return ObjectType_Button;
  if (type & DIDFT_TGLBUTTON)       return ObjectType_Button;
  
  return ObjectType_Unknown;
}

size_t get_object_data_size (ObjectType type)
{
  switch (type)
  {
    case ObjectType_AbsoluteAxis:
    case ObjectType_RelativeAxis: return sizeof (LONG);
    case ObjectType_Button:       return sizeof (BYTE);
    case ObjectType_POV:          return sizeof (DWORD);
    default:                      return sizeof (size_t);
  }
}

BOOL FAR PASCAL enum_object_callback (LPCDIDEVICEOBJECTINSTANCEA object_instance, LPVOID device)
{
/*  printf ("Device object:\n");
  printf ("  name is '%s'\n", object_instance->tszName);
  printf ("  type is '%s'\n", object_type_name (object_instance->guidType));
  printf ("  offset is %d\n", object_instance->dwOfs);
  
  printf ("  dwType is: ");
  print_didft_type (object_instance->dwType & 0xff0000ff);
  printf ("\n");

  printf ("  flags is: ");
  print_flags (object_instance->dwFlags);
  printf ("\n");
  
  printf ("  dwFFMaxForce is %d\n", object_instance->dwFFMaxForce);
  printf ("  dwFFForceResolution is %d\n", object_instance->dwFFForceResolution);*/

  ObjectType object_type = get_object_type (object_instance->dwType & 0xff0000ff);

  if (object_type != ObjectType_Unknown)
    ((OtherDevice*)device)->RegisterObject (object_instance->tszName, object_instance->dwOfs, object_type);

  return DIENUM_CONTINUE;
}

}

/*
   Конструктор/деструктор
*/

OtherDevice::OtherDevice (Window* window, const char* in_name, IDirectInputDevice8* in_direct_input_device_interface)
  : event_handler (&default_event_handler), name (in_name), device_interface (in_direct_input_device_interface, false), 
    poll_timer (xtl::bind (&OtherDevice::PollDevice, this), 10)
{
  static const char* METHOD_NAME = "input::low_level::direct_input_driver::OtherDevice::OtherDevice";

  HRESULT operation_result;

  operation_result = device_interface->EnumObjects (&enum_object_callback, this, DIDFT_ALL);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't enumerate device objects, error '%s'", get_direct_input_error_name (operation_result));

  size_t device_data_size = 0;

  for (ObjectsArray::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter)
    if (iter->offset >= device_data_size)
      device_data_size = iter->offset + get_object_data_size (iter->type);

  if (device_data_size % 4)
    device_data_size += 4 - device_data_size % 4;

  last_device_data.resize (device_data_size);
  current_device_data.resize (device_data_size);

  xtl::uninitialized_storage<DIOBJECTDATAFORMAT> objects_data_format (objects.size ());

  for (size_t i = 0; i < objects.size (); i++)
  {
    objects_data_format.data ()[i].pguid   = NULL;
    objects_data_format.data ()[i].dwOfs   = objects[i].offset;
    objects_data_format.data ()[i].dwType  = DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV | DIDFT_PSHBUTTON | DIDFT_TGLBUTTON | DIDFT_ABSAXIS | DIDFT_RELAXIS | DIDFT_ANYINSTANCE;
    objects_data_format.data ()[i].dwFlags = 0;
  } 

  DIDATAFORMAT data_format = { sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_ABSAXIS, device_data_size, objects.size (), objects_data_format.data () }; 

  operation_result = device_interface->SetDataFormat (&data_format);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't setup device data format, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->SetCooperativeLevel ((HWND)window->Handle (), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->Acquire ();

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));

  device_interface->Poll ();
  device_interface->GetDeviceState (last_device_data.size (), last_device_data.data ());
}

OtherDevice::~OtherDevice ()
{
  device_interface->Unacquire ();

  Release ();
}

/*
   Регистрация объекта
*/

void OtherDevice::RegisterObject (const char* name, size_t offset, ObjectType type)
{
  objects.push_back (ObjectData (name, offset, type));
}

/*
   Получение данных
*/

void OtherDevice::PollDevice ()
{
  static const char* METHOD_NAME = "input::low_level::direct_input_driver::OtherDevice::PollDevice";

  HRESULT operation_result;

  operation_result = device_interface->Poll ();

  if ((operation_result != DI_OK) && (operation_result != DI_NOEFFECT))
  {
    operation_result = device_interface->Acquire ();

    if (operation_result != DI_OK)
      printf ("%s: Can't set device %s cooperative level, error '%s'\n", METHOD_NAME, name.c_str (), get_direct_input_error_name (operation_result));
//      throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));
  }


  operation_result = device_interface->GetDeviceState (current_device_data.size (), current_device_data.data ());

  if (operation_result != DI_OK)
    printf ("%s: Can't get device %s state, error '%s'\n", METHOD_NAME, name.c_str (), get_direct_input_error_name (operation_result));
//    throw xtl::format_operation_exception (METHOD_NAME, "Can't get device state, error '%s'", get_direct_input_error_name (operation_result));

  static char message[MESSAGE_BUFFER_SIZE];

  for (ObjectsArray::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter)
  {
    char* current_value = &(current_device_data.data ()[iter->offset]);

    if (memcmp (&(last_device_data.data ()[iter->offset]), current_value, get_object_data_size (iter->type)))
    {
      switch (iter->type)
      {
        case ObjectType_AbsoluteAxis:
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' axis %ld", iter->name.c_str (), *((LONG*)current_value));
          break;
        case ObjectType_RelativeAxis:
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' delta %ld", iter->name.c_str (), *((LONG*)current_value) - *((LONG*)(&last_device_data.data ()[iter->offset])));
          break;
        case ObjectType_Button:
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' %s", iter->name.c_str (), (*current_value & 0x80) ? "down" : "up");
          break;
        case ObjectType_POV:
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' pov %d", iter->name.c_str (), *((DWORD*)current_value));
          break;
      }

      event_handler (message);
    }
  }

  current_device_data.swap (last_device_data);
}
