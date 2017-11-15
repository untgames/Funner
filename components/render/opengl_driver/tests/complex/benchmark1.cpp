#include "shared.h"

const size_t       MAX_BENCHMARK_DURATION  = 5000;
const size_t       MAX_BENCHMARK_FRAMES    = 5000;
const unsigned int COLOR_BITS_TO_TEST []   = { 32 };
const unsigned int DEPTH_BITS_TO_TEST []   = { 24 };
const char*        PIXEL_SHADER_FILE_NAME  = "data/glsl/flat.frag";
const char*        VERTEX_SHADER_FILE_NAME = "data/glsl/flat.vert";

struct MyShaderParameters
{
  math::mat4f transform;
  int         diffuse_texture_slot;
};

struct Vec3f
{
  float x, y, z;

  Vec3f ()
    {}

  Vec3f (float in_x, float in_y, float in_z)
    : x (in_x)
    , y (in_y)
    , z (in_z)
    {}
};

struct Vec2f
{
  float x, y;

  Vec2f ()
    {}

  Vec2f (float in_x, float in_y)
    : x (in_x)
    , y (in_y)
    {}
};

struct Color4f
{
  float red, green, blue, alpha;

  Color4f ()
    {}

  Color4f (float in_red, float in_green, float in_blue, float in_alpha)
    : red   (in_red)
    , green (in_green)
    , blue  (in_blue)
    , alpha (in_alpha)
    {}
};

struct MyVertex
{
  Vec3f      position;
  Vec3f      normal;
  Vec2f      tcoord;
  ::Color4f  color;

  MyVertex ()
   {}

  MyVertex (const Vec3f& in_position, const Vec3f& in_normal, const Vec2f& in_tcoord, const ::Color4f& in_color)
    : position (in_position)
    , normal   (in_normal)
    , tcoord   (in_tcoord)
    , color    (in_color)
    {}
};

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

class IBenchmark
{
  public:
    virtual ~IBenchmark () {}

    virtual void Redraw () = 0;

    virtual void PrintAdditionalTitle () {}
};

stl::vector<size_t> frames_times;

struct Benchmark
{
  stl::string                                             title;
  size_t                                                  start_time;
  size_t                                                  last_frame_time;
  IBenchmark*                                             benchmark;
  xtl::function<IBenchmark* (unsigned int, unsigned int)> start_function;
  unsigned int                                            current_color_bits;
  unsigned int                                            current_depth_bits;

  Benchmark (const char* in_title, xtl::function<IBenchmark* (unsigned int, unsigned int)> in_start_function)
    : title (in_title)
    , start_time (0)
    , last_frame_time ()
    , benchmark ()
    , start_function (in_start_function)
    , current_color_bits (0)
    , current_depth_bits (0)
    {}

  //returns true if benchmark is completed
  bool Redraw ()
  {
    if (!start_time)
    {
      Start ();
      //do nothing on first frame, so all resources can be processed by OpenGL (resources upload to VM, etc.)
      return false;
    }

    size_t current_time = common::milliseconds ();

    frames_times.push_back (current_time - last_frame_time);

    benchmark->Redraw ();

    last_frame_time = current_time;

    if (current_time - start_time > MAX_BENCHMARK_DURATION || frames_times.size () == MAX_BENCHMARK_FRAMES)
    {
      PrintResults ();

      current_color_bits++;

      //all color bits mode tested
      if (current_color_bits >= sizeof (COLOR_BITS_TO_TEST) / sizeof (*COLOR_BITS_TO_TEST))
      {
        current_color_bits = 0;
        current_depth_bits++;

        //all modes tested
        if (current_depth_bits >= sizeof (DEPTH_BITS_TO_TEST) / sizeof (*DEPTH_BITS_TO_TEST))
          return true;
      }

      start_time = 0;

      return false;
    }

    return false;
  }

  void Start ()
  {
    frames_times.clear ();
    benchmark = start_function (COLOR_BITS_TO_TEST [current_color_bits], DEPTH_BITS_TO_TEST [current_depth_bits]);

    printf ("Started benchmark '%s'\n", title.c_str ());

    start_time = last_frame_time = common::milliseconds ();
  }

