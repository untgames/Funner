#include "shared.h"

using namespace plarium::system;
using namespace plarium::hsserver;

const char* get_error_code_name (ErrorCode code)
{
  switch (code)
  {
    case ErrorCode_Generic:             return "Generic";
    case ErrorCode_SendQueueOverflow:   return "SendQueueOverflow";
    case ErrorCode_Timeout:             return "Timeout";
    case ErrorCode_InvalidHeader:       return "InvalidHeader";
    case ErrorCode_HashMismatch:        return "HashMismatch";
    default:                            return "Unknown";
  }
}

const char* get_connection_state_name (HsConnectionState state)
{
  switch (state)
  {
    case HsConnectionState_Disconnected: return "Disconnected";
    case HsConnectionState_Connecting:   return "Connecting";
    case HsConnectionState_Connected:    return "Connected";
    default:                             return "Unknown";
  }
}

class HsConnectionEventListener : public IHsConnectionEventListener
{
  public:
    void OnError (HsConnection& sender, ErrorCode code, const char* error)
    {
      printf ("Error occured: code '%s' error '%s'\n", get_error_code_name (code), error);
    }

    void OnMessageReceived (HsConnection& sender, unsigned short plugin_id, const unsigned char* message, size_t size)
    {
      printf ("OnMessageReceived! Plugin %d, message '%s', size %d\n", plugin_id, message, size);
    }

    void OnStateChanged (HsConnection& sender, HsConnectionState new_state)
    {
      printf ("OnStateChange - new state '%s'!\n", get_connection_state_name (new_state));
    }
};

class HsConnectionLogListener : public IHsConnectionLogListener
{
  public:
    void OnLogMessage (HsConnection& sender, const char* message)
    {
      printf ("OnLogMessage - '%s'\n", message);
    }
};

int main ()
{
  printf ("Results of hs_connection_test:\n");

  try
  {
    HsConnectionSettings connection_settings;

    memset (&connection_settings, 0, sizeof (connection_settings));

    unsigned char key [16] = { 0xd0, 0x71, 0x73, 0x41, 0x0d, 0xee, 0xd2, 0x6f, 0x44, 0x3f, 0x7b, 0xa4, 0x42, 0x2f, 0x86, 0x03 };

    connection_settings.send_queue_size       = 2;
    connection_settings.keep_alive_interval   = 30000;
    connection_settings.compression_enabled   = true;
    connection_settings.compression_threshold = 1024;
    connection_settings.encryption_enabled    = true;
    connection_settings.encryption_key_bits   = sizeof (key) * 8;

    memcpy (connection_settings.encryption_key, key, sizeof (key));

    HsConnectionEventListener event_listener;
    HsConnectionLogListener   log_listener;
    HsConnection              connection (connection_settings);

    connection.SetEventListener (&event_listener);
    connection.SetLogListener (&log_listener);

    connection.Connect ("109.234.156.146", 9999);

    const char* message_text =  "[{"
      " \"event\":\"user.authenticate\","
      " \"lastName\":\"Anonym\","
      " \"firstName\":\"Anonym\","
      " \"token\":\"16880094c24e9a9754d026d903b06dbe\","
      " \"photo\":\"http://cs912.userapi.com/u93339557/a_57700b24.jpg\","
      " \"network\":\"VK\","
      " \"loginId\":\"50594503\","
      " \"version\":1"
      "}]";

    connection.SendMessage (0, (const unsigned char*)message_text, strlen (message_text));

    Thread::Sleep (2000);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
