#include "shared.h"

using namespace network;

const unsigned short CONNECTION_PORT = 12973;
const char*          MESSAGE1        = "First hello from socket!";
const char*          MESSAGE2        = "Last hello from socket!";

volatile bool client_joined = false;
volatile bool server_data_received = false;

int client ()
{
  try
  {
    TcpClient client (InetAddress::Loopback (), CONNECTION_PORT);    

    client.SwitchToAsyncReceiving ();
    client.SwitchToAsyncSending ();

    client.Send (MESSAGE1);

    while (!server_data_received) syslib::Application::Sleep (1000);
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

    stl::string buffer;

    size_t required_size = xtl::xstrlen (MESSAGE1);

    while (buffer.size () < required_size)
    {
      char tmp [10];

      size_t recv_size = client_socket.Receive (tmp, sizeof (tmp), 1000);

      if (!recv_size)
        continue;

      buffer.append (tmp, recv_size);
    }

    server_data_received = true;

    printf ("Server: received '%s'\n", buffer.c_str ());
    fflush (stdout);

    client_socket.Send (MESSAGE2, (unsigned int)strlen (MESSAGE2));

    while (!client_joined) syslib::Application::Sleep (1000);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at server thread: '%s'\n", e.what ());
  }

  return 0;
}

int main ()
{
  printf ("Results of tcp_client_async_waiting_test:\n");  

  try
  {
    common::LogFilter filter ("*", &print_log);

    syslib::Thread server_thread (&server);

    syslib::Application::Sleep (100);

    syslib::Thread client_thread (&client);

    client_thread.Join ();

    client_joined = true;

    server_thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
