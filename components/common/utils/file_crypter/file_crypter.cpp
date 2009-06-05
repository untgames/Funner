#include <cstdio>
#include <climits>
#include <exception>

#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/strlib.h>

using namespace common;

const char* ENCRYPT_METHOD = "aes.encrypt";
const char* DECRYPT_METHOD = "aes.decrypt";

int main (int argc, char* argv [])
{
  try
  {
    if (argc < 4)
    {
      printf ("file-crypter encrypt <src_file_name> <dst_file_name>\n");
      printf ("file-crypter decrypt <src_file_name> <dst_file_name> <key>\n");
      return 1;
    }
        
    const char* method        = argv [1];
    const char* src_file_name = argv [2];
    const char* dst_file_name = argv [3];
    
    const char* read_method  = "";
    const char* write_method = "";    

    xtl::uninitialized_storage<char> key;

    if (!xtl::xstricmp (method, "encrypt"))
    {      
      FileHash src_file_hash;

      FileSystem::GetFileHash (src_file_name, src_file_hash);
      
      printf ("key: '");

      for (int i=0; i<16; i++)
        printf ("%02x", src_file_hash.md5 [i]);

      printf ("'\n");    
      
      key.resize (sizeof (src_file_hash.md5));
      
      memcpy (key.data (), src_file_hash.md5, key.size ());
      
      read_method  = DECRYPT_METHOD;
      write_method = ENCRYPT_METHOD;
    }
    else if (!xtl::xstricmp (method, "decrypt"))
    {
      if (argc < 5)
      {
        printf ("file-crypter decrypt <src_file_name> <dst_file_name> <key>\n");
        return 1;
      }
      
      const char* key_string = argv [4];
      size_t      key_length = strlen (key_string);

      key.resize (key_length / 2);

      compress_buffer (key_length, key_string, key.data ());            

      write_method = DECRYPT_METHOD;
      read_method  = ENCRYPT_METHOD;
    }
    else
    {
      printf ("unknown method '%s'\n", method);
      return 1;
    }

      //запись данных      

    InputFile  in_file (src_file_name);          
    StdFile    out_bin_file (dst_file_name, FileMode_ReadWrite);
    CryptoFile out_file (out_bin_file, read_method, write_method, key.data (), key.size () * CHAR_BIT);
   
      //чтение данных

    while (!in_file.Eof ())
    {
      char buffer [128];
      
      size_t size = in_file.Read (buffer, sizeof (buffer));      
      
      if (!size)
      {
        fprintf (stderr, "error at read data from source file '%s'\n", src_file_name);
        
        FileSystem::Remove (dst_file_name);

        return 1;
      }      
      
      size_t result = out_file.Write (buffer, size);      
      
      if (result != size)
      {
        fprintf (stderr, "error at write data to destination file '%s'\n", dst_file_name);

        FileSystem::Remove (dst_file_name);

        return 1;        
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
