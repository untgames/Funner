#include "shared.h"

const char* SHADER_FILE_NAME = "data/simple.vsh";

struct MyShaderParameters
{
  Matrix4f mvp_matrix;
};

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

int main ()
{
  printf ("Results of vsh_test:\n");
  
  try
  {
    Test test (L"DX11 device test window (vsh)");

    stl::string shader_source = read_shader (SHADER_FILE_NAME);

    ShaderDesc shader_desc = {SHADER_FILE_NAME, size_t (-1), shader_source.c_str (), "hlsl.vs", ""};

    ProgramPtr program (test.device->CreateProgram (1, &shader_desc, &print));
    
    static ProgramParameter shader_parameters [] = {
      {"WorldViewProj", ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, mvp_matrix)},
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc));    
    
    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof (MyShaderParameters);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;    
    
    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);
    
    MyShaderParameters my_shader_parameters = {1.0f};

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->GetImmediateContext ()->SSSetProgram (program.get ());
//    test.device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->GetImmediateContext ()->SSSetConstantBuffer (0, cb.get ());

    test.device->GetImmediateContext ()->Draw (PrimitiveType_TriangleList, 0, 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