  void PrintResults ()
  {
    printf ("Benchmark '%s", title.c_str ());
    benchmark->PrintAdditionalTitle ();
    printf ("' for color bits %u depth bits %u completed\n", COLOR_BITS_TO_TEST [current_color_bits], DEPTH_BITS_TO_TEST [current_depth_bits]);

    size_t min_frame_time = -1, max_frame_time = 0, total_frames_time = 0;

    //skip first frame, because it may have wron timing (resources may still upload to VM)
    for (size_t i = 1, count = frames_times.size (); i < count; i++)
    {
      size_t frame_time = frames_times[i];

      if (frame_time < min_frame_time)
        min_frame_time = frame_time;

      //ignore max frame time for first frames
      if (i > count / 5 && frame_time > max_frame_time)
        max_frame_time = frame_time;

      total_frames_time += frame_time;

//      printf ("Frame %d time = %d\n", (int)i, (int)frame_time);
    }

    float average_frame_time = (float)total_frames_time / (float)(frames_times.size () - 1);

    printf ("Min frame time = %d, max frame time = %d (%.2f fps), avg frame time = %.2f (%.2f fps)\n", (int)min_frame_time, (int)max_frame_time, 1000.f / max_frame_time, average_frame_time, 1000 / average_frame_time);

    delete benchmark;
  }
};

class BaseBenchmark : public IBenchmark
{
  public:
    BaseBenchmark (unsigned int color_bits, unsigned int depth_bits)
      : test (new Test (L"OpenGL device test window (benchmark1)", xtl::function<void (Test&)>(), "*", "", color_bits, depth_bits, false, syslib::WindowStyle_PopUp))
    {
      test->window.Show ();
    }

    ~BaseBenchmark ()
    {
      delete test;
    }

    void Redraw ()
    {
      test->OnRedraw ();
    }

  protected:
    Test* test;
};

struct SpriteBlendingBenchmarkParams
{
  unsigned int sprites_count;
  unsigned int texture_resolution;
  bool         use_compressed_texture;
  unsigned int textures_count;
  float        sprite_scale;
  float        texcoord_scale;
  TexMinFilter tex_min_filter;

  SpriteBlendingBenchmarkParams ()
    : sprites_count (1)
    , texture_resolution (4)
    , use_compressed_texture (false)
    , textures_count (1)
    , sprite_scale (1.f)
    , texcoord_scale (1.f)
    , tex_min_filter (TexMinFilter_LinearMipLinear)
    {}

  SpriteBlendingBenchmarkParams (unsigned int in_sprites_count, unsigned int in_texture_resolution,
                                 bool in_use_compressed_texture, unsigned int in_textures_count, float in_sprite_scale = 1.f, float in_texcoord_scale = 1.f,
                                 TexMinFilter in_tex_min_filter = TexMinFilter_LinearMipLinear)
    : sprites_count (in_sprites_count)
    , texture_resolution (in_texture_resolution)
    , use_compressed_texture (in_use_compressed_texture)
    , textures_count (in_textures_count)
    , sprite_scale (in_sprite_scale)
    , texcoord_scale (in_texcoord_scale)
    , tex_min_filter (in_tex_min_filter)
    {}
};

