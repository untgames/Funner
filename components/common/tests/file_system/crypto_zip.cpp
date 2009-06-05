#include "test.h"

const char*  CRYPTO_FILE_NAME  = "data/crypto/crypto.zip";
const char*  SEARCH_PATH       = "data/crypto";
const char*  SOURCE_FILE       = "crypto/test.txt";
const char*  RESULTS_FILE_NAME = "/io/stdout";
const char   KEY_STRING []     = "65228baf726de4b55dcad3c1388546a3";
const char*  ENCRYPT_METHOD    = "aes.encrypt";
const char*  DECRYPT_METHOD    = "aes.decrypt";

void PrintLog (const char* message)
{
  printf ("%s\n",message);
}

void ReadFileContent (const char* file_name)
{
  InputFile file (file_name);

  char buf [128];

  printf ("File content: '");

  while (!file.Eof ())
  {
    size_t read_size = file.Read (buf,sizeof (buf)-1);

    buf [read_size] = 0;

    printf ("%s",buf);
  }

  printf ("'\n");
}

int main ()
{
  printf ("Results of crypto_zip_test:\n");
  
  try
  {
      //преобразование ключа
      
    FileCryptoParameters crypto_params (DECRYPT_METHOD, ENCRYPT_METHOD, KEY_STRING);
    
      //установка параметров шифрования

    FileSystem::SetCryptoParameters (CRYPTO_FILE_NAME, crypto_params);
    FileSystem::AddSearchPath (SEARCH_PATH, &PrintLog);

      //чтение файла
      
    ReadFileContent (SOURCE_FILE);    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
