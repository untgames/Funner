#include "shared.h"

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of buffers_test:\n");

  try
  {
//    common::LogFilter filter ("*", &log_print);

    Test test (L"DX11 device test window (buffers_test)", "debug=1");
    
    BufferDesc index, vertex;
    
    index.size = 1;
    index.usage_mode = UsageMode_Default;
    index.bind_flags = BindFlag_IndexBuffer;
    index.access_flags = AccessFlag_ReadWrite;
    
    vertex.size = 1;
    vertex.usage_mode = UsageMode_Default;
    vertex.bind_flags = BindFlag_VertexBuffer;
    vertex.access_flags = AccessFlag_ReadWrite;
      
    test.device->GetImmediateContext ()->ISSetVertexBuffer(0, test.device.get()->CreateBuffer(vertex));
    test.device->GetImmediateContext ()->ISSetIndexBuffer(test.device.get()->CreateBuffer(index));
    test.device->GetImmediateContext ()->Draw (PrimitiveType_PointList, 0, 0);

    printf ("done\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
