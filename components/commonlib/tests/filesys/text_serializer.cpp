#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;
using namespace math;

const char* RESULTS_FILE_NAME = "media/tests/filesys/text_serializer_results.txt";

int main ()
{
  printf ("Results of text_serializer_test:\n");

  try
  {
    OutputTextStream stream (RESULTS_FILE_NAME);
    TextSerializer   serializer (stream,", ");

    serializer.Write          ("Hello world");
    serializer.WriteSeparator ();
    serializer.Write          (int (-1));
    serializer.WriteSeparator ();
    serializer.Write          (size_t (-1));
    serializer.WriteSeparator ();
    serializer.Write          (float (M_PI));
    serializer.WriteSeparator ();
    serializer.Write          (double (M_PI));
    serializer.WriteSeparator ();
    serializer.Write          ((long double)(M_PI));
    serializer.WriteSeparator ();
    serializer.Write          (true);
    serializer.WriteSeparator ();
    serializer.Write          (false);
    serializer.WriteSeparator ();    
    serializer.Write          ("(");
    serializer.Write          (vec4f (1.1f,2.2f,3.3f,4.4f));
    serializer.Write          (")");
    stream.Printf             ("\n");
    serializer.Write          ("(");
    serializer.Write          (mat4f (1.0f));
    serializer.Write          (")");
    serializer.WriteSeparator ();
    serializer.Write          ("(");
    serializer.Write          (quatf (1.0f));
    serializer.Write          (")");
    serializer.WriteSeparator ();
    serializer.Write          ("(");

    vector<int> v (5);

    for (size_t i=0;i<v.size ();i++)
      v [i] = i;

    copy (v.begin (),v.end (),make_iterator<int> (serializer));    
    
    serializer.Write          (")");
    serializer.WriteSeparator ();
    serializer.Write          ("(");
    
    fill_n (make_iterator<char> (serializer),5,'x');
    
    serializer.Write          (")");
    
    stream.UnbindFile ();
    
    PrintFileInfo (RESULTS_FILE_NAME);

    FileSystem::Remove (RESULTS_FILE_NAME);    
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
