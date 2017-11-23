#include "shared.h"

const char* IMAGE_PATH = "data/bottom.jpg";

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

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Vec2f    tcoord;
  Color4ub color;
};

class DrawIndexedTexturedApplication
{
  TestPtr         test;
  BufferPtr       vb;
  BufferPtr       ib;
  InputLayoutPtr  layout;
  TexturePtr      texture;
  SamplerStatePtr sampler;

  static void Redraw (Test& test)
  {
    test.device->GetImmediateContext ()->DrawIndexed (PrimitiveType_TriangleList, 0, 6, 0);  
  }

  void OnInitialize ()
  {
    try
    {
      test = new Test(L"OpenGL device test window (draw_indexed_textured)", &Redraw);

      test->window.Show ();

      CreateVertexBuffer ();
      CreateIndexBuffer ();
      SetInputStage ();
      LoadTextures (IMAGE_PATH);
    }
    catch (const xtl::exception& e)
    {
      printf("%s failed: %s\n", __FUNCTION__, e.what());
      syslib::Application::Exit (1);
    }
  }

  void OnExit ()
  {
    // Test object should be destroyed
    // BEFORE application exit.
    test.reset ();
  }

  void CreateVertexBuffer ()
  {
    try
    {
      printf ("Create vertex buffer\n");
      
      static const MyVertex verts [] = {
        {{-1, -1, 0}, {0, 0, 1}, {0, 0}, {255, 255, 255, 255}},
        {{ 1, -1, 0}, {0, 0, 1}, {1, 0}, {255, 255, 255, 255}},
        {{ 1, 1, 0},  {0, 0, 1}, {1, 1}, {255, 255, 255, 255}},
        {{-1, 1, 0},  {0, 0, 1}, {0, 1}, {255, 255, 255, 255}},
      };    
      
      static const size_t VERTICES_COUNT = sizeof verts / sizeof *verts;
      
      BufferDesc vb_desc;
      
      memset (&vb_desc, 0, sizeof vb_desc);
      
      vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
      
      vb = BufferPtr (test->device->CreateBuffer (vb_desc), false);    
      
      vb->SetData (0, vb_desc.size, verts);
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void CreateIndexBuffer ()
  {
    try
    {
      printf ("Create index buffer\n");    

      static unsigned short indices [] = {0, 1, 2, 3, 0, 2};

      BufferDesc ib_desc;

      memset (&ib_desc, 0, sizeof ib_desc);

      ib_desc.size         = sizeof indices;
      ib_desc.usage_mode   = UsageMode_Default;
      ib_desc.bind_flags   = BindFlag_IndexBuffer;
      ib_desc.access_flags = AccessFlag_ReadWrite;

      ib = BufferPtr (test->device->CreateBuffer (ib_desc), false);

      ib->SetData (0, ib_desc.size, indices);  
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void SetInputStage ()
  {
    try
    {
      printf ("Set input-stage\n");
      
      VertexAttribute attributes [] = {
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , offsetof (MyVertex, normal)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , offsetof (MyVertex, position)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color)
          , InputDataFormat_Vector4
          , InputDataType_UByte
          , 0
          , offsetof (MyVertex, color)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_TexCoord0)
          , InputDataFormat_Vector2
          , InputDataType_Float
          , 0
          , offsetof (MyVertex, tcoord)
          , sizeof (MyVertex)
        },
      };
      
      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UShort;
      layout_desc.index_buffer_offset     = 0;
      
      layout = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      test->device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
      test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
      test->device->GetImmediateContext ()->ISSetIndexBuffer (ib.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void LoadTextures (const char* path)
  {
    try
    {
      printf ("Load textures\n");
      
      media::Image image (path, media::PixelFormat_RGB8);

      TextureDesc texture_desc;
      
      memset (&texture_desc, 0, sizeof texture_desc);
      
      texture_desc.dimension            = TextureDimension_2D;
      texture_desc.width                = image.Width ();
      texture_desc.height               = image.Height ();
      texture_desc.layers               = 1;
      texture_desc.format               = PixelFormat_RGB8;
      texture_desc.bind_flags           = BindFlag_Texture;
      texture_desc.access_flags         = AccessFlag_ReadWrite;
      texture_desc.generate_mips_enable = true;
      
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

      texture = TexturePtr (test->device->CreateTexture (texture_desc), false);
      sampler = SamplerStatePtr (test->device->CreateSamplerState (sampler_desc), false);

      texture->SetData (0, 0, 0, 0, image.Width (), image.Height (), PixelFormat_RGB8, image.Bitmap ());

      test->device->GetImmediateContext ()->SSSetTexture (0, texture.get ());
      test->device->GetImmediateContext ()->SSSetSampler (0, sampler.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

public:
  DrawIndexedTexturedApplication ()
  {
    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnInitialize
      , xtl::bind(&DrawIndexedTexturedApplication::OnInitialize, this)
    );

    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnExit
      , xtl::bind(&DrawIndexedTexturedApplication::OnExit, this)
    );
  }

  void Run ()
  {
    printf ("Main loop\n");

    syslib::Application::Run ();
  }
};

int main ()
{
  printf ("Results of draw_indexed_textured_test:\n");
  
  try
  {
    DrawIndexedTexturedApplication app;

    app.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }

  return 0;
}
