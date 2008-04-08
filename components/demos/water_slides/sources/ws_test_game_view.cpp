#include "shared.h"

using namespace render::low_level;

namespace
{

/*
     онстанты
*/

const char*  SHADER_FILE_NAME   = "media/fpp_shader.wxf"; //им€ файла с шейдером
const char*  WATER_TEXTURE_NAME = "media/phong.jpg";      //им€ файла с текстурой воды
const size_t GRID_SIZE          = 128;                    //количество разбиений сетки с водой
const float  WATER_UPDATE_TIME  = 0.01f;                  //период обновлени€ воды

/*
    ќписание отрисовываемых вершин
*/

struct Vec3f
{
  float x, y, z;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct Vertex
{
  Vec3f    position;
  Vec3f    normal;
  Color4ub color;
};

/*
    ѕоле скоростей воды
*/

struct WaterField
{
  float U [GRID_SIZE][GRID_SIZE];
};

/*
    ѕараметры шейдера
*/

struct ShaderParameters
{
  math::mat4f projection_matrix;
  math::mat4f view_matrix;  
  math::mat4f object_matrix;
};

/*
    “естовое игровое отображение
*/

class TestView: public IGameView
{
  public:
    TestView () : current_device (0), prev_field (&water_field [0]), next_field (&water_field [1])
    {
      memset (vertices, 0, sizeof vertices);
      memset (water_field, 0, sizeof water_field);

      for (size_t i=0; i<GRID_SIZE; i++)
      {
        for (size_t j=0; j<GRID_SIZE; j++)
        {
          Vertex& vert = vertices [i][j];
          
          vert.position.x  = 1.0f - 2.0f * i / (GRID_SIZE - 1);
          vert.position.y  = 1.0f - 2.0f * j / (GRID_SIZE - 1);
          vert.normal.z    = -4.0f / (GRID_SIZE - 1);
          vert.color.red   = 0;
          vert.color.green = 0;
          vert.color.blue  = 0;
          vert.color.alpha = 0;          
        }
      }
      
      indices.reserve (GRID_SIZE * GRID_SIZE * 2);

      indices_block_size = (GRID_SIZE - 2) * 2;

      for (size_t i=1; i<GRID_SIZE-1; i++)
      {
        for (size_t j=1; j<GRID_SIZE-1; j++)
        {
          indices.push_back (&vertices [i][j]   - &vertices [0][0]);
          indices.push_back (&vertices [i+1][j] - &vertices [0][0]);
        }        
      }      
    }
    
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
      current_device->ISSetIndexBuffer             (index_buffer.get ());      
      current_device->RSSetState                   (rasterizer.get ());
      current_device->SSSetSampler                 (0, texture_sampler.get ());
      current_device->SSSetTexture                 (0, water_texture.get ());

      for (size_t i=0; i<GRID_SIZE-2; i++)
        current_device->DrawIndexed (PrimitiveType_TriangleStrip, i * indices_block_size, indices_block_size, 0);
    }

    void LoadResources (IDevice& device)
    {
      current_device = &device;
      
      BufferDesc vb_desc;
      
      memset (&vb_desc, 0, sizeof vb_desc);
      
      vb_desc.size         = GRID_SIZE * GRID_SIZE * sizeof Vertex;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
      
      vertex_buffer = BufferPtr (current_device->CreateBuffer (vb_desc), false);
      
      BufferDesc ib_desc;
      
      memset (&ib_desc, 0, sizeof ib_desc);
      
      ib_desc.size         = indices.size () * sizeof size_t;
      ib_desc.usage_mode   = UsageMode_Default;
      ib_desc.bind_flags   = BindFlag_IndexBuffer;
      ib_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      index_buffer = BufferPtr (current_device->CreateBuffer (ib_desc), false);

      index_buffer->SetData (0, indices.size () * sizeof size_t, &indices [0]);

      VertexAttribute attributes [] = {
        {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (Vertex, normal), sizeof (Vertex)},
        {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (Vertex, position), sizeof (Vertex)},
        {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_UByte, 0, offsetof (Vertex, color), sizeof (Vertex)},
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
      
      RasterizerDesc rs_desc;

      memset (&rs_desc, 0, sizeof (rs_desc));

      rs_desc.fill_mode               = FillMode_Solid;
      rs_desc.cull_mode               = CullMode_Back;
      rs_desc.front_counter_clockwise = false;
      rs_desc.depth_bias              = 1;
      rs_desc.scissor_enable          = true;
      rs_desc.multisample_enable      = true;
      rs_desc.antialiased_line_enable = false;

      rasterizer = RasterizerStatePtr (current_device->CreateRasterizerState (rs_desc), false);
      
      media::Image water_image (WATER_TEXTURE_NAME, media::PixelFormat_RGB8);
      
      TextureDesc tex_desc;

      memset (&tex_desc, 0, sizeof (tex_desc));      
      
      tex_desc.dimension            = TextureDimension_2D;
      tex_desc.width                = water_image.Width ();
      tex_desc.height               = water_image.Height ();
      tex_desc.layers               = 1;
      tex_desc.format               = PixelFormat_RGB8;
      tex_desc.bind_flags           = BindFlag_Texture;
      tex_desc.generate_mips_enable = true;
      tex_desc.access_flags         = AccessFlag_ReadWrite;

      water_texture = TexturePtr (current_device->CreateTexture (tex_desc), false);      

      water_texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, PixelFormat_RGB8, water_image.Bitmap ());
      
      SamplerDesc sampler_desc;
      
      memset (&sampler_desc, 0, sizeof (sampler_desc));

      sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
      sampler_desc.mag_filter           = TexMagFilter_Linear;
      sampler_desc.wrap_u               = TexcoordWrap_Clamp;
      sampler_desc.wrap_v               = TexcoordWrap_Clamp;
      sampler_desc.wrap_w               = TexcoordWrap_Clamp;
//      sampler_desc.max_anisotropy       = 16;      
//      sampler_desc.comparision_function = CompareMode_Less;
//      sampler_desc.mip_lod_bias         = 0.2f;
//      sampler_desc.min_lod              = 1.f;
//      sampler_desc.max_lod              = 2.f;

      texture_sampler = SamplerStatePtr (current_device->CreateSamplerState (sampler_desc), false);

      UpdateWater ();
    }
    
