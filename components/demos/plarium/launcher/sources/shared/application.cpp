#include "shared.h"

using namespace engine;
using namespace plarium::launcher;
using namespace plarium::hsserver;
using namespace plarium::utility;

namespace
{

//constants
const char*         APP_ID                            = "2972852";
const char*         APP_SECRET                        = "f2h3o02f43h34qv9h3124vb3721432jpo32ADG";
const char*         DEBUG_MODE_NOTIFICATION_PREFIX    = "DebugMode#";
const char*         OPEN_URL_NOTIFICATION_PREFIX      = "ApplicationOpenURL ";
const char*         PLARIUM_TOKEN_NOTIFICATION_PREFIX = "GetPlariumToken#";
const size_t        SEND_QUEUE_SIZE                   = 16;
const size_t        KEEP_ALIVE_INTERVAL               = 30000;
const size_t        COMPRESSION_THRESHOLD             = 1024;
const unsigned char ENCRYPTION_KEY []                 = { 0xd0, 0x71, 0x73, 0x41, 0x0d, 0xee, 0xd2, 0x6f, 0x44, 0x3f, 0x7b, 0xa4, 0x42, 0x2f, 0x86, 0x03 };
const char*         QUOTE_REPLACEMENT                 = "\\u0027";
const size_t        QUOTE_REPLACEMENT_LENGTH          = strlen (QUOTE_REPLACEMENT);

}

/*
   Application launcher
*/

struct Application::Impl : public INotificationListener, public IHsConnectionEventListener, public IHsConnectionLogListener
{
  sgi_stl::auto_ptr<HsConnection> connection;
  engine::IEngine*                engine;
  sgi_stl::string                 host;
  unsigned short                  port;
  unsigned char*                  message_buffer;
  size_t                          message_buffer_size;
  bool                            debug_mode;
  IOpenUrlHandler*                open_url_handler;

  Impl ()
    : engine (0)
    , port (0)
    , message_buffer (0)
    , message_buffer_size (0)
    , debug_mode (false)
  {
    HsConnectionSettings connection_settings;

    memset (&connection_settings, 0, sizeof (connection_settings));

    connection_settings.send_queue_size       = SEND_QUEUE_SIZE;
    connection_settings.keep_alive_interval   = KEEP_ALIVE_INTERVAL;
    connection_settings.compression_enabled   = true;
    connection_settings.compression_threshold = COMPRESSION_THRESHOLD;
    connection_settings.encryption_enabled    = true;
    connection_settings.encryption_key_bits   = sizeof (ENCRYPTION_KEY) * 8;

    memcpy (&connection_settings.encryption_key, ENCRYPTION_KEY, sizeof (ENCRYPTION_KEY));

    connection.reset (new HsConnection (connection_settings));

    connection->SetEventListener (this);
    connection->SetLogListener (this);
  }

  ~Impl ()
  {
    delete [] message_buffer;
  }

  //Set open URL handler
  void SetOpenUrlHandler (IOpenUrlHandler* handler)
  {
   open_url_handler = handler;
  }

  void Run (engine::IEngine* in_engine)
  {
    if (engine)
      throw sgi_stl::logic_error ("plarium::launcher::Application::Run - already run");

    if (!in_engine)
      throw sgi_stl::invalid_argument ("plarium::launcher::Application::Run - null engine");

    engine = in_engine;

    engine->AttachNotificationListener ("HsConnection *", this);
    engine->AttachNotificationListener (format ("%s*", DEBUG_MODE_NOTIFICATION_PREFIX).c_str (), this);
    engine->AttachNotificationListener (format ("%s*", PLARIUM_TOKEN_NOTIFICATION_PREFIX).c_str (), this);
    engine->AttachNotificationListener (format ("%s*", OPEN_URL_NOTIFICATION_PREFIX).c_str (), this);

    engine->Run ();
  }

