#include "shared.h"

struct State
{
  IInputLayout* layout;
  IBuffer*      index_buffer;
  IBuffer*      vertex_buffer [DEVICE_VERTEX_BUFFER_SLOTS_COUNT];
  
  void Init (IDevice& device)
  {
    layout       = device.ISGetInputLayout ();
    index_buffer = device.ISGetIndexBuffer ();

    for (int i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      vertex_buffer [i] = device.ISGetVertexBuffer (i);
  }
  
  void Check (const State& src)
  {
    printf ("ISGetInputLayout: %d\n", layout == src.layout);
    printf ("ISGetIndexBuffer: %d\n", index_buffer == src.index_buffer);
    
    for (int i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      printf ("ISGetVertexBuffer(%d): %d\n", i, vertex_buffer [i] == src.vertex_buffer [i]);
  }
};

int main ()
{
  printf ("Results of unit_input_stage_state_block_test:\n");
  
  try
  {
    Test test;
    
    BufferDesc buffer_desc;
    
    memset (&buffer_desc, 0, sizeof buffer_desc);
    
    buffer_desc.size       = 12;
    buffer_desc.usage_mode = UsageMode_Default;
    buffer_desc.bind_flags = BindFlag_VertexBuffer;
    buffer_desc.access_flags = AccessFlag_ReadWrite;
    
    BufferPtr vertex_buffer (test.device->CreateBuffer (buffer_desc), false);
    
    for (int i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      test.device->ISSetVertexBuffer (i, vertex_buffer.get ());
      
    buffer_desc.bind_flags = BindFlag_IndexBuffer;
    
    BufferPtr index_buffer (test.device->CreateBuffer (buffer_desc), false);
    
    test.device->ISSetIndexBuffer (index_buffer.get ());

    State src_state;

    src_state.Init (*test.device);    

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_InputStage);
    
    mask.is_vertex_buffers [3] = false;
    
    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);

    state_block->Capture ();
    
    test.device->ISSetInputLayout (0);
    test.device->ISSetIndexBuffer (0);
    
    for (int i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      test.device->ISSetVertexBuffer (i, 0);

    printf ("after reset\n");
    
    State dst_state;
    
    dst_state.Init (*test.device);    
    dst_state.Check (src_state);
    
    state_block->Apply ();

    printf ("after apply\n");    

    dst_state.Init (*test.device);    
    dst_state.Check (src_state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
