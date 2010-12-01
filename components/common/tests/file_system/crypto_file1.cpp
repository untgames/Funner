#include "test.h"

const char*  CRYPTO_FILE_NAME  = "data/crypto.bin";
const char*  RESULTS_FILE_NAME = "/io/stdout";
const char*  BLOCK             = "---ABCDEFGHIJKLMNOPQRSTUVWXYZ---\n";
const char*  REPLACEMENT_BLOCK = "********************************\n";
const char*  END_BLOCK         = "Final";
const size_t BLOCK_COUNT       = 1000;
const char   KEY [16+1]        = "ABCDEFGHIJKLMNOP";
const size_t KEY_BITS          = 8 * (sizeof (KEY) - 1);
const char*  ENCRYPT_METHOD    = "aes.encrypt";
const char*  DECRYPT_METHOD    = "aes.decrypt";

int main ()
{
  printf ("Results of crypto_file1_test:\n");
  
  try
  {
      //запись данных
    
    StdFile out_file (CRYPTO_FILE_NAME, FileMode_ReadWrite);
    CryptoFile crypto_file (out_file, DECRYPT_METHOD, ENCRYPT_METHOD, KEY, KEY_BITS);

    size_t block_size = strlen (BLOCK);

    for (size_t i=0; i<BLOCK_COUNT; i++)
      crypto_file.Write (BLOCK, block_size);
      
    crypto_file.Seek (BLOCK_COUNT / 2 * block_size);
    crypto_file.Write (REPLACEMENT_BLOCK, strlen (REPLACEMENT_BLOCK));

    crypto_file.Seek  (0, FileSeekMode_End);
    crypto_file.Write (END_BLOCK, strlen (END_BLOCK));

    out_file.Close ();
    crypto_file.Close ();
    
      //чтение данных
    
    InputFile in_file (CRYPTO_FILE_NAME);

    crypto_file = CryptoFile (in_file, DECRYPT_METHOD, KEY, KEY_BITS);
    out_file    = OutputFile (RESULTS_FILE_NAME);

    while (!crypto_file.Eof ())
    {
      char buffer [128];
      
      size_t size = crypto_file.Read (buffer, sizeof (buffer));      
      
      out_file.Write (buffer, size);
    }

    crypto_file.Close ();
    in_file.Close ();    

    FileSystem::Remove (CRYPTO_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