    void FlushResources ()
    {     
      current_device           = 0;
      vertex_buffer            = 0;
      index_buffer             = 0;
      constant_buffer          = 0;
      input_layout             = 0;
      shader                   = 0;
      shader_parameters_layout = 0;
      rasterizer               = 0;
      water_texture            = 0;
      texture_sampler          = 0;
    }    
    
    void OnIdle ()
    {
      static size_t last = MyApplication::Milliseconds ();
      
      float dt = (MyApplication::Milliseconds () - last) / 1000.0f;
      
      if (dt > WATER_UPDATE_TIME)
      {
        UpdateWater ();

        last = MyApplication::Milliseconds ();
      }
    }

    void OnMouse (syslib::WindowEvent event, int x, int y)
    {
//      printf ("Event %d x=%d y=%d\n", event, x, y);
    }
    
  private:
    static void PrintShaderError (const char* message)
    {
      MyApplication::Instance ().LogFormatMessage ("%s", message);
    }
  
    void UpdateShaderParameters ()
    {
      ShaderParameters shader_parameters;
      
      memset (&shader_parameters, 0, sizeof shader_parameters);

      shader_parameters.projection_matrix = transpose (get_ortho_proj (-2, 2, -2, 2, -1, 1)); //транспонирование не нужно!!!
//      shader_parameters.view_matrix   = math::mat4f (1.0f);
      shader_parameters.view_matrix   = math::rotatef (math::deg2rad (45.0f), 1, 0, 0);
      shader_parameters.object_matrix = math::mat4f (1.0f);

      constant_buffer->SetData (0, sizeof shader_parameters, &shader_parameters);
    }
    
    void UpdateWater ()
    {
      int i, j, i1, j1;

      i1 = rand () % (GRID_SIZE - 10);
      j1 = rand () % (GRID_SIZE - 10);

      /*1*/
      
      if (!(rand()&31))
        for(i=-3; i<4; i++)
        {
          for (j=-3; j<4; j++)
          {
            float v = 6.0f - i * i - j * j;

            if (v < 0.0f)
              v = 0.0f;

            next_field->U [i+i1+3][j+j1+3] -= v * 0.004f;
          }
        }

      for (i=1; i<GRID_SIZE-1; i++)
      {
        for (j=1; j<GRID_SIZE-1; j++)
        {
          /*2*/

          vertices [i][j].position.z = next_field->U [i][j];
          vertices [i][j].normal.x   = next_field->U [i-1][j] - next_field->U [i+1][j];
          vertices [i][j].normal.y   = next_field->U [i][j-1] - next_field->U [i][j+1];
          
          vertices [i][j].color.blue = unsigned char (255.0f * next_field->U [i][j]);

          /*3*/
          
          static const float vis = 0.005f;

          float laplas = (next_field->U [i-1][j] +
                          next_field->U [i+1][j] +
                          next_field->U [i][j+1] +
                          next_field->U [i][j-1]) * 0.25f - next_field->U [i][j];

          /*4*/

          prev_field->U [i][j] = ((2.0f - vis) * next_field->U [i][j] - prev_field->U [i][j] * (1.0f - vis) + laplas);
        }
      }

      /*5*/

      stl::swap (next_field, prev_field);
      
      /*6*/

      vertex_buffer->SetData (0, GRID_SIZE * GRID_SIZE * sizeof Vertex, vertices);
    }
    
  private:
    typedef stl::vector<size_t> IndexArray;

  private:
    WaterField                 water_field [2];
    WaterField                 *prev_field, *next_field;
    Vertex                     vertices [GRID_SIZE][GRID_SIZE];
    IndexArray                 indices;
    size_t                     indices_block_size;
    IDevice*                   current_device;
    BufferPtr                  vertex_buffer;
    BufferPtr                  constant_buffer;
    BufferPtr                  index_buffer;
    InputLayoutPtr             input_layout;
    ProgramPtr                 shader;
    ProgramParametersLayoutPtr shader_parameters_layout;
    RasterizerStatePtr         rasterizer;
    TexturePtr                 water_texture;
    SamplerStatePtr            texture_sampler;
};

}

//создание тестового игрового отображени€
GameView create_test_game_view ()
{
  return GameView (new TestView);
}
