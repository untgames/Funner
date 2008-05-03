#include "shared.h"

const char* SHADER_FILE_NAME = "data/fpp_shader.wxf";

struct MyShaderParameters
{
  float    quadratic_attenuation;
  Matrix4f view_matrix, object_matrix, texgen;
};

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

int main ()
{
  printf ("Results of shader_fpp_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (shader_fpp)");

    stl::string shader_source = read_shader (SHADER_FILE_NAME);

    ShaderDesc shader_desc = {"my_shader", size_t (-1), shader_source.c_str (), "fpp", ""};

    ProgramPtr program (test.device->CreateProgram (1, &shader_desc, &print));
    
    static ProgramParameter shader_parameters [] = {
      {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, view_matrix)},
      {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, object_matrix)},
      {"myTexgen", ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, texgen)},
      {"myQuadraticAttenuation", ProgramParameterType_Float, 0, 1, offsetof (MyShaderParameters, quadratic_attenuation)}
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};
    ProgramParametersLayoutPtr program_parameters_layout (test.device->CreateProgramParametersLayout (program_parameters_layout_desc));    
    
    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;    
    
    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);
    
    MyShaderParameters my_shader_parameters = {
      0.3f
    };
    
    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
    
    test.device->SSSetProgram (program.get ());
    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());
    
    test.device->Draw (PrimitiveType_TriangleList, 0, 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
