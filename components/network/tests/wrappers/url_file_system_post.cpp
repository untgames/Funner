#include "shared.h"

const size_t BUFFER_SIZE = 16384;

int main ()
{
  printf ("Results of url_file_system_post_test:\n");

  try
  {
    common::LogFilter filter ("network.*", &print_log);
    
    TestHttpServer http_server;
    
    common::InputFile input_file ("data/post.txt");
    common::StdFile output_file ("http://localhost:81/test.cgi", common::FileMode_ReadWrite);

    xtl::uninitialized_storage<char> buffer (BUFFER_SIZE);

    while (!input_file.Eof ())
    {
      size_t read_size = input_file.Read (buffer.data (), buffer.size ());

      if (!read_size)
        break;

      output_file.Write (buffer.data (), read_size);
    }
    
    while (!output_file.Eof ())
    {
      size_t read_size = output_file.Read (buffer.data (), buffer.size ());

      if (!read_size)
        break;

      buffer.data ()[read_size - 1] = 0;
      
      printf ("%s", buffer.data ());
    }
    
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
