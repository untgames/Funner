#include "shared.h"

size_t write (const void* buffer, size_t size)
{
  size_t result = size;

  printf ("flush %u bytes: '", size);
  
  const char* s = (const char*)buffer;
  
  for (;size--; s++)
    printf ("%c", *s);
    
  printf ("'\n");
  
  return result ? result - 1 : 0;
}

void test_write (OutputStreamBuffer& buffer, const char* string)
{
  size_t len = strlen (string);

  printf ("write string '%s' (size=%u). result=%u\n", string, len, buffer.Write (string, len));
}

int main ()
{
  printf ("Results of output_stream_buffer_test:\n");
  
  char user_buffer [10];
  
  memset (user_buffer, '*', sizeof (user_buffer));
  
  try
  {   
    OutputStreamBuffer buffer (&write, 10);
    
    test_write (buffer, "Hello world");
    
    buffer.SetWriter (&OutputStreamBuffer::DefaultWriter);
    
    buffer.SetUserBuffer (user_buffer, sizeof (user_buffer));
    
    buffer.Resize (8);
    
    test_write (buffer, "Hello world");
    
    buffer.Resize (110);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  printf ("user buffer: '");
  
  for (size_t i=0; i<sizeof (user_buffer); i++)
    printf ("%c", user_buffer [i]);
    
  printf ("'\n");

  return 0;
}
