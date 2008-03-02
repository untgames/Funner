#include "shared.h"

const char* PIXEL_SHADER_FILE_NAME   = "data/glsl/bumpslc.frag";
const char* VERTEX_SHADER_FILE_NAME  = "data/glsl/bumpslc.vert";
const char* PIXEL_SHADER_FILE_NAME2  = "data/glsl/wood.frag";
const char* VERTEX_SHADER_FILE_NAME2 = "data/glsl/wood.vert";

#pragma pack (1)

struct MyShaderParameters2
{
  float        grain_size_recip;
  math::vec3f  dark_color;
  math::vec3f  color_spread;
  math::vec3f  light_position;
  float        scale;
  math::mat4f  transform;
};

struct MyShaderParameters
{
  math::vec3f light_position;
  int         bump_texture_slot;
  int         diffuse_texture_slot;  
  math::mat4f transform;
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
  printf ("Results of shader_stage_state_block_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (shader_stage_state_block)");
    
    static ProgramParameter shader_parameters[] = {
      {"LightPosition", ProgramParameterType_Float3, 3, 1, offsetof (MyShaderParameters, light_position)},
      {"bump_sampler2d", ProgramParameterType_Int, 3, 1, offsetof (MyShaderParameters, bump_texture_slot)},
      {"diffuse_sampler2d", ProgramParameterType_Int, 3, 1, offsetof (MyShaderParameters, diffuse_texture_slot)},      
      {"Transform", ProgramParameterType_Float4x4, 3, 1, offsetof (MyShaderParameters, transform)}
    };

    static ProgramParameter shader_parameters2[] = {
      {"GrainSizeRecip", ProgramParameterType_Float, 3, 1, offsetof (MyShaderParameters2, grain_size_recip)},
      {"DarkColor", ProgramParameterType_Float3, 3, 1, offsetof (MyShaderParameters2, dark_color)},
      {"colorSpread", ProgramParameterType_Float3, 3, 1, offsetof (MyShaderParameters2, color_spread)},
      {"LightPosition", ProgramParameterType_Float3, 3, 1, offsetof (MyShaderParameters2, light_position)},
      {"Scale", ProgramParameterType_Float, 3, 1, offsetof (MyShaderParameters2, scale)},
      {"Transform", ProgramParameterType_Float4x4, 3, 1, offsetof (MyShaderParameters2, transform)}
    };

    stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);
    stl::string pixel_shader_source2  = read_shader (PIXEL_SHADER_FILE_NAME2),
                vertex_shader_source2 = read_shader (VERTEX_SHADER_FILE_NAME2);

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters},
                                alt_program_parameters_layout_desc = {sizeof shader_parameters2 / sizeof *shader_parameters2, shader_parameters2};
    
    ShaderDesc shader_descs [] = {
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""}
    }; 

    ShaderDesc alt_shader_descs [] =  {
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""}
    };

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print)), 
               alt_shader (test.device->CreateProgram (sizeof alt_shader_descs / sizeof *alt_shader_descs, alt_shader_descs, &print));
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc)),
                               alt_program_parameters_layout (test.device->CreateProgramParametersLayout (alt_program_parameters_layout_desc));

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    BufferDesc cb_desc2 (cb_desc);
   
    cb_desc2.size = sizeof MyShaderParameters2;
 
    BufferPtr buffer (test.device->CreateBuffer (cb_desc), false), cb2 (test.device->CreateBuffer (cb_desc2), false);
    
    test.device->SSSetProgram (shader.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (3, buffer.get ());

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
    
    test.device->SSSetProgram (alt_shader.get ());
    test.device->SSSetProgramParametersLayout (alt_program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (3, cb2.get ());

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
