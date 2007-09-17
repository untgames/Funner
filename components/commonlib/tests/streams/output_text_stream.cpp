#include "shared.h" 

int main ()
{
  printf ("Results of output_text_stream_test:\n");
  
  try
  {
    OutputTextStream stream (&dump);

    write (stream, "Hello world\n");
    write (stream, 12, "-###");
    write (stream, "\n");
    write (stream, (size_t)-1, "hex:000");
    write (stream, "\n");
    write (stream, 3.14f);
    write (stream, "\n");
    write (stream, 3.14f, "-.#");
    write (stream, "\n");
    write (stream, 3.14f, "###.000");
    write (stream, "\n");
    write (stream, 3.14f, "-000.0");
    write (stream, "\n");
    write (stream, true, "alpha");
    write (stream, "\n");
    write (stream, false);
    write (stream, "\n");    
    write (stream, 'A');
    write (stream, "\n");
    write (stream, "(");
    write (stream, vec4f (1.1f, 2.2f, 3.3f, 4.4f));
    write (stream, ")\n");
    write (stream, "(");
    write (stream, mat4f ());
    write (stream, ")\n");
    write (stream, "(");
    write (stream, quatf (1.0f));
    write (stream, ")\n");
    write (stream, stl::string ("Hello world\n"));
    
    int array [] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    write_range (stream, array, array + 10, "hex:00", ", ");
    write       (stream, "\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
