#include "shared.h"

using namespace render::low_level;

namespace
{

/*
    Константы
*/

const char*  WATER_SHADER_FILE_NAME  = "media/water_shader.wxf";  //имя файла с шейдером воды
const char*  GROUND_SHADER_FILE_NAME = "media/ground_shader.wxf"; //имя файла с шейдером земли
const char*  WATER_TEXTURE_NAME      = "media/sky.jpg";           //имя файла с текстурой воды
const char*  GROUND_TEXTURE_NAME     = "media/bottom.jpg";        //имя файла с текстурой земли
const char*  BOAT_TEXTURE_NAME       = "media/boat.tif";          //имя файла с лодкой
const size_t GRID_SIZE               = 64;                        //количество разбиений сетки с водой
const float  WATER_UPDATE_TIME       = 0.01f;                     //период обновления воды
const float  SCALE_FACTOR            = 1.1f;                      //коэффициент растяжения сетки

/*
    Описание отрисовываемых вершин
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
    Поле скоростей воды
*/

struct WaterField
{
  float U [GRID_SIZE][GRID_SIZE];
};

/*
    Параметры шейдера
*/

struct ShaderParameters
{
  math::mat4f projection_matrix;
  math::mat4f view_matrix;  
  math::mat4f object_matrix;
};

/*
    Тестовое игровое отображение
*/

class TestView: public IGameView
{
  public:
    TestView () : update_timer (xtl::bind (&TestView::OnTime, this), 10),
      water_gen (false), 
      prev_field (&water_field [0]),
      next_field (&water_field [1]),
      current_device (0),
      listener (scene_graph::Listener::Create ()),
      sound_emitter (scene_graph::SoundEmitter::Create ("drop")),
      boat_x (50),
      boat_y (50)
    {
      memset (vertices, 0, sizeof vertices);
      memset (water_field, 0, sizeof water_field);

      for (size_t i=0; i<GRID_SIZE; i++)
      {
        for (size_t j=0; j<GRID_SIZE; j++)
        {
          Vertex& vert = vertices [i][j];
          
          vert.position.x  = SCALE_FACTOR - 2 * SCALE_FACTOR * i / float (GRID_SIZE - 1);
          vert.position.y  = SCALE_FACTOR - 2 * SCALE_FACTOR * j / float (GRID_SIZE - 1);
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

//      listener->SetOrientation (180.f, 0.f, 0.f, 1.f);

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
      current_device->ISSetIndexBuffer  (rect_index_buffer.get ());
      current_device->DrawIndexed       (PrimitiveType_TriangleList, 0, 6, 0);

      current_device->SSSetProgram      (water_shader.get ());
      current_device->OSSetBlendState   (blend_state.get ());
      current_device->SSSetTexture      (0, water_texture.get ());
      current_device->ISSetVertexBuffer (0, vertex_buffer.get ());
      current_device->ISSetIndexBuffer  (index_buffer.get ());

      for (size_t i=0; i<GRID_SIZE-2; i++)
        current_device->DrawIndexed (PrimitiveType_TriangleStrip, i * indices_block_size, indices_block_size, 0);

      current_device->ISSetVertexBuffer   (0, rect_vertex_buffer.get ());
      current_device->ISSetIndexBuffer    (rect_index_buffer.get ());
      current_device->SSSetProgram        (ground_shader.get ());
      current_device->SSSetConstantBuffer (0, boat_constant_buffer.get ());
      current_device->SSSetTexture        (0, boat_texture.get ());
//      current_device->DrawIndexed         (PrimitiveType_TriangleList, 0, 6, 0);

      current_device->OSSetBlendState (default_blend_state.get ());
      current_device->OSSetDepthStencilState (default_depth_stencil_state.get ());      
    }

    void LoadResources (sound::ScenePlayer* player, IDevice& device)
    {
      if (player)
        player->SetListener (&*(listener.get ()));        

      current_device = &device;
      
      BufferDesc vb_desc;
      
      memset (&vb_desc, 0, sizeof vb_desc);
      
      vb_desc.size         = GRID_SIZE * GRID_SIZE * sizeof (Vertex);
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
     
      vertex_buffer = BufferPtr (current_device->CreateBuffer (vb_desc), false);
      
      BufferDesc ib_desc;
      
      memset (&ib_desc, 0, sizeof ib_desc);
      
      ib_desc.size         = indices.size () * sizeof (size_t);
      ib_desc.usage_mode   = UsageMode_Default;
      ib_desc.bind_flags   = BindFlag_IndexBuffer;
      ib_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      index_buffer = BufferPtr (current_device->CreateBuffer (ib_desc), false);

      index_buffer->SetData (0, indices.size () * sizeof (size_t), &indices [0]);
      
      static size_t rect_indices [] = {0, 1, 2, 3, 0, 2};
      
      ib_desc.size = sizeof rect_indices;
      
      rect_index_buffer = BufferPtr (current_device->CreateBuffer (ib_desc), false);

      rect_index_buffer->SetData (0, sizeof rect_indices, rect_indices);

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
      
      cb_desc.size         = sizeof (ShaderParameters);
      cb_desc.usage_mode   = UsageMode_Default;
      cb_desc.bind_flags   = BindFlag_ConstantBuffer;
      cb_desc.access_flags = AccessFlag_ReadWrite;

      constant_buffer = BufferPtr (current_device->CreateBuffer (cb_desc), false);
      boat_constant_buffer = BufferPtr (current_device->CreateBuffer (cb_desc), false);
      
      RasterizerDesc rs_desc;

      memset (&rs_desc, 0, sizeof (rs_desc));

      rs_desc.fill_mode  = FillMode_Solid;
      rs_desc.cull_mode  = CullMode_None;      

      rasterizer = RasterizerStatePtr (current_device->CreateRasterizerState (rs_desc), false);            

      water_texture  = LoadTexture (WATER_TEXTURE_NAME);
      ground_texture = LoadTexture (GROUND_TEXTURE_NAME);
      boat_texture   = LoadTexture (BOAT_TEXTURE_NAME);
      
      SamplerDesc sampler_desc;
      
      memset (&sampler_desc, 0, sizeof (sampler_desc));

      sampler_desc.min_filter = TexMinFilter_Linear;
      sampler_desc.mag_filter = TexMagFilter_Linear;
      sampler_desc.max_anisotropy = 1;
      sampler_desc.wrap_u     = TexcoordWrap_Repeat;
      sampler_desc.wrap_v     = TexcoordWrap_Repeat;
      sampler_desc.comparision_function = CompareMode_AlwaysPass;
      sampler_desc.min_lod    = 0;
      sampler_desc.max_lod    = FLT_MAX;

      texture_sampler = SamplerStatePtr (current_device->CreateSamplerState (sampler_desc), false);
      
      static Vertex rect_vertices [] = {
        {{-1, -1, 0}, {0, 0, -1}, {0, 0}, {255, 255, 255, 255}},
        {{ 1, -1, 0}, {0, 0, -1}, {1, 0}, {255, 255, 255, 255}},
        {{ 1,  1, 0}, {0, 0, -1}, {1, 1}, {255, 255, 255, 255}},
        {{-1,  1, 0}, {0, 0, -1}, {0, 1}, {255, 255, 255, 255}},
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
      boat_constant_buffer     = 0;
      input_layout             = 0;
      water_shader             = 0;
      ground_shader            = 0;
      shader_parameters_layout = 0;
      rasterizer               = 0;
      water_texture            = 0;
      boat_texture             = 0;
      texture_sampler          = 0;
      blend_state              = 0;
    }    
    
    void OnTime ()
    {
      static size_t last = MyApplication::Milliseconds ();
      
//      float dt = (MyApplication::Milliseconds () - last) / 1000.0f;
      
//      if (dt > WATER_UPDATE_TIME)
      {
        UpdateWater ();

        last = MyApplication::Milliseconds ();
      }      
    }
    
    void OnIdle ()
    {
    }
    
    float TransformMouseCoord (float x)
    {
      return (2.0f * x + SCALE_FACTOR - 1.0f) / 2.0f / SCALE_FACTOR;
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
          
          boat_x = x;
          boat_y = 100 - y;

          sound_emitter->SetPosition ((float)x - 50.f, 0.f, 0.f);
          sound_emitter->Play ();

        case syslib::WindowEvent_OnMouseMove:
        {
          boat_x = x;
          boat_y = 100 - y;  

          int i1 = int (TransformMouseCoord (float (100 - x) / 100.0f) * (GRID_SIZE - 1)),
              j1 = int (TransformMouseCoord (float (y) / 100.0f) * (GRID_SIZE - 1));

          for (int i=-3; i<4; i++)
          {
            if (i+i1+3 >= (int)GRID_SIZE)
              continue;
            
            for (int j=-3; j<4; j++)
            {
              if (j+j1+3 >= (int)GRID_SIZE)
                continue;

              float v = 6.0f - i * i - j * j;

              if (v < 0.0f)
                v = 0.0f;                                  

              next_field->U [i+i1+3][j+j1+3] -= v * 0.004f;
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
      stl::string shader_source;
      
      common::FileSystem::LoadTextFile (name, shader_source);
      
      ShaderDesc shader_desc = {name, size_t (-1), shader_source.c_str (), "fpp", ""};

      return ProgramPtr (current_device->CreateProgram (1, &shader_desc, &PrintShaderError));
    }    
  
    TexturePtr LoadTexture (const char* name)
    {
      media::Image image (name);
      
      PixelFormat format;
      
      switch (image.Format ())
      {
        case media::PixelFormat_RGB8:
          format = PixelFormat_RGB8;
          break;
        case media::PixelFormat_BGR8:
          image.Convert (media::PixelFormat_RGB8);
        
          format = PixelFormat_RGB8;
          break;        
        case media::PixelFormat_RGBA8:
          format = PixelFormat_RGBA8;
          break;
        case media::PixelFormat_BGRA8:
          image.Convert (media::PixelFormat_RGBA8);
        
          format = PixelFormat_RGBA8;
          break;
        case media::PixelFormat_L8:
          format = PixelFormat_L8;
          break;
        case media::PixelFormat_A8:
          format = PixelFormat_A8;
          break;        
        case media::PixelFormat_LA8:
          format = PixelFormat_LA8;
          break;        
        default:
          throw xtl::format_operation_exception ("TestView::LoadTexture", "Unknown texture format=%d", image.Format ());
          break;
      }
      
      TextureDesc tex_desc;

      memset (&tex_desc, 0, sizeof (tex_desc));      
      
      tex_desc.dimension    = TextureDimension_2D;
      tex_desc.width        = image.Width ();
      tex_desc.height       = image.Height ();
      tex_desc.layers       = 1;
      tex_desc.format       = format;
      tex_desc.bind_flags   = BindFlag_Texture;
      tex_desc.access_flags = AccessFlag_ReadWrite;      

      TexturePtr texture (current_device->CreateTexture (tex_desc), false);            

      texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, format, image.Bitmap ());            

      return texture;
    }
  
    static void PrintShaderError (const char* message)
    {
      printf ("shader error: %s\n", message);
//      MyApplication::Instance ().LogFormatMessage ("%s", message);
    }
  
    void UpdateShaderParameters ()
    {
      ShaderParameters shader_parameters;

      memset (&shader_parameters, 0, sizeof shader_parameters);

      shader_parameters.object_matrix     = 1.0f;
      shader_parameters.projection_matrix = get_ortho_proj (-1, 1, -1, 1, -30, 30);
      shader_parameters.view_matrix       = math::translatef (0, 0, -15.5);

      constant_buffer->SetData (0, sizeof shader_parameters, &shader_parameters);
      
      shader_parameters.object_matrix = math::translatef (float (boat_x) / 50.0f - 1.0f, float (boat_y) / 50.0f - 1.0f, 2.0f) * 
                                        math::scalef (0.1f, 0.1f, 0.1f);

      boat_constant_buffer->SetData (0, sizeof shader_parameters, &shader_parameters);
    }
    
    void UpdateWater ()
    {
      int i, j;

      for (i=1; i<(int)GRID_SIZE-1; i++)
      {
        for (j=1; j<(int)GRID_SIZE-1; j++)
        {
          Vertex& v = vertices [i][j];

          v.position.z = next_field->U [i][j] - 1;
          v.normal.x   = next_field->U [i-1][j] - next_field->U [i+1][j];
          v.normal.y   = next_field->U [i][j-1] - next_field->U [i][j+1];

//          printf ("%g ", fabs (255.0f * v.normal.z));

//          v.color.alpha = unsigned char (200.0f - next_field->U [i][j] * 150.0f);
          
//          static const float vis = 0.01f;
          static const float vis = 0.1f;          
//          static const float vis = 0.02f;

          float laplas = (next_field->U [i-1][j] +
                          next_field->U [i+1][j] +
                          next_field->U [i][j+1] +
                          next_field->U [i][j-1]) * 0.25f - next_field->U [i][j];

          prev_field->U [i][j] = ((2.0f - vis) * next_field->U [i][j] - prev_field->U [i][j] * (1.0f - vis) + laplas);
        }
      }

      stl::swap (next_field, prev_field);

      vertex_buffer->SetData (0, GRID_SIZE * GRID_SIZE * sizeof (Vertex), vertices);
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
    BufferPtr                  boat_constant_buffer;
    BufferPtr                  index_buffer;
    BufferPtr                  rect_index_buffer;
    InputLayoutPtr             input_layout;
    ProgramPtr                 water_shader;
    ProgramPtr                 ground_shader;    
    ProgramParametersLayoutPtr shader_parameters_layout;
    RasterizerStatePtr         rasterizer;
    TexturePtr                 water_texture;
    TexturePtr                 ground_texture;
    TexturePtr                 boat_texture;
    SamplerStatePtr            texture_sampler;
    BlendStatePtr              blend_state;
    scene_graph::Scene         scene;
    ListenerPtr                listener;
    SoundEmitterPtr            sound_emitter;
    int                        boat_x;
    int                        boat_y;
};

}

//создание тестового игрового отображения
GameView create_test_game_view ()
{
  return GameView (new TestView, false);
}
