#include <cstdio>
#include <climits>
#include <exception>

#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/strlib.h>

using namespace common;

const char*  ENCRYPT_METHOD = "aes.encrypt";
const char*  DECRYPT_METHOD = "aes.decrypt";
const size_t BUFFER_SIZE    = 128*1024;

int main (int argc, char* argv [])
{
  try
  {
    if (argc < 5)
    {
      printf ("file-crypter encrypt <src_file_name> <dst_file_name> <key-file>\n");
      printf ("file-crypter decrypt <src_file_name> <dst_file_name> <key-file>\n");
      return 1;
    }
        
    const char* method        = argv [1];
    const char* src_file_name = argv [2];
    const char* dst_file_name = argv [3];
    const char* key_file_name = argv [4];
    
    const char* read_method  = "";
    const char* write_method = "";    

    filecryptokey_t key;
      
    FileSystem::GetFileCryptoKey (key_file_name, key);

    if (!xtl::xstricmp (method, "encrypt"))
    {      
      read_method  = DECRYPT_METHOD;
      write_method = ENCRYPT_METHOD;
    }
    else if (!xtl::xstricmp (method, "decrypt"))
    {
      write_method = DECRYPT_METHOD;
      read_method  = ENCRYPT_METHOD;
    }
    else
    {
      printf ("unknown method '%s'\n", method);
      return 1;
    }

      //шифрование

    InputFile  in_file (src_file_name);          
    StdFile    out_bin_file (dst_file_name, FileMode_ReadWrite);
    CryptoFile out_file (out_bin_file, read_method, write_method, key, sizeof (key) * CHAR_BIT);
    
    size_t file_size      = in_file.Size (),
           processed_size = 0;
           
    xtl::uninitialized_storage<char> buffer (BUFFER_SIZE);

    while (!in_file.Eof ())
    {
      size_t size = in_file.Read (buffer.data (), buffer.size ());
      
      if (!size)
      {
        fprintf (stderr, "error at read data from source file '%s'\n", src_file_name);
        
        FileSystem::Remove (dst_file_name);

        return 1;
      }      
      
      size_t result = out_file.Write (buffer.data (), size);      
      
      if (result != size)
      {
        fprintf (stderr, "error at write data to destination file '%s'\n", dst_file_name);

        FileSystem::Remove (dst_file_name);

        return 1;        
      }
      
      processed_size += size;
      
//      printf ("\rProgress %.1f%%", 100.0f * float (processed_size)/float (file_size));
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
