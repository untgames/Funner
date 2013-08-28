#include "shared.h"

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализатор команд от клиента к серверу
///////////////////////////////////////////////////////////////////////////////////////////////////
class MyClientToServerSerializer: private OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource(const char* name);
    void UnloadResource(const char* name);

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
class MyServerToClientSerializer: private OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды сериализации (кодогенерация)
///////////////////////////////////////////////////////////////////////////////////////////////////

    void Dummy();

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
    default:
      return DeserializeUnknownCommand (id);
  }
}


/*
    Server to client
*/

inline void MyServerToClientSerializer::Dummy()
{
  BeginCommand(CommandId_Dummy);

  EndCommand();
}

template <class Dispatcher> inline bool MyServerToClientDeserializer::Deserialize(CommandId id, Dispatcher& dispatcher)
{
  switch (id)
  {
    case CommandId_Dummy:
      dispatcher.Dummy();
      return true;
    default:
      return DeserializeUnknownCommand (id);
  }
}

class Processor
{
  public:
    Processor (const char* in_name) : processor_name (in_name) {}
    ~Processor () { printf ("%sProcessor::~Processor()\n", processor_name.c_str ()); }

    void LoadResource (const char* name) { printf ("%sProcessor::LoadResource('%s')\n", processor_name.c_str (), name); };
    void UnloadResource (const char* name) { printf ("%sProcessor::UnloadResource('%s')\n", processor_name.c_str (), name); };
    void Dummy () { printf ("%sProcessor::Dummy()\n", processor_name.c_str ()); }

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
    Connection (const char* name) : ProcessorHolder (name), Context (processor) { }

    void ProcessCommands (const CommandBuffer& commands)
    {
      Context::ProcessCommands (commands);
    }

    xtl::trackable& GetTrackable () { return *this; }

    void AddRef  () { addref (this); }
    void Release () { release (this); }    

    using Context::SetCounterparty;
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

    xtl::com_ptr<ClientToServer> client_to_server (new ClientToServer ("Client"), false);
    xtl::com_ptr<ServerToClient> server_to_client (new ServerToClient ("Server"), false);

    client_to_server->SetCounterparty (server_to_client.get ());
    server_to_client->SetCounterparty (client_to_server.get ());

    client_to_server->LoadResource ("resource1");
    client_to_server->Flush ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }  

  return 0;
}
