#include "test.h"

const char* FILE_NAME = "data/test.txt";

int main ()
{
  printf ("Results of file_crypto_key_test:\n");
  
  try
  {
    filecryptokey_t key;
    
    FileSystem::GetFileCryptoKey (FILE_NAME, key);
    
    char key_string [sizeof (key) * 2 + 1];
    
    decompress_buffer (sizeof (key), key, key_string);
    
    printf ("key: '%s'\n", key_string);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
 