#include "test.h"

const char*  CRYPTO_FILE_NAME  = "data/crypto_with_key_file/crypted-file.crypto.zip";
const char*  SEARCH_PATH       = "data/crypto_with_key_file";
const char*  SOURCE_FILE       = "crypted-file.txt";
const char*  RESULTS_FILE_NAME = "/io/stdout";
const char*  KEY_FILE          = "data/crypto_with_key_file/key.txt";
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
  printf ("Results of crypto_zip_key_file_test:\n");
  
  try
  {
      //получение ключа
      
    filecryptokey_t crypto_key;
      
    FileSystem::GetFileCryptoKey (KEY_FILE, crypto_key);    

      //преобразование ключа      

    FileCryptoParameters crypto_params (DECRYPT_METHOD, ENCRYPT_METHOD, crypto_key, sizeof (crypto_key) * CHAR_BIT);
    
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
