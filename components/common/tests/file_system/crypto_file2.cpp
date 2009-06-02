#include "test.h"

const char*  CRYPTO_FILE_NAME = "data/crypto.bin";
const char   KEY [16+1]       = "ABCDEFGHIJKLMNOP";
const size_t KEY_BITS         = 8 * (sizeof (KEY) - 1);
const char*  ENCRYPT_METHOD   = "aes.encrypt";
const char*  DECRYPT_METHOD   = "aes.decrypt";

int main ()
{
  printf ("Results of crypto_file2_test:\n");
  
  try
  {
    StdFile file (CRYPTO_FILE_NAME, FileMode_ReadWrite);
    CryptoFile crypto_file (file, DECRYPT_METHOD, ENCRYPT_METHOD, KEY, KEY_BITS);

    printf ("file size: %u / %u\n", crypto_file.Size (), file.Size ());
    
    crypto_file.Resize (113);
    
    printf ("file size: %u / %u\n", crypto_file.Size (), file.Size ());    

    crypto_file.Close ();
    file.Close ();

    FileSystem::Remove (CRYPTO_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
