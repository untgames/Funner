#include "shared.h"

int main ()
{
  printf ("Results of output_stage1_test:\n");
  
  try
  {
    Test test ("OpenGL device test window (output_stage1)");

    IFrameBuffer* buffer = test.device->OSGetFrameBuffer ();
    
    dump_desc (*buffer);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
