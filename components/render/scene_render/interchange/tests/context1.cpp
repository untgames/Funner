#include "shared.h"

namespace
{

const int CommandId_Dummy = CommandId_FirstUserDefined + 1;
const int CommandId_SetImage = CommandId_Dummy + 1;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class MyClientToServerSerializer: public OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource(const char* name);
    void UnloadResource(const char* name);
    void SetImage(const media::Image&);

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class MyClientToServerDeserializer: public BasicDeserializer, public InputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчер десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Dispatcher> bool Deserialize (CommandId id, Dispatcher& dispatcher);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализатор команд от сервера к клиенту
///////////////////////////////////////////////////////////////////////////////////////////////////
class MyServerToClientSerializer: public OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Dummy(int a, int b);

  protected:
    using OutputStream::Swap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Десериализатор команд от сервера к клиенту
///////////////////////////////////////////////////////////////////////////////////////////////////
class MyServerToClientDeserializer: public BasicDeserializer, public InputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Диспетчер десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Dispatcher> bool Deserialize (CommandId id, Dispatcher& dispatcher);
};

/*
    Client to server
*/

inline void MyClientToServerSerializer::LoadResource(const char* name)
{
  BeginCommand(CommandId_LoadResource);
  write(*this, name);
  EndCommand();
}

inline void MyClientToServerSerializer::UnloadResource(const char* name)
{
  BeginCommand(CommandId_UnloadResource);
  write(*this, name);
  EndCommand();
}

inline void MyClientToServerSerializer::SetImage(const media::Image& image)
{
  BeginCommand(CommandId_SetImage);
  write(*this, image);
  EndCommand();
}

template <class Dispatcher> inline bool MyClientToServerDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_LoadResource:
      dispatcher.LoadResource(read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_UnloadResource:
      dispatcher.UnloadResource(read(*this, xtl::type<const char*> ()));
      return true;
    case CommandId_SetImage:
      dispatcher.SetImage(read(*this, xtl::type<media::Image> ()));
      return true;
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}


/*
    Server to client
*/

inline void MyServerToClientSerializer::Dummy(int a, int b)
{
  BeginCommand(CommandId_Dummy);
  write(*this, a);
  write(*this, b);
  EndCommand();
}

template <class Dispatcher> inline bool MyServerToClientDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_Dummy:
    {
      int arg1 = read(*this, xtl::type<int> ());
      int arg2 = read(*this, xtl::type<int> ());

      dispatcher.Dummy(arg1, arg2);
      return true;
    }
    default:
      return DeserializeUnknownCommand (id, *this);
  }
}

class Processor
{
  public:
    Processor (const char* in_name) : processor_name (in_name) {}
    ~Processor () { printf ("%sProcessor::~Processor()\n", processor_name.c_str ()); }

    void LoadResource (const char* name) { printf ("%sProcessor::LoadResource('%s')\n", processor_name.c_str (), name); };
    void UnloadResource (const char* name) { printf ("%sProcessor::UnloadResource('%s')\n", processor_name.c_str (), name); };
    void Dummy (int a, int b) { printf ("%sProcessor::Dummy(%d, %d)\n", processor_name.c_str (), a, b); }
    void SetImage (const media::Image& image) { printf ("%sProcessor::SetImage(%ux%ux%ux%s)\n", processor_name.c_str (), image.Width (), image.Height (), image.Depth (), get_format_name (image.Format ())); }

  private:
    stl::string processor_name;
};

struct ProcessorHolder
{
  Processor processor;

  ProcessorHolder (const char* name) : processor (name) {}
};

template <class Serializer, class Deserializer>
class Connection: public IConnection, private ProcessorHolder, public Context<Serializer, Deserializer>, public xtl::reference_counter, public xtl::trackable
{
  public:
    typedef Context<Serializer, Deserializer> ConnectionContext;

    Connection (const char* name, bool in_inprocessed) : ProcessorHolder (name), ConnectionContext (processor), inprocessed (in_inprocessed) { }

    bool IsInprocessed () { return inprocessed; }

    void ProcessCommands (const CommandBuffer& commands)
    {
      ConnectionContext::ProcessCommands (commands);
    }

    xtl::trackable& GetTrackable () { return *this; }

    void AddRef  () { addref (this); }
    void Release () { release (this); }    

    using ConnectionContext::SetCounterparty;

  private:
    bool inprocessed;
};

void print_log (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

}

int main ()
{
  try
  {
    printf ("Results of context1_test:\n");

    common::LogFilter filter ("render.*", &print_log);

    typedef Connection<MyClientToServerSerializer, MyServerToClientDeserializer> ClientToServer;
    typedef Connection<MyServerToClientSerializer, MyClientToServerDeserializer> ServerToClient;

    {
      xtl::com_ptr<ClientToServer> client_to_server (new ClientToServer ("Client", true), false);
      xtl::com_ptr<ServerToClient> server_to_client (new ServerToClient ("Server", true), false);

      client_to_server->SetCounterparty (server_to_client.get ());
      server_to_client->SetCounterparty (client_to_server.get ());

      client_to_server->LoadResource ("resource1");
      client_to_server->SetImage (media::Image (4, 4, 1, media::PixelFormat_L8));
      client_to_server->Flush ();

      server_to_client->Dummy (1, 2);
      server_to_client->Flush ();
    }

    {
      xtl::com_ptr<ClientToServer> client_to_server (new ClientToServer ("ClientRemote", false), false);
      xtl::com_ptr<ServerToClient> server_to_client (new ServerToClient ("ServerRemote", false), false);

      client_to_server->SetCounterparty (server_to_client.get ());
      server_to_client->SetCounterparty (client_to_server.get ());

      client_to_server->SetImage (media::Image (4, 4, 1, media::PixelFormat_L8));
      client_to_server->Flush ();
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }  

  return 0;
}
