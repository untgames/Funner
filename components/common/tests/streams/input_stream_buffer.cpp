#include "shared.h"

size_t my_read (void* buffer, size_t size)
{
  char src [3] = {'A', 'B', 'C'};

  if (size > sizeof (src))
    size = sizeof (src);

  memcpy (buffer, src, size);

  return size;
}

void test_read (InputStreamBuffer& input_buffer, size_t size)
{
  char* buffer = new char [size];

  size_t read_size = input_buffer.Read (buffer, size);

  printf ("read data (size=%lu, result=%lu): '", size, read_size);

  for (size_t i=0; i<read_size; i++)
    printf ("%c", buffer [i]);

  printf ("'\n");
}

int main ()
{
  printf ("Results of input_stream_buffer_test:\n");

  char user_buffer [] = "Hello world";

  try
  {
    InputStreamBuffer buffer (&my_read, 10);

    test_read (buffer, 12);

    buffer.SetUserBuffer (user_buffer, sizeof (user_buffer) - 1);
    buffer.SetReader     (&InputStreamBuffer::DefaultReader);

    test_read (buffer, 120);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