class SpriteBlendingBenchmark : public BaseBenchmark
{
  public:
    SpriteBlendingBenchmark (unsigned int color_bits, unsigned int depth_bits, const SpriteBlendingBenchmarkParams& in_params)
      : BaseBenchmark (color_bits, depth_bits)
      , params (in_params)
      , current_texture (0)
      , current_low_level_component (0)
    {
      verts [0] = MyVertex (Vec3f (-params.sprite_scale, -params.sprite_scale, 0), Vec3f (0, 0, 1), Vec2f (0, 0),                                         ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [1] = MyVertex (Vec3f ( params.sprite_scale, -params.sprite_scale, 0), Vec3f (0, 0, 1), Vec2f (params.texcoord_scale, 0),                     ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [2] = MyVertex (Vec3f ( params.sprite_scale, params.sprite_scale, 0),  Vec3f (0, 0, 1), Vec2f (params.texcoord_scale, params.texcoord_scale), ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [3] = MyVertex (Vec3f (-params.sprite_scale, params.sprite_scale, 0),  Vec3f (0, 0, 1), Vec2f (0, params.texcoord_scale),                     ::Color4f (1.f, 1.f, 1.f, 0.4f));

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (verts);
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      vb[0] = BufferPtr (test->device->CreateBuffer (vb_desc), false);
      vb[1] = BufferPtr (test->device->CreateBuffer (vb_desc), false);

      indices [0] = 0;
      indices [1] = 1;
      indices [2] = 2;
      indices [3] = 3;
      indices [4] = 0;
      indices [5] = 2;

      BufferDesc ib_desc;

      memset (&ib_desc, 0, sizeof ib_desc);

      ib_desc.size         = sizeof indices;
      ib_desc.usage_mode   = UsageMode_Default;
      ib_desc.bind_flags   = BindFlag_IndexBuffer;
      ib_desc.access_flags = AccessFlag_ReadWrite;

      ib[0] = BufferPtr (test->device->CreateBuffer (ib_desc), false);
      ib[1] = BufferPtr (test->device->CreateBuffer (ib_desc), false);

      VertexAttribute attributes [] = {
        {test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal), InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
        {test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position), InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
        {test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color), InputDataFormat_Vector4, InputDataType_Float, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
        {test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_TexCoord0), InputDataFormat_Vector2, InputDataType_Float, 0, offsetof (MyVertex, tcoord), sizeof (MyVertex)},
      };

      InputLayoutDesc layout_desc;

      memset (&layout_desc, 0, sizeof layout_desc);

      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UShort;
      layout_desc.index_buffer_offset     = 0;

      layout[0] = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);
      layout[1] = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                  vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);

      ShaderDesc shader_descs [] = {
        {"p_shader", (unsigned int)-1, pixel_shader_source.c_str (), "glsl.ps", ""},
        {"v_shader", (unsigned int)-1, vertex_shader_source.c_str (), "glsl.vs", ""}
      };

      static ProgramParameter shader_parameters[] = {
        {"DiffuseTexture", ProgramParameterType_Int, 0, 1, TEST_OFFSETOF (MyShaderParameters, diffuse_texture_slot)},
        {"ModelViewProjectionMatrix", ProgramParameterType_Float4x4, 0, 1, TEST_OFFSETOF (MyShaderParameters, transform)}
      };

      ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

      shader[0] = ProgramPtr (test->device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
      shader[1] = ProgramPtr (test->device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
      program_parameters_layout[0] = ProgramParametersLayoutPtr (test->device->CreateProgramParametersLayout (program_parameters_layout_desc), false);
      program_parameters_layout[1] = ProgramParametersLayoutPtr (test->device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

      BufferDesc cb_desc;

      memset (&cb_desc, 0, sizeof cb_desc);

      cb_desc.size         = sizeof (MyShaderParameters);
      cb_desc.usage_mode   = UsageMode_Default;
      cb_desc.bind_flags   = BindFlag_ConstantBuffer;
      cb_desc.access_flags = AccessFlag_ReadWrite;

      cb[0] = BufferPtr (test->device->CreateBuffer (cb_desc), false);
      cb[1] = BufferPtr (test->device->CreateBuffer (cb_desc), false);

      SamplerDesc sampler_desc;

      memset (&sampler_desc, 0, sizeof sampler_desc);

      sampler_desc.min_filter     = params.tex_min_filter;
      sampler_desc.max_anisotropy = 1;
      sampler_desc.mag_filter     = TexMagFilter_Linear;
      sampler_desc.wrap_u         = TexcoordWrap_Repeat;
      sampler_desc.wrap_v         = TexcoordWrap_Repeat;
      sampler_desc.wrap_w         = TexcoordWrap_Repeat;
      sampler_desc.comparision_function = CompareMode_AlwaysPass;
      sampler_desc.min_lod        = 0;
      sampler_desc.max_lod        = FLT_MAX;

      sampler[0] = SamplerStatePtr (test->device->CreateSamplerState (sampler_desc), false);
      sampler[1] = SamplerStatePtr (test->device->CreateSamplerState (sampler_desc), false);

      textures.reserve (params.textures_count);

      PixelFormat pixel_format = params.use_compressed_texture ? PixelFormat_DXT5 : PixelFormat_RGBA8;

      size_t image_size = params.texture_resolution * params.texture_resolution * 4 * 1.5;

      unsigned int mips_levels_count = get_mips_count (params.texture_resolution);

      char*         mem                = new char [image_size];
      unsigned int* texture_data_sizes = new unsigned int [mips_levels_count];

      for (unsigned int i = 0, mip_level_size = params.texture_resolution; i < mips_levels_count; i++, mip_level_size /= 2)
      {
        texture_data_sizes [i] = get_image_size (mip_level_size, mip_level_size, pixel_format);
      }

      for (unsigned int i = 0; i < params.textures_count; i++)
      {
        TextureDesc texture_desc;

        memset (&texture_desc, 0, sizeof texture_desc);

        texture_desc.dimension            = TextureDimension_2D;
        texture_desc.width                = params.texture_resolution;
        texture_desc.height               = params.texture_resolution;
        texture_desc.layers               = 1;
        texture_desc.format               = pixel_format;
        texture_desc.bind_flags           = BindFlag_Texture;
        texture_desc.access_flags         = AccessFlag_ReadWrite;
        texture_desc.generate_mips_enable = false;

        for (size_t i = 0; i < image_size; i++)
        {
          mem [i] = rand () % 255;
        }

        TextureData texture_data;

        memset (&texture_data, 0, sizeof texture_data);

        texture_data.data  = mem;
        texture_data.sizes = texture_data_sizes;

        TexturePtr texture = TexturePtr (test->device->CreateTexture (texture_desc, texture_data), false);

        textures.push_back (texture);
      }

      delete [] mem;
      delete [] texture_data_sizes;

      BlendDesc blend_desc;

      memset (&blend_desc, 0, sizeof blend_desc);

      blend_desc.render_target [0].blend_enable                     = true;
      blend_desc.render_target [0].blend_color_operation            = BlendOperation_Add;
      blend_desc.render_target [0].blend_color_source_argument      = BlendArgument_SourceAlpha;
      blend_desc.render_target [0].blend_color_destination_argument = BlendArgument_InverseSourceAlpha;
      blend_desc.render_target [0].blend_alpha_operation            = BlendOperation_Add;
      blend_desc.render_target [0].blend_alpha_source_argument      = BlendArgument_SourceAlpha;
      blend_desc.render_target [0].blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;
      blend_desc.render_target [0].color_write_mask                 = ColorWriteFlag_All;

      blend_state[0] = BlendStatePtr (test->device->CreateBlendState (blend_desc), false);
      blend_state[1] = BlendStatePtr (test->device->CreateBlendState (blend_desc), false);

      DepthStencilDesc depth_stencil_desc;

      memset (&depth_stencil_desc, 0, sizeof depth_stencil_desc);

      depth_stencil_desc.depth_test_enable  = true;
      depth_stencil_desc.depth_write_enable = false;
      depth_stencil_desc.depth_compare_mode = CompareMode_LessEqual;

      depth_stencil_state[0] = DepthStencilStatePtr (test->device->CreateDepthStencilState (depth_stencil_desc), false);
      depth_stencil_state[1] = DepthStencilStatePtr (test->device->CreateDepthStencilState (depth_stencil_desc), false);
    }

    void Redraw ()
    {
      render::low_level::Color4f clear_color;

      clear_color.red   = 0;
      clear_color.green = 0.7f;
      clear_color.blue  = 0.7f;
      clear_color.alpha = 1.0f;

      unsigned int rt_index = 0;

      test->device->GetImmediateContext ()->ClearViews (ClearFlag_All, 1, &rt_index, &clear_color, 1.0f, 0);

      MyShaderParameters my_shader_parameters = {
        1,
        0
      };

      for (size_t i = 0; i < params.sprites_count; i++)
      {
        vb[current_low_level_component]->SetData (0, sizeof (verts), verts);
        ib[current_low_level_component]->SetData (0, sizeof (indices), indices);
        cb[current_low_level_component]->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);


        test->device->GetImmediateContext ()->ISSetInputLayout (layout[current_low_level_component].get ());
        test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb[current_low_level_component].get ());
        test->device->GetImmediateContext ()->ISSetIndexBuffer (ib[current_low_level_component].get ());
        test->device->GetImmediateContext ()->SSSetTexture (0, textures[current_texture].get ());
        test->device->GetImmediateContext ()->SSSetSampler (0, sampler[current_low_level_component].get ());
        test->device->GetImmediateContext ()->SSSetProgram (shader[current_low_level_component].get ());
        test->device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout[current_low_level_component].get ());
        test->device->GetImmediateContext ()->SSSetConstantBuffer (0, cb[current_low_level_component].get ());
        test->device->GetImmediateContext ()->OSSetBlendState (blend_state[current_low_level_component].get ());
        test->device->GetImmediateContext ()->OSSetDepthStencilState (depth_stencil_state[current_low_level_component].get ());

        test->device->GetImmediateContext ()->DrawIndexed (PrimitiveType_TriangleList, 0, 6, 0);

        current_texture = (current_texture + 1) % textures.size ();
        current_low_level_component = (current_low_level_component + 1) % 2;
      }

      test->device->GetImmediateContext ()->Flush ();

      test->swap_chain->Present ();
    }

