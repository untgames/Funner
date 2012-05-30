#include "shared.h"

#include <system/tcp_client.h>

#include <utility/crypto.h>
#include <utility/hash.h>

using namespace plarium::system;
using namespace plarium::utility;

void print_data (const unsigned char* data, size_t data_size)
{
  for (size_t i = 0; i < data_size; i++)
    printf ("%02x", data [i]);

  printf ("\n");
}

unsigned int endian_swap (unsigned int x)
{
  return (x>>24) |
         ((x<<8) & 0x00FF0000) |
         ((x>>8) & 0x0000FF00) |
         (x<<24);
}

int main ()
{
  printf ("Results of tcp_client_test:\n");

  try
  {
    TcpClient tcp_client;

    unsigned char address [4];

    address [0] = 109;
    address [1] = 234;
    address [2] = 156;
    address [3] = 146;

    tcp_client.Connect (address, 9999);

    unsigned char send_buffer [1024];

    send_buffer [0] = 0x06;

    send_buffer [1] = 0x02;

    send_buffer [2] = 0;
    send_buffer [3] = 0;

    unsigned char* hmac    = send_buffer + 8;
    unsigned char* iv      = hmac + 32;
    unsigned char* message = iv + 16;

    memset (iv, 0, 16);

    //http://oauth.vk.com/authorize?client_id=2972852&scope=friends&redirect_uri=http://response.com&display=page&response_type=token

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

    size_t message_text_length = strlen (message_text);

    unsigned char key [16] = { 0xd0, 0x71, 0x73, 0x41, 0x0d, 0xee, 0xd2, 0x6f, 0x44, 0x3f, 0x7b, 0xa4, 0x42, 0x2f, 0x86, 0x03 };

    aes_ofb (key, sizeof (key) * 8, message_text_length, (const unsigned char*)message_text, message, iv);

    unsigned char hashed_key [32];

    sha256 (key, 16, hashed_key);

    unsigned char hmac2 [32];

    hmac_sha256 (hashed_key, sizeof (hashed_key), iv, 16 + message_text_length, hmac2);

    printf ("calc hmac for data size %d, data: ", 16 + message_text_length);
    print_data (hmac2, 32);

    hmac_sha256 (hashed_key, sizeof (hashed_key), iv, 16 + message_text_length, hmac);
//    strcpy (message, "{\"event\":\"slot.spin\", \"slotId\":1, \"betId\":1, \"lines\":1}");

    int little_endian_size = message_text_length + 32 + 16,
        big_endian_size    = little_endian_size;//endian_swap (little_endian_size);

    int* message_size = (int*)(send_buffer + 4);

    *message_size = big_endian_size;

    printf ("message = '%s' length = %d\n", message_text, little_endian_size);

    tcp_client.Send (send_buffer, 8 + little_endian_size);

    printf ("message size %d: ", little_endian_size);
    print_data ((const unsigned char*)message_size, sizeof (int));

    printf ("Sent data: ");
    print_data (send_buffer, 8 + little_endian_size);

    unsigned char buffer [1024];

    printf ("sent!\n");

    tcp_client.Receive (buffer, 16);

    printf ("Received!\n");
    print_data (buffer, 16);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
