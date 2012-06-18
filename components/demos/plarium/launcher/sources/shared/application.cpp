#include "shared.h"

using namespace engine;
using namespace plarium::launcher;
using namespace plarium::hsserver;
using namespace plarium::utility;

namespace
{

//constants
const size_t        SEND_QUEUE_SIZE          = 16;
const size_t        KEEP_ALIVE_INTERVAL      = 30000;
const size_t        COMPRESSION_THRESHOLD    = 1024;
const unsigned char ENCRYPTION_KEY []        = { 0xd0, 0x71, 0x73, 0x41, 0x0d, 0xee, 0xd2, 0x6f, 0x44, 0x3f, 0x7b, 0xa4, 0x42, 0x2f, 0x86, 0x03 };
const char*         QUOTE_REPLACEMENT        = "\\u0027";
const size_t        QUOTE_REPLACEMENT_LENGTH = strlen (QUOTE_REPLACEMENT);

}

/*
   Application launcher
*/

struct Application::Impl : public INotificationListener, public IHsConnectionEventListener, public IHsConnectionLogListener
{
  std::auto_ptr<HsConnection> connection;
  engine::IEngine*            engine;
  std::string                 host;
  unsigned short              port;
  unsigned char*              message_buffer;
  size_t                      message_buffer_size;

  Impl ()
    : engine (0)
    , port (0)
    , message_buffer (0)
    , message_buffer_size (0)
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

  void Run (engine::IEngine* in_engine)
  {
    if (engine)
      throw std::logic_error ("plarium::launcher::Application::Run - already run");

    if (!in_engine)
      throw std::invalid_argument ("plarium::launcher::Application::Run - null engine");

    engine = in_engine;

    engine->AttachNotificationListener ("HsConnection *", this);

    engine->Run ();
  }

  void OnNotification (const char* notification)
  {
    static const char* METHOD_NAME = "plarium::launcher::Application::OnNotification";

    std::vector<std::string> words   = split (notification, " ", "'\"");
    const std::string&       command = words [1];

    if (command == "Connect")
    {
      if (words.size () != 4)
        throw std::invalid_argument (format ("%s: invalid 'Connect' arguments '%s'", METHOD_NAME, notification));

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
      if (words.size () != 4)
        throw std::invalid_argument (format ("%s: invalid 'SendMessage' arguments '%s'", METHOD_NAME, notification));

      connection->SendMessage (atoi (words [2].c_str ()), (const unsigned char*)words [3].c_str (), words [3].size ());
    }
  }

  void OnLogMessage (HsConnection& sender, const char* message)
  {
    printf ("HsConnection: '%s'\n", message);
  }

  void OnError (HsConnection& sender, ErrorCode code, const char* error)
  {
    engine->Execute (format ("lua: OnHsConnectionError (%d, \"%s\")", code, error).c_str ());
  }

  void OnMessageReceived (HsConnection& sender, unsigned short plugin_id, const unsigned char* message, size_t size)
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

    engine->Execute (format ("lua: OnHsConnectionMessage (%d, '%s')", plugin_id, message).c_str ());
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
