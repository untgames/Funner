#include "shared.h"

using namespace render::low_level;

namespace
{

const char* SHADER_FILE_NAME = "media/fpp_shader.wxf";

struct Vec3f
{
  float x, y, z;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Color4ub color;
};

struct ShaderParameters
{
  math::mat4f projection_matrix;
  math::mat4f view_matrix;  
  math::mat4f object_matrix;
};

//тестовое игровое отображение
class TestView: public IGameView
{
  public:
    TestView () : current_device (0) {}
    
    ~TestView ()
    {
      FlushResources ();
    }
  
    void OnDraw ()
    {
      if (!current_device)
        return;

      UpdateShaderParameters ();        
      
      current_device->SSSetProgram                 (shader.get ());
      current_device->SSSetProgramParametersLayout (shader_parameters_layout.get ());
      current_device->SSSetConstantBuffer          (0, constant_buffer.get ());
      current_device->ISSetInputLayout             (input_layout.get ());
      current_device->ISSetVertexBuffer            (0, vertex_buffer.get ());      

      current_device->Draw (PrimitiveType_TriangleList, 0, 3);
    }

    void LoadResources (IDevice& device)
    {
      current_device = &device;
      
      static const size_t VERTICES_COUNT = 3;
      
      BufferDesc vb_desc;
      
      memset (&vb_desc, 0, sizeof vb_desc);
      
      vb_desc.size         = sizeof MyVertex * VERTICES_COUNT;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
      
      vertex_buffer = BufferPtr (current_device->CreateBuffer (vb_desc), false);
      
      static const MyVertex verts [] = {
        {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
        {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
      };
      
      vertex_buffer->SetData (0, vb_desc.size, verts);      
      
      VertexAttribute attributes [] = {
        {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
        {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
        {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_Float, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
      };
      
      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UInt;
      layout_desc.index_buffer_offset     = 0;

      input_layout = InputLayoutPtr (current_device->CreateInputLayout (layout_desc), false);
      
      stl::string shader_source = load_text_file (SHADER_FILE_NAME);
      
      ShaderDesc shader_descs [] = {
        {"fpp_shader", size_t (-1), shader_source.c_str (), "fpp", ""},
      };

      static ProgramParameter shader_parameters[] = {
        {"myProjMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, projection_matrix)},
        {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, view_matrix)},
        {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, object_matrix)},
      };      

      ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

      shader = ProgramPtr (current_device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &PrintShaderError));

      shader_parameters_layout = ProgramParametersLayoutPtr (current_device->CreateProgramParametersLayout (program_parameters_layout_desc));
      
      BufferDesc cb_desc;
      
      memset (&cb_desc, 0, sizeof cb_desc);
      
      cb_desc.size         = sizeof ShaderParameters;
      cb_desc.usage_mode   = UsageMode_Default;
      cb_desc.bind_flags   = BindFlag_ConstantBuffer;
      cb_desc.access_flags = AccessFlag_ReadWrite;

      constant_buffer = BufferPtr (current_device->CreateBuffer (cb_desc), false);      
    }
    
    void FlushResources ()
    {     
      current_device           = 0;
      vertex_buffer            = 0;
      constant_buffer          = 0;
      input_layout             = 0;
      shader                   = 0;
      shader_parameters_layout = 0;
    }    

    void OnMouse (syslib::WindowEvent event, int x, int y)
    {
//      printf ("Event %d x=%d y=%d\n", event, x, y);
    }
    
  private:
    static void PrintShaderError (const char* message)
    {
      MyApplication::Instance ().LogFormatMessage ("Error in shader '%s'", message);
    }
  
    void UpdateShaderParameters ()
    {
      ShaderParameters shader_parameters;
      
      memset (&shader_parameters, 0, sizeof shader_parameters);

      shader_parameters.projection_matrix = transpose (get_ortho_proj (-2, 2, -2, 2, -1, 1)); //транспонирование не нужно!!!
      shader_parameters.view_matrix   = math::mat4f (1.0f);
      shader_parameters.object_matrix = math::mat4f (1.0f);

      printf ("!!!\n");

      constant_buffer->SetData (0, sizeof shader_parameters, &shader_parameters);
    }

  private:
    IDevice*                   current_device;
    BufferPtr                  vertex_buffer;
    BufferPtr                  constant_buffer;
    InputLayoutPtr             input_layout;
    ProgramPtr                 shader;
    ProgramParametersLayoutPtr shader_parameters_layout;
};

}

//создание тестового игрового отображения
GameView create_test_game_view ()
{
  return GameView (new TestView);
}
