#include "shared.h"

const char* SHADER_FILE_NAME = "data/fpp_shader.wxf";

#pragma pack (1)

struct MyShaderParameters
{
  math::vec3f light_position;
};

struct State
{
  IProgram*                 program;
  IProgramParametersLayout* program_parameters_layout;
  IBuffer*                  buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];
  
  void Init (IDevice& device)
  {
    program_parameters_layout = device.SSGetProgramParametersLayout ();
    program                   = device.SSGetProgram ();

    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      buffers [i] = device.SSGetConstantBuffer (i);
  }  

  void Check (const State& src)
  {
    printf ("SSGetProgramParametersLayout: %d\n", program_parameters_layout == src.program_parameters_layout);
    printf ("SSGetProgram: %d\n", program == src.program);    
      
    for (size_t i=0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      printf ("SSGetConstantBuffer(%u): %d\n", i, buffers [i] == src.buffers [i]);      
  }
};

void print (const char* message)
{
}

int main ()
{
  printf ("Results of unit_shader_stage_state_block_test:\n");
  
  try
  {
    Test test;        

    ShaderDesc shader_descs [] = {
      {"fpp_shader", ~0, "", "fpp", ""},
    };    

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print));        

    static ProgramParameter shader_parameters [] = {
      {"LightPosition", ProgramParameterType_Float3, 3, 1, 0},
    };    
    
    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof (shader_parameters) / sizeof (*shader_parameters), shader_parameters};

    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc));        

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr buffer (test.device->CreateBuffer (cb_desc), false);
    
    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    
    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      test.device->SSSetConstantBuffer (i, buffer.get ());

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_ShaderStage);
    
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      mask.ss_samplers [i] = false;
      mask.ss_textures [i] = false;
    }

    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);
    
    state_block->Capture ();

    test.device->SSSetProgram (0);
    test.device->SSSetProgramParametersLayout (0);

    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      test.device->SSSetConstantBuffer (i, 0);    

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