    void PrintAdditionalTitle ()
    {
      unsigned int pixel_size   = params.use_compressed_texture ? 1 : 4,
                   texture_size = params.texture_resolution * params.texture_resolution * pixel_size;

      printf (" sprites count %u, texture resolution %u, use compressed texture '%c', textures count %u, sprite scale %.4f, texcoord scale %.4f, texture minification filter %s, VM size = %u",
              params.sprites_count, params.texture_resolution, params.use_compressed_texture ? 'y' : 'n', params.textures_count,
              params.sprite_scale, params.texcoord_scale, get_name (params.tex_min_filter),
              params.textures_count * texture_size / 1024 / 1024);
    }

  private:
    MyVertex                      verts [4];
    unsigned short                indices [6];
    BufferPtr                     vb [2];
    BufferPtr                     ib [2];
    BufferPtr                     cb [2];
    InputLayoutPtr                layout [2];
    stl::vector<TexturePtr>       textures;
    SamplerStatePtr               sampler [2];
    BlendStatePtr                 blend_state [2];
    DepthStencilStatePtr          depth_stencil_state [2];
    ProgramPtr                    shader [2];
    ProgramParametersLayoutPtr    program_parameters_layout [2];
    SpriteBlendingBenchmarkParams params;
    unsigned int                  current_texture;
    unsigned int                  current_low_level_component;
};

