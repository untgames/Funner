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
const size_t BUFFER_SIZE = 16;

void default_event_handler (const char*)
{
}

/*const char* object_type_name (GUID type)
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
}*/

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

bool is_pov_pressed (DWORD pov_status)
{
  return LOWORD (pov_status) != 0xFFFF;
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

OtherDevice::OtherDevice (Window* window, const char* in_name, IDirectInputDevice8* in_direct_input_device_interface, const DebugLogHandler& in_debug_log_handler)
  : event_handler (&default_event_handler), name (in_name), device_interface (in_direct_input_device_interface, false), 
    poll_timer (xtl::bind (&OtherDevice::PollDevice, this), 10), device_lost (false), debug_log_handler (in_debug_log_handler)
{
  static const char* METHOD_NAME = "input::low_level::direct_input_driver::OtherDevice::OtherDevice";

  HRESULT operation_result;

  operation_result = device_interface->EnumObjects (&enum_object_callback, this, DIDFT_ALL);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't enumerate device objects, error '%s'", get_direct_input_error_name (operation_result));

  size_t device_data_size = 0;

  for (ObjectsMap::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter)
    if (iter->second.offset >= device_data_size)
      device_data_size = iter->second.offset + get_object_data_size (iter->second.type);

  if (device_data_size % 4)
    device_data_size += 4 - device_data_size % 4;

  xtl::uninitialized_storage<DIOBJECTDATAFORMAT> objects_data_format (objects.size ());

  size_t i = 0;
  for (ObjectsMap::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter, i++)
  {
    objects_data_format.data ()[i].pguid   = NULL;
    objects_data_format.data ()[i].dwOfs   = iter->second.offset;
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

  DIPROPDWORD buffer_size_property;

  buffer_size_property.diph.dwSize       = sizeof(DIPROPDWORD);
  buffer_size_property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  buffer_size_property.diph.dwObj        = 0;
  buffer_size_property.diph.dwHow        = DIPH_DEVICE;
  buffer_size_property.dwData            = BUFFER_SIZE;

  operation_result = device_interface->SetProperty (DIPROP_BUFFERSIZE, &buffer_size_property.diph);

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device buffer size, error '%s'", get_direct_input_error_name (operation_result));

  operation_result = device_interface->Acquire ();

  if (operation_result != DI_OK)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set device cooperative level, error '%s'", get_direct_input_error_name (operation_result));

  for (ObjectsMap::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter)
  {
    if (iter->second.type == ObjectType_AbsoluteAxis)
    {
      DIPROPRANGE range_property;

      range_property.diph.dwSize       = sizeof(DIPROPRANGE); 
      range_property.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
      range_property.diph.dwObj        = iter->second.offset;
      range_property.diph.dwHow        = DIPH_BYOFFSET; 

      operation_result = device_interface->GetProperty (DIPROP_RANGE, &range_property.diph);

      if (operation_result != DI_OK)
      {
        iter->second.bad_object = true;

        stl::string log_message = common::format ("Can't get object's %s range, not using this object.", iter->second.name.c_str ());

        debug_log_handler (log_message.c_str ());

        continue;
      }

      iter->second.min_value = range_property.lMin;
      iter->second.max_value = range_property.lMax;
        
      AddProperty (".dead_zone", iter, ObjectPropertyType_DeadZone, 0.f);
      AddProperty (".saturation", iter, ObjectPropertyType_Saturation, 1.f);
    }
    if (iter->second.type == ObjectType_RelativeAxis)
      AddProperty (".sensitivity", iter, ObjectPropertyType_Sensitivity, 1.f);
  }

  xtl::uninitialized_storage<char> initial_device_data (device_data_size);

  device_interface->Poll ();
  device_interface->GetDeviceState (initial_device_data.size (), initial_device_data.data ());

  for (ObjectsMap::iterator iter = objects.begin (), end = objects.end (); iter != end; ++iter)
  {
    switch (iter->second.type)
    {
      case ObjectType_AbsoluteAxis:
        iter->second.last_value = *(DWORD*)&(initial_device_data.data ()[iter->second.offset]);
        break;
      case ObjectType_RelativeAxis:
        iter->second.last_value = *(DWORD*)&(initial_device_data.data ()[iter->second.offset]);
        break;
      case ObjectType_Button:
        iter->second.last_value = initial_device_data.data ()[iter->second.offset];
        break;
      case ObjectType_POV:
        iter->second.last_value = *(DWORD*)&(initial_device_data.data ()[iter->second.offset]);
        break;
    }
  }
}

OtherDevice::~OtherDevice ()
{
  device_interface->Unacquire ();

  Release ();
}

/*
   Настройки устройства
*/

void OtherDevice::SetProperty (const char* name, float value)
{
  ObjectPropertyMap::iterator property_iter = objects_properties_map.find (name);

  if (property_iter == objects_properties_map.end ())
    throw xtl::make_argument_exception ("input::low_level::direct_input_driver::OtherDevice::SetProperty", "name", name);

  switch (property_iter->second.property_type)
  {
    case ObjectPropertyType_DeadZone:
    {
      if (value > 1.f) value = 1.f;
      if (value < 0.f) value = 0.f;

      SetDwordProperty (DIPROP_DEADZONE, (DWORD)(value * 10000), property_iter->second.object_offset);

      break;
    }
    case ObjectPropertyType_Saturation: 
    {
      if (value > 1.f) value = 1.f;
      if (value < 0.f) value = 0.f;

      SetDwordProperty (DIPROP_SATURATION, (DWORD)(value * 10000), property_iter->second.object_offset);

      break;
    }
  }

  property_iter->second.value = value;
}

void OtherDevice::SetDwordProperty (REFGUID property, DWORD value, DWORD object_offset)
{
  DIPROPDWORD property_value;

  property_value.diph.dwSize       = sizeof(DIPROPDWORD);
  property_value.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  property_value.diph.dwObj        = object_offset;
  property_value.diph.dwHow        = DIPH_BYOFFSET;
  property_value.dwData            = value;

  device_interface->SetProperty (property, &property_value.diph);
}

float OtherDevice::GetProperty (const char* name)
{
  ObjectPropertyMap::iterator property_iter = objects_properties_map.find (name);

  if (property_iter == objects_properties_map.end ())
    throw xtl::make_argument_exception ("input::low_level::direct_input_driver::OtherDevice::GetProperty", "name", name);

  return property_iter->second.value;
}

/*
   Регистрация объекта
*/

void OtherDevice::RegisterObject (const char* name, size_t offset, ObjectType type)
{
  objects.insert_pair (offset, ObjectData (name, offset, type));
}

/*
   Получение данных
*/

void OtherDevice::PollDevice ()
{
  HRESULT     operation_result;
  bool        device_reacquired = false;
  stl::string log_message;

  operation_result = device_interface->Poll ();

  if ((operation_result != DI_OK) && (operation_result != DI_NOEFFECT))
  {
    bool reacquire_result = ReAcquireDevice ();

    if (reacquire_result)
    {
      log_message = common::format ("Device %s reacquired", name.c_str ());

      debug_log_handler (log_message.c_str ());
      event_handler ("device reacquired");
      
      device_lost       = false;
      device_reacquired = true;
    }
    else if (operation_result == DIERR_INPUTLOST)
    {
      if (!device_lost)
      {
        log_message = common::format ("Device %s lost", name.c_str ());

        debug_log_handler (log_message.c_str ());
        event_handler ("device lost");
        
        device_lost = true;
      }

      return;
    }
    else if (!device_lost)
    {
      log_message = common::format ("Can't poll device %s, error '%s'", name.c_str (), get_direct_input_error_name (operation_result));

      debug_log_handler (log_message.c_str ());
    }
    else
      return;
  }

  DIDEVICEOBJECTDATA events[BUFFER_SIZE];
  DWORD              events_count = BUFFER_SIZE;

  operation_result = device_interface->GetDeviceData (sizeof (DIDEVICEOBJECTDATA), events, &events_count, 0);

  if (operation_result == DI_BUFFEROVERFLOW)
  {
    log_message = common::format ("Device %s buffer was overflowed", name.c_str ());

    debug_log_handler (log_message.c_str ());
  }
  else if (operation_result != DI_OK)
  {
    if (operation_result == DIERR_INPUTLOST)
    {
      if (device_reacquired)
      {
        log_message = common::format ("Bad device %s (loosing at get state)", name.c_str ());

        debug_log_handler (log_message.c_str ());
        event_handler ("device bad");
        
        poll_timer.Pause ();

        return;
      }

      bool reacquire_result = ReAcquireDevice ();
    
      if (reacquire_result)
      {
        operation_result = device_interface->GetDeviceData (sizeof (DIDEVICEOBJECTDATA), events, &events_count, 0);

        if (operation_result == DI_BUFFEROVERFLOW)
        {
          log_message = common::format ("Device %s buffer was overflowed", name.c_str ());

          debug_log_handler (log_message.c_str ());
        }
        else if (operation_result != DI_OK)
        { 
          log_message = common::format ("Can't get device %s state, error '%s'", name.c_str (), get_direct_input_error_name (operation_result));

          debug_log_handler (log_message.c_str ());
          return;
        }
      }
      else
      {
        log_message = common::format ("Device %s lost", name.c_str ());

        debug_log_handler (log_message.c_str ());
        event_handler ("device lost");
        device_lost = true;

        return;
      }      
    }
    else
    {
      log_message = common::format ("Can't get device %s state, error '%s'", name.c_str (), get_direct_input_error_name (operation_result));

      debug_log_handler (log_message.c_str ());
      return;
    }
  }

  static char message[MESSAGE_BUFFER_SIZE];

  for (size_t i = 0; i < events_count; i++)
  {
    ObjectsMap::iterator iter = objects.find (events[i].dwOfs);

    if (iter == objects.end ())
      continue;

    if (iter->second.bad_object)
      continue;

    switch (iter->second.type)
    {
      case ObjectType_AbsoluteAxis:
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' axis %.4f", iter->second.name.c_str (), (((float)(events[i].dwData - iter->second.min_value)) / (float)(iter->second.max_value - iter->second.min_value)) * 2.f - 1.f);
        event_handler (message);
        
        break;
      case ObjectType_RelativeAxis:
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' delta %f", iter->second.name.c_str (), ((float)(LONG)(events[i].dwData - iter->second.last_value)) * iter->second.properties[ObjectPropertyType_Sensitivity]->second.value);
        event_handler (message);
        
        break;
      case ObjectType_Button:
        xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' %s", iter->second.name.c_str (), (events[i].dwData & 0x80) ? "down" : "up");
        event_handler (message);
        
        break;
      case ObjectType_POV:
        if (!is_pov_pressed (events[i].dwData))
        {
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' up", iter->second.name.c_str ());
          event_handler (message);
        }
        else
        {
          xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' pov %d", iter->second.name.c_str (), events[i].dwData);
          event_handler (message);
          
          if (!is_pov_pressed (iter->second.last_value))
          {
            xsnprintf (message, MESSAGE_BUFFER_SIZE, "'%s' down", iter->second.name.c_str ());
            event_handler (message);
          }
        }
        break;
    }

    iter->second.last_value = events[i].dwData;
  }
}

/*
   Добавление свойства объекта
*/

void OtherDevice::AddProperty (const char* property_name, ObjectsMap::iterator object_iter, ObjectPropertyType property_type, float default_value)
{
  stl::string full_property_name = object_iter->second.name;

  full_property_name.append (property_name);

  ObjectPropertyMap::iterator property_iter = objects_properties_map.insert_pair (full_property_name.c_str (), ObjectPropertyMapElement (default_value, object_iter->second.offset, property_type)).first;

  object_iter->second.properties[property_type] = property_iter;

  if (!properties.empty ())
    properties += ' ';

  properties += '\'';
  properties.append (full_property_name);
  properties += '\'';
}

/*
   Попытка переполучения устройства
*/

bool OtherDevice::ReAcquireDevice ()
{
  HRESULT operation_result;

  device_interface->Unacquire ();
  operation_result = device_interface->Acquire ();

  if (operation_result == DI_OK)
    return true;

  return false;
}
