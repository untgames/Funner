#include "shared.h"

using namespace network;

const unsigned short CONNECTION_PORT = 12973;
const char*          MESSAGE         = "Hello from socket!";

int client ()
{
  try
  {
    Socket socket (SocketDomain_IpV4, SocketProtocol_Tcp);

    socket.Connect (SocketAddress (InetAddress::Loopback (), CONNECTION_PORT));

    char receive_buffer [128];

    memset (receive_buffer, 0, sizeof (receive_buffer));

    printf ("Received %u bytes\n", socket.Receive (receive_buffer, sizeof (receive_buffer)));

    printf ("Received text '%s'\n", receive_buffer);
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

    client_socket.Send (MESSAGE, xtl::xstrlen (MESSAGE) + 1);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at server thread: '%s'\n", e.what ());
  }

  return 0;
}

int main ()
{
  printf ("Results of socket1_test:\n");

  try
  {
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
