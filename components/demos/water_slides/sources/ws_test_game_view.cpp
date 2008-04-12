#include "shared.h"

#pragma pack(1)

using namespace render::low_level;

namespace
{

/*
     онстанты
*/

const char*  WATER_SHADER_FILE_NAME  = "media/water_shader.wxf";  //им€ файла с шейдером воды
const char*  GROUND_SHADER_FILE_NAME = "media/ground_shader.wxf"; //им€ файла с шейдером земли
const char*  WATER_TEXTURE_NAME      = "media/sky.jpg";           //им€ файла с текстурой воды
const char*  GROUND_TEXTURE_NAME     = "media/bottom.jpg";        //им€ файла с текстурой земли
const size_t GRID_SIZE               = 64;                       //количество разбиений сетки с водой
const float  WATER_UPDATE_TIME       = 0.01f;                     //период обновлени€ воды

/*
    ќписание отрисовываемых вершин
*/

struct Vec3f
{
  float x, y, z;
};

struct Vec2f
{
  float x, y;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct Vertex
{
  Vec3f position;
  Vec3f normal;
  Vec2f texcoord;
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
    TestView () : update_timer (xtl::bind (&TestView::OnTime, this), 10),
      water_gen (false), current_device (0), prev_field (&water_field [0]), next_field (&water_field [1]),
      listener (scene_graph::Listener::Create ()),
      sound_emitter (scene_graph::SoundEmitter::Create ("drop"))
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
          vert.texcoord.x  = float (j) / float (GRID_SIZE - 1);
          vert.texcoord.y  = float (i) / float (GRID_SIZE - 1);
          vert.color.red   = vert.color.green = vert.color.blue = 255;
          vert.color.alpha = 255;
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

      listener->BindToParent (scene.Root ());
      sound_emitter->BindToParent (scene.Root ());
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
      
      DepthStencilStatePtr default_depth_stencil_state = current_device->OSGetDepthStencilState ();      
      BlendStatePtr        default_blend_state         = current_device->OSGetBlendState ();      

      current_device->RSSetState                   (rasterizer.get ());            
      current_device->SSSetProgramParametersLayout (shader_parameters_layout.get ());      
      current_device->SSSetConstantBuffer          (0, constant_buffer.get ());
      current_device->SSSetSampler                 (0, texture_sampler.get ());
      current_device->ISSetInputLayout             (input_layout.get ());
      current_device->OSSetDepthStencilState       (0);

      current_device->SSSetProgram      (ground_shader.get ());
      current_device->SSSetTexture      (0, ground_texture.get ());
      current_device->ISSetVertexBuffer (0, rect_vertex_buffer.get ());
      current_device->Draw              (PrimitiveType_TriangleStrip, 0, 4);

      current_device->SSSetProgram      (water_shader.get ());
      current_device->OSSetBlendState   (blend_state.get ());
      current_device->SSSetTexture      (0, water_texture.get ());
      current_device->ISSetVertexBuffer (0, vertex_buffer.get ());
      current_device->ISSetIndexBuffer  (index_buffer.get ());

      for (size_t i=0; i<GRID_SIZE-2; i++)
        current_device->DrawIndexed (PrimitiveType_TriangleStrip, i * indices_block_size, indices_block_size, 0);

      current_device->OSSetBlendState (default_blend_state.get ());
      current_device->OSSetDepthStencilState (default_depth_stencil_state.get ());
    }