  void OnNotification (const char* notification)
  {
    static const char* METHOD_NAME = "plarium::launcher::Application::OnNotification";

    if (strstr (notification, PLARIUM_TOKEN_NOTIFICATION_PREFIX) == notification)
    {
      sgi_stl::string token_component_source = format ("%s%s", APP_ID, APP_SECRET);

      unsigned char token_component [16];

      md5 (token_component_source.c_str (), token_component_source.size (), token_component);

      char token_component_string [33];

      for (size_t i = 0; i < sizeof (token_component); i++)
        sprintf (token_component_string + i * 2, "%02x", token_component [i]);

      token_component_string [32] = 0;

      sgi_stl::string token_source = format ("%s_%s_%s", token_component_string, notification + strlen (PLARIUM_TOKEN_NOTIFICATION_PREFIX), APP_SECRET);

      unsigned char token [16];

      md5 (token_source.c_str (), token_source.size (), token);

      char token_string [33];

      for (size_t i = 0; i < sizeof (token); i++)
        sprintf (token_string + i * 2, "%02x", token [i]);

      token_string [32] = 0;

      engine->Execute (format ("lua: OnPlariumTokenObtained ('%s')", token_string).c_str ());
    }
    else if (strstr (notification, DEBUG_MODE_NOTIFICATION_PREFIX) == notification)
    {
      debug_mode = atoi (notification + strlen (DEBUG_MODE_NOTIFICATION_PREFIX)) != 0;
    }
    else if (strstr (notification, OPEN_URL_NOTIFICATION_PREFIX) == notification)
    {
      if (open_url_handler)
        open_url_handler->HandleUrlOpen (notification + strlen (OPEN_URL_NOTIFICATION_PREFIX));
    }
    else
    {
      sgi_stl::vector<sgi_stl::string> words   = split (notification, " ", "'\"");
      const sgi_stl::string&       command = words [1];

      if (command == "Connect")
      {
        if (words.size () != 4)
          throw sgi_stl::invalid_argument (format ("%s: invalid 'Connect' arguments '%s'", METHOD_NAME, notification));

        host = words [2];
        port = atoi (words [3].c_str ());

        connection->Connect (host.c_str (), port);
      }
      else if (command == "Disconnect")
      {
        connection->Disconnect ();
      }
      else if (command == "Reconnect")
      {
        connection->Disconnect ();
        connection->Connect (host.c_str (), port);
      }
      else if (command == "SendMessage")
      {
        if (words.size () != 5)
          throw sgi_stl::invalid_argument (format ("%s: invalid 'SendMessage' arguments '%s'", METHOD_NAME, notification));

        connection->SendMessage (atoi (words [2].c_str ()), atoi (words [3].c_str ()), (const unsigned char*)words [4].c_str (), words [4].size ());
      }
    }
  }

  void OnLogMessage (HsConnection& sender, const char* message)
  {
    if (debug_mode)
      printf ("HsConnection: '%s'\n", message);
  }

  void OnError (HsConnection& sender, ErrorCode code, const char* error)
  {
    engine->Execute (format ("lua: OnHsConnectionError (%d, \"%s\")", code, error).c_str ());
  }

  void OnMessageReceived (HsConnection& sender, unsigned char sender_plugin_id, unsigned char receiver_plugin_id, const unsigned char* message, size_t size)
  {
    bool quote_found = false;

    for (const unsigned char* m = message; *m; m++)
    {
      if (*m == '\'')
      {
        quote_found = true;
        break;
      }
    }

    if (quote_found)
    {
      size_t escaped_message_size = size * 5;

      if (message_buffer_size < escaped_message_size)
      {
        try
        {
          delete [] message_buffer;

          message_buffer = new unsigned char [escaped_message_size];

          message_buffer_size = escaped_message_size;
        }
        catch (...)
        {
          message_buffer      = 0;
          message_buffer_size = 0;
          throw;
        }
      }

      unsigned char* replaced_char = message_buffer;

      for (const unsigned char* m = message; *m; m++, replaced_char++)
      {
        if (*m == '\'')
        {
          memcpy (replaced_char, QUOTE_REPLACEMENT, QUOTE_REPLACEMENT_LENGTH);

          replaced_char += QUOTE_REPLACEMENT_LENGTH - 1;
        }
        else
        {
          *replaced_char = *m;
        }
      }

      *replaced_char = 0;

      message = message_buffer;
    }

    engine->Execute (format ("lua: OnHsConnectionMessage (%d, %d, '%s')", sender_plugin_id, receiver_plugin_id, message).c_str ());
  }

  void OnStateChanged (HsConnection& sender, HsConnectionState new_state)
  {
    engine->Execute (format ("lua: OnHsConnectionStateChanged (%d)", new_state).c_str ());
  }
};

/*
   Constructor / destructor
*/

Application::Application ()
  : impl (new Impl)
  {}

Application::~Application ()
{
  delete impl;
}

/*
   Main loop start
*/

void Application::Run (engine::IEngine* engine)
{
  impl->Run (engine);
}

/*
   Set open URL handler
*/

void Application::SetOpenUrlHandler (IOpenUrlHandler* handler)
{
  impl->SetOpenUrlHandler (handler);
}
