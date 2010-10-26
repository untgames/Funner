#include <cstdio>
#include <climits>
#include <cstring>
#include <exception>

#include <common/crypto.h>

using namespace common;

char key [16+1]                             = "ABCDEFGHIJKLMNOP";
char src_buffer [16 * 2 + 1]                = "Hello world from the AES encrypt";
char dst_encrypt_buffer [sizeof src_buffer];
char dst_decrypt_buffer [sizeof src_buffer];

const size_t KEY_BITS    = CHAR_BIT * (sizeof (key) - 1);
const size_t BUFFER_SIZE = sizeof src_buffer - 1;

int main ()
{
  printf ("Results of aes_context_test:\n");
  
  try
  {  
    CryptoContext context ("aes.encrypt", key, KEY_BITS);

    memset (dst_encrypt_buffer, 0, sizeof dst_decrypt_buffer);        
    
    printf ("source:             '%s'\n", src_buffer);
    printf ("encrypt update:      %u\n",  context.Update (BUFFER_SIZE, src_buffer, dst_encrypt_buffer));
    printf ("dst_encrypt_buffer:  ");
    
    for (size_t i=0; i<BUFFER_SIZE; i++)
      printf ("%02x", (unsigned char)dst_encrypt_buffer [i]);
      
    printf ("\n");

    CryptoContext ("aes.decrypt", key, KEY_BITS).Swap (context);
    
    memset (dst_decrypt_buffer, 0, sizeof dst_decrypt_buffer);

    printf ("decrypt update:      %u\n",  context.Update (BUFFER_SIZE, dst_encrypt_buffer, dst_decrypt_buffer));  
    printf ("dst_decrypt_buffer: '%s'\n", dst_decrypt_buffer);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
