#include "shared.h"

namespace
{

class Processor
{
  public:
    Processor (const char* in_name) : processor_name (in_name) {}

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
class Connection: public IConnection, private ProcessorHolder, public Context<Serializer, Deserializer>, public xtl::reference_counter
{
  public:
    typedef render::scene::interchange::IConnectionListener IConnectionListener;

    Connection (const char* name) : ProcessorHolder (name), Context (processor) { }

    void Send (const CommandBuffer& commands)
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnIncomingCommands (commands);
    }

    void ProcessIncomingCommands () {}

    void AttachListener (IConnectionListener* listener) { listeners.push_back (listener); }
    void DetachListener (IConnectionListener* listener) { listeners.erase (stl::remove (listeners.begin (), listeners.end (), listener), listeners.end ()); }

    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    typedef stl::vector<IConnectionListener*> ListenerArray;

  private:
    ListenerArray listeners;
};

}

int main ()
{
  try
  {
    printf ("Results of context1_test:\n");

    typedef Connection<ClientToServerSerializer, ClientToServerDeserializer> ClientToServer;
    typedef Connection<ServerToClientSerializer, ServerToClientDeserializer> ServerToClient;

    xtl::com_ptr<ClientToServer> client_to_server (new ClientToServer ("Client"), false);
    xtl::com_ptr<ServerToClient> server_to_client (new ServerToClient ("Server"), false);

    client_to_server->SetConnection (server_to_client);
    server_to_client->SetConnection (client_to_server);

    client_to_server->LoadResource ("resource1");
    client_to_server->Flush ();

    client_to_server->SetConnection (xtl::com_ptr<IConnection> ());
    server_to_client->SetConnection (xtl::com_ptr<IConnection> ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }  

  return 0;
}
