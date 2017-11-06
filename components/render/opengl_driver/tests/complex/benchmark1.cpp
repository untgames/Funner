#include "shared.h"

const size_t       MAX_BENCHMARK_DURATION  = 5000;
const size_t       MAX_BENCHMARK_FRAMES    = 5000;
const unsigned int COLOR_BITS_TO_TEST []   = { 32 };
const unsigned int DEPTH_BITS_TO_TEST []   = { 16, 24 };
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

      if (frame_time > max_frame_time)
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

class SpriteBlendingBenchmark : public BaseBenchmark
{
  public:
    SpriteBlendingBenchmark (unsigned int color_bits, unsigned int depth_bits, unsigned int in_sprites_count, unsigned int in_texture_resolution, bool in_use_compressed_texture,
                             unsigned int in_textures_count, float sprite_scale)
      : BaseBenchmark (color_bits, depth_bits)
      , sprites_count (in_sprites_count)
      , texture_resolution (in_texture_resolution)
      , use_compressed_texture (in_use_compressed_texture)
      , textures_count (in_textures_count)
      , current_texture (0)
    {
      printf ("Create vertex buffer\n");

      verts [0] = MyVertex (Vec3f (-sprite_scale, -sprite_scale, 0), Vec3f (0, 0, 1), Vec2f (0, 0),                       ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [1] = MyVertex (Vec3f ( sprite_scale, -sprite_scale, 0), Vec3f (0, 0, 1), Vec2f (sprite_scale, 0),            ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [2] = MyVertex (Vec3f ( sprite_scale, sprite_scale, 0),  Vec3f (0, 0, 1), Vec2f (sprite_scale, sprite_scale), ::Color4f (1.f, 1.f, 1.f, 0.4f));
      verts [3] = MyVertex (Vec3f (-sprite_scale, sprite_scale, 0),  Vec3f (0, 0, 1), Vec2f (0, sprite_scale),            ::Color4f (1.f, 1.f, 1.f, 0.4f));

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (verts);
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      vb = BufferPtr (test->device->CreateBuffer (vb_desc), false);

      printf ("Create index buffer\n");

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

      ib = BufferPtr (test->device->CreateBuffer (ib_desc), false);

      printf ("Set input-stage\n");

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

      layout = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      printf ("Set shader stage\n");

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

      shader = ProgramPtr (test->device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
      program_parameters_layout = ProgramParametersLayoutPtr (test->device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

      BufferDesc cb_desc;

      memset (&cb_desc, 0, sizeof cb_desc);

      cb_desc.size         = sizeof (MyShaderParameters);
      cb_desc.usage_mode   = UsageMode_Default;
      cb_desc.bind_flags   = BindFlag_ConstantBuffer;
      cb_desc.access_flags = AccessFlag_ReadWrite;

      cb = BufferPtr (test->device->CreateBuffer (cb_desc), false);

      MyShaderParameters my_shader_parameters = {
        1,
        0
      };

      cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

      test->device->GetImmediateContext ()->SSSetProgram (shader.get ());
      test->device->GetImmediateContext ()->SSSetProgramParametersLayout (program_parameters_layout.get ());
      test->device->GetImmediateContext ()->SSSetConstantBuffer (0, cb.get ());

      printf ("Load textures\n");

      SamplerDesc sampler_desc;

      memset (&sampler_desc, 0, sizeof sampler_desc);

      sampler_desc.min_filter     = TexMinFilter_Linear;
      sampler_desc.max_anisotropy = 1;
      sampler_desc.mag_filter     = TexMagFilter_Linear;
      sampler_desc.wrap_u         = TexcoordWrap_Repeat;
      sampler_desc.wrap_v         = TexcoordWrap_Repeat;
      sampler_desc.wrap_w         = TexcoordWrap_Repeat;
      sampler_desc.comparision_function = CompareMode_AlwaysPass;
      sampler_desc.min_lod        = 0;
      sampler_desc.max_lod        = FLT_MAX;

      sampler = SamplerStatePtr (test->device->CreateSamplerState (sampler_desc), false);

      textures.reserve (textures_count);

      PixelFormat pixel_format = use_compressed_texture ? PixelFormat_DXT5 : PixelFormat_RGBA8;

      size_t image_size = texture_resolution * texture_resolution * 4;

      char* mem = new char [image_size];

      for (unsigned int i = 0; i < textures_count; i++)
      {
        TextureDesc texture_desc;

        memset (&texture_desc, 0, sizeof texture_desc);

        texture_desc.dimension            = TextureDimension_2D;
        texture_desc.width                = texture_resolution;
        texture_desc.height               = texture_resolution;
        texture_desc.layers               = 1;
        texture_desc.format               = pixel_format;
        texture_desc.bind_flags           = BindFlag_Texture;
        texture_desc.access_flags         = AccessFlag_ReadWrite;
        texture_desc.generate_mips_enable = false;

        TexturePtr texture = TexturePtr (test->device->CreateTexture (texture_desc), false);

        for (size_t i = 0; i < image_size; i++)
        {
          mem [i] = rand () % 255;
        }

        texture->SetData (0, 0, 0, 0, texture_resolution, texture_resolution, pixel_format, mem);

        textures.push_back (texture);
      }

      delete [] mem;

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

      blend_state = BlendStatePtr (test->device->CreateBlendState (blend_desc), false);

      DepthStencilDesc depth_stencil_desc;

      memset (&depth_stencil_desc, 0, sizeof depth_stencil_desc);

      depth_stencil_desc.depth_test_enable  = true;
      depth_stencil_desc.depth_write_enable = false;
      depth_stencil_desc.depth_compare_mode = CompareMode_LessEqual;

      depth_stencil_state = DepthStencilStatePtr (test->device->CreateDepthStencilState (depth_stencil_desc), false);
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

      for (size_t i = 0; i < sprites_count; i++)
      {
        vb->SetData (0, sizeof (verts), verts);
        ib->SetData (0, sizeof (indices), indices);

        test->device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
        test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
        test->device->GetImmediateContext ()->ISSetIndexBuffer (ib.get ());
        test->device->GetImmediateContext ()->SSSetTexture (0, textures[current_texture].get ());
        test->device->GetImmediateContext ()->SSSetSampler (0, sampler.get ());
        test->device->GetImmediateContext ()->OSSetBlendState (blend_state.get ());
        test->device->GetImmediateContext ()->OSSetDepthStencilState (depth_stencil_state.get ());

        test->device->GetImmediateContext ()->DrawIndexed (PrimitiveType_TriangleList, 0, 6, 0);

        current_texture = (current_texture + 1) % textures.size ();
      }

      test->device->GetImmediateContext ()->Flush ();

      test->swap_chain->Present ();
    }

    void PrintAdditionalTitle ()
    {
      unsigned int pixel_size   = use_compressed_texture ? 1 : 4,
                   texture_size = texture_resolution * texture_resolution * pixel_size;

      printf (" sprites count %u, texture resolution %u, use compressed texture '%c', textures count %u, VM size = %u", sprites_count, texture_resolution, use_compressed_texture ? 'y' : 'n', textures_count, textures_count * texture_size / 1024 / 1024);
    }

  private:
    MyVertex                   verts [4];
    unsigned short             indices [6];
    BufferPtr                  vb;
    BufferPtr                  ib;
    BufferPtr                  cb;
    InputLayoutPtr             layout;
    stl::vector<TexturePtr>    textures;
    SamplerStatePtr            sampler;
    BlendStatePtr              blend_state;
    DepthStencilStatePtr       depth_stencil_state;
    ProgramPtr                 shader;
    ProgramParametersLayoutPtr program_parameters_layout;
    unsigned int               sprites_count;
    unsigned int               texture_resolution;
    bool                       use_compressed_texture;
    unsigned int               textures_count;
    unsigned int               current_texture;
};

IBenchmark* clear_start (unsigned int color_bits, unsigned int depth_bits)
{
  return new BaseBenchmark (color_bits, depth_bits);
}

IBenchmark* sprite_blending_start (unsigned int color_bits, unsigned int depth_bits, unsigned int sprites_count, unsigned int texture_resolution,
                                   bool use_compressed_texture, unsigned int textures_count, float sprite_scale)
{
  return new SpriteBlendingBenchmark (color_bits, depth_bits, sprites_count, texture_resolution, use_compressed_texture, textures_count, sprite_scale);
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

    benchmarks.push_back (Benchmark ("Clear", &clear_start));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 1024, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 1024, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 1024, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 2048, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 2048, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 2048, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 4096, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res compressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 4096, true, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 1024, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 1024, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen low res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 1024, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 2048, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 2048, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen medium res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 2048, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 20, 4096, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen high res uncompressed texture", xtl::bind (&sprite_blending_start, _1, _2, 40, 4096, false, 1, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending low vm usage", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, false, 2, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending medium vm usage", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, false, 5, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending high vm usage", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, false, 10, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen", xtl::bind (&sprite_blending_start, _1, _2, 2, 4096, false, 2, 1.f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen", xtl::bind (&sprite_blending_start, _1, _2, 8, 4096, false, 2, 0.5f)));
    benchmarks.push_back (Benchmark ("Sprite blending full screen vs non fullscreen", xtl::bind (&sprite_blending_start, _1, _2, 32, 4096, false, 2, 0.25f)));
    benchmarks.push_back (Benchmark ("Draw calls", xtl::bind (&sprite_blending_start, _1, _2, 1000, 4, false, 2, 0.0001f)));

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
