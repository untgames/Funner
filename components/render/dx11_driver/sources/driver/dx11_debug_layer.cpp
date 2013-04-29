#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Константы
*/

const size_t MESSAGE_BUFFER_RESERVE_SIZE = 16384;

/*
    Конструктор / деструктор
*/

DeviceDebugLayer::DeviceDebugLayer (const DxDevicePtr& in_device)
  : device (in_device)
  , processed_messages_count ()
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    ID3D11InfoQueue* dx_info_queue = 0;

    check_errors ("ID3D11Device::QueryInterface", device->QueryInterface (__uuidof (ID3D10InfoQueue), (void**)&dx_info_queue));

    if (!dx_info_queue)
      throw xtl::format_operation_exception ("", "ID3D11Device::QueryInterface failed");

    info_queue = DxInfoQueuePtr (dx_info_queue, false);

    message_buffer.resize (MESSAGE_BUFFER_RESERVE_SIZE);

    update_action = common::ActionQueue::PushAction (xtl::bind (&DeviceDebugLayer::UpdateMessages, this), common::ActionThread_Background);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::DeviceDebugLayer::DeviceDebugLayer");
    throw;
  }
}

DeviceDebugLayer::~DeviceDebugLayer ()
{
  StopAndJoin ();  
}

/*
    Остановка
*/

void DeviceDebugLayer::StopAndJoin ()
{
  try
  {
    common::Lock lock (*this);

    update_action.Cancel ();

    UpdateMessagesCore ();
  }
  catch (...)
  {
  }
}

/*
    Обновление сообщений
*/

void DeviceDebugLayer::UpdateMessages ()
{
  try
  {
    common::Lock lock (*this);

    UpdateMessagesCore ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11:DeviceDebugLayer::UpdateMessages");
    throw;
  }
}

namespace
{

const char* tostring (D3D11_MESSAGE_CATEGORY category)
{
  switch (category)
  {
    case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:     return "APPLICATION_DEFINED";
    case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:           return "MISCELLANEOUS";
    case D3D11_MESSAGE_CATEGORY_INITIALIZATION:          return "INITIALIZATION";
    case D3D11_MESSAGE_CATEGORY_CLEANUP:                 return "CLEANUP";
    case D3D11_MESSAGE_CATEGORY_COMPILATION:             return "COMPILATION";
    case D3D11_MESSAGE_CATEGORY_STATE_CREATION:          return "STATE_CREATION";
    case D3D11_MESSAGE_CATEGORY_STATE_SETTING:           return "STATE_SETTING";
    case D3D11_MESSAGE_CATEGORY_STATE_GETTING:           return "STATE_GETTING";
    case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:   return "RESOURCE_MANIPULATION";
    case D3D11_MESSAGE_CATEGORY_EXECUTION:               return "EXECUTION";
    default:                                             return "UNKOWN_CAREGORY";
  }
}

const char* tostring (D3D11_MESSAGE_SEVERITY severity)
{
  switch (severity)
  {
    case D3D11_MESSAGE_SEVERITY_CORRUPTION: return "CORRUPTION";
    case D3D11_MESSAGE_SEVERITY_ERROR:      return "ERROR";
    case D3D11_MESSAGE_SEVERITY_WARNING:    return "WARNING";
    case D3D11_MESSAGE_SEVERITY_INFO:       return "INFO";
    default:                                return "UNKOWN_SEVERITY";
  }
}

}

void DeviceDebugLayer::UpdateMessagesCore ()
{
  UINT64 total_count = info_queue->GetNumStoredMessages();

  if (total_count < processed_messages_count)
    return;

  SIZE_T message_length = 0;
  UINT64 message_index  = processed_messages_count;
  
  check_errors ("ID3D11InfoQueue::GetMessage", info_queue->GetMessage (message_index, 0, &message_length));

  processed_messages_count++;

  if (!message_length)
    return;

  message_buffer.resize (message_length + 1, false);
  
  char* end = message_buffer.data () + message_buffer.size () - 1;

  *end = 0;

  D3D11_MESSAGE* message = (D3D11_MESSAGE*)message_buffer.data ();

  check_errors ("ID3D11InfoQueue::GetMessage", info_queue->GetMessage (message_index, message, &message_length));

  log.Printf ("%s/%s: %s", tostring (message->Category), tostring (message->Severity), message->pDescription ? message->pDescription : "<null>");  
}
