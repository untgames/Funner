#include "shared.h"

#include "shared.h"

using namespace network;

const unsigned short CONNECTION_PORT = 12973;
const char*          MESSAGE1        = "First hello from socket!";
const char*          MESSAGE2        = "Last hello from socket!";

int client ()
{
  try
  {
    TcpClient client (InetAddress::Loopback (), CONNECTION_PORT);    

    while (!client.IsClosed ())
    {
      char receive_buffer [10];

      memset (receive_buffer, 0, sizeof (receive_buffer));
      
      size_t required_size = sizeof (receive_buffer);
          
      if (!client.ReceiveExactly (receive_buffer, required_size, 3000))
      {      
        memset (receive_buffer, 0, sizeof (receive_buffer));
        
        size_t size = client.Receive (receive_buffer, required_size, 1000);
        
        printf ("Receive failed. Received %u bytes\n", size);
      }

      printf ("Received text '%s'\n", receive_buffer);
    }
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
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at server thread: '%s'\n", e.what ());
  }

  return 0;
}

int main ()
{
  printf ("Results of tcp_client_test:\n");  

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
