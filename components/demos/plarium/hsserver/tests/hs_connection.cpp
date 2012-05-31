#include "shared.h"

using namespace plarium::system;
using namespace plarium::hsserver;

class HsConnectionListener : public IHsConnectionListener
{
  public:
    void OnMessageReceived (HsConnection* sender, unsigned short plugin_id, const char* event)
    {
      printf ("OnMessageReceived! Plugin %d, message '%s'\n", plugin_id, event);
    }

    void OnDisconnect (HsConnection* sender)
    {
      printf ("OnDisconnect!\n");
    }
};

int main ()
{
  printf ("Results of hs_connection_test:\n");

  try
  {
    HsConnectionListener listener;
    HsConnection connection;

    connection.SetListener (&listener);

    unsigned char key [16] = { 0xd0, 0x71, 0x73, 0x41, 0x0d, 0xee, 0xd2, 0x6f, 0x44, 0x3f, 0x7b, 0xa4, 0x42, 0x2f, 0x86, 0x03 };

    connection.SetEncryptionEnabled (true);
    connection.SetEncryptionKey (key, sizeof (key) * 8);

    unsigned char address [4];

    address [0] = 109;
    address [1] = 234;
    address [2] = 156;
    address [3] = 146;

    connection.Connect (address, 9999);

    const char* message_text =  "[{"
      " \"event\":\"user.authenticate\","
      " \"lastName\":\"Anonym\","
      " \"firstName\":\"Anonym\","
      " \"token\":\"d666bbd49c325786d335a3a3e8d3dbd8c5dd3f6d3f6847306f20a6ac2263fc4\","
      " \"photo\":\"http://cs912.userapi.com/u93339557/a_57700b24.jpg\","
      " \"network\":\"VK\","
      " \"loginId\":\"93339557\","
      " \"version\":1"
      "}]";

    connection.SendMessage (0, message_text);

    Thread::Sleep (1000);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
