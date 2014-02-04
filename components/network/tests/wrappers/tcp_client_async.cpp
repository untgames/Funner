#include "shared.h"

using namespace network;

const unsigned short CONNECTION_PORT = 12974;
const char*          MESSAGE1        = "First hello from socket!";
const char*          MESSAGE2        = "Last hello from socket!";

struct Receiver
{
  TcpClient&           client;
  stl::string          buffer;
  xtl::auto_connection on_data_received_connection;

  Receiver (TcpClient& in_client) : client (in_client)
  {
    on_data_received_connection = client.RegisterAsyncReceivingEventHandler (xtl::bind (&Receiver::OnDataReceived, this, _1, _2));
  }

  void OnDataReceived (const void* data, size_t size)
  {
      //mutex is needed in normal case

    buffer.append ((const char*)data, size);

    if (buffer.size () == xtl::xstrlen (MESSAGE1) + xtl::xstrlen (MESSAGE2))
      client.Send (MESSAGE1, xtl::xstrlen (MESSAGE1));
  }
};

int client ()
{
  try
  {
    TcpClient client (InetAddress::Loopback (), CONNECTION_PORT);    

    Receiver receiver (client);

    client.SwitchToAsyncReceiving ();
    client.SwitchToAsyncSending ();

    while (!client.IsClosed ())
      syslib::Application::Sleep (1000);

    printf ("Client: received '%s'\n", receiver.buffer.c_str ());
    fflush (stdout);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at client thread '%s'\n", e.what ());
  }

  return 0;
}

int server ()
{
  try
  {
    Socket socket (SocketDomain_IpV4, SocketProtocol_Tcp);

    unsigned char address [] = { 0, 0, 0, 0 };

    socket.Bind (SocketAddress (InetAddress (address), CONNECTION_PORT));

    socket.Listen ();

    Socket client_socket = socket.Accept ();

    client_socket.Send (MESSAGE1, xtl::xstrlen (MESSAGE1));
    
    syslib::Application::Sleep (1000);
        
    client_socket.Send (MESSAGE2, xtl::xstrlen (MESSAGE2));

    stl::string buffer;

    size_t required_size = xtl::xstrlen (MESSAGE1);

    while (buffer.size () != required_size)
    {
      char tmp [10];

      size_t recv_size = client_socket.Receive (tmp, sizeof (tmp));

      if (!recv_size)
        continue;

      buffer.append (tmp, recv_size);
    }

    printf ("Server: received '%s'\n", buffer.c_str ());
    fflush (stdout);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at server thread: '%s'\n", e.what ());
  }

  return 0;
}

int main ()
{
  printf ("Results of tcp_client_async_test:\n");  

  try
  {
    common::LogFilter filter ("*", &print_log);

    syslib::Thread server_thread (&server);

    syslib::Application::Sleep (100);

    syslib::Thread client_thread (&client);

    client_thread.Join ();
    server_thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
