#include "shared.h"

const char* SHADER_FILE_NAME = "data/ffp_shader.wxf";

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
    program_parameters_layout = device.GetImmediateContext ()->SSGetProgramParametersLayout ();
    program                   = device.GetImmediateContext ()->SSGetProgram ();

    for (unsigned int i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      buffers [i] = device.GetImmediateContext ()->SSGetConstantBuffer (i);
  }  

  void Check (const State& src)
  {
    printf ("SSGetProgramParametersLayout: %d\n", program_parameters_layout == src.program_parameters_layout);
    printf ("SSGetProgram: %d\n", program == src.program);    
      
    for (unsigned int i=0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      printf ("SSGetConstantBuffer(%u): %d\n", i, buffers [i] == src.buffers [i]);      
  }
};

void print (const char* message)
{
}

void on_application_initialized ()
{
  try
  {
    Test test;

    ShaderDesc shader_descs [] = {
      {"ffp_shader", ~0u, "", "ffp", ""},
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

    test.device->GetImmediateContext ()->SSSetProgram (shader.get ());
    test.device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout.get ());

    for (unsigned int i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      test.device->GetImmediateContext ()->SSSetConstantBuffer (i, buffer.get ());

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;

    mask.Set (StateBlockGroup_ShaderStage);

    for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      mask.ss_samplers [i] = false;
      mask.ss_textures [i] = false;
    }

    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);

    state_block->Capture (test.device->GetImmediateContext ());

    test.device->GetImmediateContext ()->SSSetProgram (0);
    test.device->GetImmediateContext ()->SSSetProgramParametersLayout (0);

    for (unsigned int i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      test.device->GetImmediateContext ()->SSSetConstantBuffer (i, 0);

    printf ("after reset\n");

    State dst_state;

    dst_state.Init (*test.device);
    dst_state.Check (src_state);

    state_block->Apply (test.device->GetImmediateContext ());

    printf ("after apply\n");

    dst_state.Init (*test.device);
    dst_state.Check (src_state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of unit_shader_stage_state_block_test:\n");
  
  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