IBenchmark* clear_start (unsigned int color_bits, unsigned int depth_bits)
{
  return new BaseBenchmark (color_bits, depth_bits);
}

IBenchmark* sprite_blending_start (unsigned int color_bits, unsigned int depth_bits, const SpriteBlendingBenchmarkParams& params)
{
  return new SpriteBlendingBenchmark (color_bits, depth_bits, params);
}

stl::vector<Benchmark> benchmarks;
size_t                 current_benchmark = 0;

void idle ()
{
  if (current_benchmark >= benchmarks.size ())
  {
    syslib::Application::Exit(0);
    return;
  }

  if (benchmarks[current_benchmark].Redraw ())
    current_benchmark++;
}

int main ()
{
  printf ("Benchmarks started:\n");
  
  try
  {
    frames_times.reserve (MAX_BENCHMARK_FRAMES);

//    benchmarks.push_back (Benchmark ("Clear", &clear_start));
/*    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 1024, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 1024, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 1024, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 2048, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 2048, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 2048, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 4096, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 8192, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 8192, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 8192, true, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 1024, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 1024, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 1024, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 2048, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 2048, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 2048, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 4096, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 8192, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 8192, false, 2))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (40, 8192, false, 2))));*/
/*    benchmarks.push_back (Benchmark ("Sprite blending low vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 2, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending medium vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 5, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 10, 1.f))));*/
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 1", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 2, 1.f, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 0.5", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (8, 4096, false, 2, 0.5f, 0.5f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 0.25", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (32, 4096, false, 2, 0.25f, 0.25f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 0.125", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (64, 4096, false, 2, 0.125f, 0.125f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 0.125", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (128, 4096, false, 2, 0.125f, 0.125f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen scale 0.0625", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (512, 4096, false, 2, 0.0625f, 0.0625f))));
    benchmarks.push_back (Benchmark ("Draw calls", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (1000, 4, false, 2, 0.0001f))));
/*    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 16, true, 1, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 8192, true, 1, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 16, false, 1, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 8192, false, 1, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 10, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 30, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 50, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 70, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, true, 90, 1.f))));*/
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 10, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 16, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 17, 1.f))));
//    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 20, 1.f))));
//    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (2, 4096, false, 30, 1.f))));
/*    benchmarks.push_back (Benchmark ("Sprite blending mipmaps compressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 1.f, TexMinFilter_Linear))));
    benchmarks.push_back (Benchmark ("Sprite blending mipmaps compressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 1.f, TexMinFilter_LinearMipPoint))));
    benchmarks.push_back (Benchmark ("Sprite blending mipmaps compressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 1.f, TexMinFilter_LinearMipLinear))));
    benchmarks.push_back (Benchmark ("Sprite blending mipmaps uncompressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, false, 2, 1.f, 1.f, TexMinFilter_Linear))));
    benchmarks.push_back (Benchmark ("Sprite blending mipmaps uncompressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, false, 2, 1.f, 1.f, TexMinFilter_LinearMipPoint))));
    benchmarks.push_back (Benchmark ("Sprite blending mipmaps uncompressed", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, false, 2, 1.f, 1.f, TexMinFilter_LinearMipLinear))));
    benchmarks.push_back (Benchmark ("Sprite blending texcoord scale", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending texcoord scale", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 0.7f))));
    benchmarks.push_back (Benchmark ("Sprite blending texcoord scale", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (20, 4096, true, 2, 1.f, 0.5f))));*/

/*    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen vs tex size 1024", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (8, 1024, false, 2, 0.5f, 1.f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen vs tex size 2048", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (8, 2048, false, 2, 0.5f, 0.5f))));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen vs tex size 4096", xtl::bind (&sprite_blending_start, _1, _2, SpriteBlendingBenchmarkParams (8, 4096, false, 2, 0.5f, 0.25f))));*/

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle));

    syslib::Application::Run ();

    printf ("All benchmarks completed\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