    void LoadResources (sound::SGPlayer* player, IDevice& device)
    {
      if (player)
        player->SetListener (*(listener.get ()));

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

      static VertexAttribute attributes [] = {
        {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (Vertex, normal), sizeof (Vertex)},
        {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (Vertex, position), sizeof (Vertex)},
        {VertexAttributeSemantic_TexCoord0, InputDataFormat_Vector2, InputDataType_Float, 0, offsetof (Vertex, texcoord), sizeof (Vertex)},
        {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_UByte, 0, offsetof (Vertex, color), sizeof (Vertex)},
      };
      
      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UInt;
      layout_desc.index_buffer_offset     = 0;            

      input_layout = InputLayoutPtr (current_device->CreateInputLayout (layout_desc), false);      
      
      static ProgramParameter shader_parameters[] = {
        {"myProjMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, projection_matrix)},
        {"myViewMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, view_matrix)},
        {"myObjectMatrix", ProgramParameterType_Float4x4, 0, 1, offsetof (ShaderParameters, object_matrix)},
      };      

      ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

      water_shader  = LoadShader (WATER_SHADER_FILE_NAME);
      ground_shader = LoadShader (GROUND_SHADER_FILE_NAME);      

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

      rs_desc.fill_mode  = FillMode_Solid;
      rs_desc.cull_mode  = CullMode_None;
      rs_desc.depth_bias = 1;

      rasterizer = RasterizerStatePtr (current_device->CreateRasterizerState (rs_desc), false);      

      water_texture  = LoadTexture (WATER_TEXTURE_NAME);
      ground_texture = LoadTexture (GROUND_TEXTURE_NAME);

      SamplerDesc sampler_desc;
      
      memset (&sampler_desc, 0, sizeof (sampler_desc));

      sampler_desc.min_filter = TexMinFilter_LinearMipLinear;
      sampler_desc.mag_filter = TexMagFilter_Linear;
      sampler_desc.wrap_u     = TexcoordWrap_Clamp;
      sampler_desc.wrap_v     = TexcoordWrap_Clamp;
      sampler_desc.wrap_w     = TexcoordWrap_Clamp;

      texture_sampler = SamplerStatePtr (current_device->CreateSamplerState (sampler_desc), false);
      
      static Vertex rect_vertices [] = {
        {{-1, -1, 0}, {0, 0, -1}, {0, 0}, {255, 255, 255, 255}},
        {{ 1, -1, 0}, {0, 0, -1}, {1, 0}, {255, 255, 255, 255}},
        {{-1,  1, 0}, {0, 0, -1}, {0, 1}, {255, 255, 255, 255}},
        {{ 1,  1, 0}, {0, 0, -1}, {1, 1}, {255, 255, 255, 255}},
      };

      vb_desc.size = sizeof (rect_vertices);

      rect_vertex_buffer = BufferPtr (current_device->CreateBuffer (vb_desc), false);

      rect_vertex_buffer->SetData (0, sizeof rect_vertices, rect_vertices);

      BlendDesc blend_desc;
      
      memset (&blend_desc, 0, sizeof (blend_desc));

      blend_desc.blend_enable                     = true;
      blend_desc.blend_color_operation            = BlendOperation_Add;
      blend_desc.blend_color_source_argument      = BlendArgument_SourceAlpha;
      blend_desc.blend_color_destination_argument = BlendArgument_InverseSourceAlpha;
      blend_desc.blend_alpha_operation            = BlendOperation_Add;
      blend_desc.blend_alpha_source_argument      = BlendArgument_SourceAlpha;
      blend_desc.blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;
      blend_desc.color_write_mask                 = ColorWriteFlag_All;

      blend_state = BlendStatePtr (current_device->CreateBlendState (blend_desc), false);            

      UpdateWater ();
    }
    
    void FlushResources ()
    {     
      current_device           = 0;
      vertex_buffer            = 0;
      rect_vertex_buffer       = 0;
      index_buffer             = 0;
      constant_buffer          = 0;
      input_layout             = 0;
      water_shader             = 0;
      ground_shader            = 0;
      shader_parameters_layout = 0;
      rasterizer               = 0;
      water_texture            = 0;
      texture_sampler          = 0;
      blend_state              = 0;
    }    
    
    void OnTime ()
    {
      static size_t last = MyApplication::Milliseconds ();
      
      float dt = (MyApplication::Milliseconds () - last) / 1000.0f;
      
//      if (dt > WATER_UPDATE_TIME)
      {
        UpdateWater ();

        last = MyApplication::Milliseconds ();
      }      
    }
    
    void OnIdle ()
    {
    }

    void OnMouse (syslib::WindowEvent event, int x, int y)
    {
      switch (event)
      {
        case syslib::WindowEvent_OnLeftButtonUp:
          water_gen = false;
          break; 
        case syslib::WindowEvent_OnLeftButtonDown:
          water_gen = true;

          sound_emitter->Stop ();
          sound_emitter->SetPosition ((float)x - 50.f, 50.f - y, 0.f);
          sound_emitter->Play ();
        case syslib::WindowEvent_OnMouseMove:
        {
          if (water_gen)
          {
            int i1 = int (float (100-x) / 100.0f * (GRID_SIZE - 5)),
                j1 = int (float (y) / 100.0f * (GRID_SIZE - 5));

            for (int i=-3; i<4; i++)
            {
              for (int j=-3; j<4; j++)
              {
                float v = 6.0f - i * i - j * j;

                if (v < 0.0f)
                  v = 0.0f;

                next_field->U [i+i1+3][j+j1+3] -= v * 0.004f;
              }
            }            
          }          
                  
          break;
        }
        default:
          break;
      }      
    }
    
  private:
    ProgramPtr LoadShader (const char* name)
    {
      stl::string shader_source = load_text_file (name);
      
      ShaderDesc shader_desc = {name, size_t (-1), shader_source.c_str (), "fpp", ""};

      return ProgramPtr (current_device->CreateProgram (1, &shader_desc, &PrintShaderError));
    }    
  
    TexturePtr LoadTexture (const char* name)
    {
      media::Image water_image (name, media::PixelFormat_RGB8);
      
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

      TexturePtr texture (current_device->CreateTexture (tex_desc), false);

      texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, PixelFormat_RGB8, water_image.Bitmap ());      

      return texture;
    }
  
    static void PrintShaderError (const char* message)
    {
      MyApplication::Instance ().LogFormatMessage ("%s", message);
    }
  
    void UpdateShaderParameters ()
    {
      ShaderParameters shader_parameters;

      memset (&shader_parameters, 0, sizeof shader_parameters);

      shader_parameters.object_matrix     = math::mat4f (1.0f);      
      shader_parameters.projection_matrix = get_ortho_proj (-1, 1, -1, 1, -10, 10);
      shader_parameters.view_matrix       = math::translatef (0, 0, -5.5);

      constant_buffer->SetData (0, sizeof shader_parameters, &shader_parameters);
    }
    
    void UpdateWater ()
    {
      int i, j;

      for (i=1; i<GRID_SIZE-1; i++)
      {
        for (j=1; j<GRID_SIZE-1; j++)
        {
          Vertex& v = vertices [i][j];

          v.position.z = next_field->U [i][j];                      
          v.normal.x   = next_field->U [i-1][j] - next_field->U [i+1][j];
          v.normal.y   = next_field->U [i][j-1] - next_field->U [i][j+1];

//          printf ("%g ", fabs (255.0f * v.normal.z));

//          v.color.alpha = unsigned char (200.0f - next_field->U [i][j] * 150.0f);
          
//          static const float vis = 0.005f;
          static const float vis = 0.02f;

          float laplas = (next_field->U [i-1][j] +
                          next_field->U [i+1][j] +
                          next_field->U [i][j+1] +
                          next_field->U [i][j-1]) * 0.25f - next_field->U [i][j];

          prev_field->U [i][j] = ((2.0f - vis) * next_field->U [i][j] - prev_field->U [i][j] * (1.0f - vis) + laplas);
        }
      }

      stl::swap (next_field, prev_field);

      vertex_buffer->SetData (0, GRID_SIZE * GRID_SIZE * sizeof Vertex, vertices);
    }
    
  private:
    typedef stl::vector<size_t> IndexArray;

  private:
    syslib::Timer              update_timer;
    bool                       water_gen;
    WaterField                 water_field [2];
    WaterField                 *prev_field, *next_field;
    Vertex                     vertices [GRID_SIZE][GRID_SIZE];
    IndexArray                 indices;
    size_t                     indices_block_size;
    IDevice*                   current_device;
    BufferPtr                  vertex_buffer;
    BufferPtr                  rect_vertex_buffer;
    BufferPtr                  constant_buffer;
    BufferPtr                  index_buffer;
    InputLayoutPtr             input_layout;
    ProgramPtr                 water_shader;
    ProgramPtr                 ground_shader;    
    ProgramParametersLayoutPtr shader_parameters_layout;
    RasterizerStatePtr         rasterizer;
    TexturePtr                 water_texture;
    TexturePtr                 ground_texture;
    SamplerStatePtr            texture_sampler;
    BlendStatePtr              blend_state;
    scene_graph::Scene         scene;
    ListenerPtr                listener;
    SoundEmitterPtr            sound_emitter;
};

}

//создание тестового игрового отображени€
GameView create_test_game_view ()
{
  return GameView (new TestView);
}
