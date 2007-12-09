#include "shared.h"

int main ()
{
  printf ("Results of output_stage2_test:\n");
  
  try
  {
    Test test ("OpenGL device test window (output_stage2)");

    FrameBufferDesc desc;

    test.device->OSGetFrameBuffer ()->GetDesc (desc);

    desc.width  = 256;
    desc.height = 1024;

    IFrameBuffer* buffer = test.device->CreateFrameBuffer (desc);
    
    dump_desc (*buffer);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
