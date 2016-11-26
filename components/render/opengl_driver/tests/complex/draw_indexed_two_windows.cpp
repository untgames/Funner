#include <cstdio>
#include <cmath>
#include <exception>

#include <float.h>

#include <math/matrix.h>
#include <math/utility.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/string>
#include <stl/hash_map>

#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/connection.h>
#include <xtl/ref.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/time.h>

#include <media/compressed_image.h>
#include <media/image.h>
#include <media/mesh.h>

using namespace render::low_level;

#ifdef _MSC_VER
  #define TEST_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define TEST_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

typedef xtl::com_ptr<IDriver>                  DriverPtr;
typedef xtl::com_ptr<ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<IDevice>                  DevicePtr;
typedef xtl::com_ptr<ITexture>                 TexturePtr;
typedef xtl::com_ptr<ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<IView>                    ViewPtr;
typedef xtl::com_ptr<IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<IBuffer>                  BufferPtr;
typedef xtl::com_ptr<IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<IProgram>                 ProgramPtr;
typedef xtl::com_ptr<IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<IPredicate>               PredicatePtr;

//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
struct TestLogFilter
{
  common::LogFilter log_filter;

  TestLogFilter () : log_filter ("*", &LogPrint) {}

  static void LogPrint (const char* log_name, const char* message)
  {
    printf ("%s: %s\n", log_name, message);
    fflush (stdout);
  }
};

typedef common::Singleton<TestLogFilter> TestLogFilterSingleton;

//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
struct Test
{
  typedef xtl::function<void (Test&)> CallbackFn;

  syslib::Window     window1;
  syslib::Window     window2;
  SwapChainPtr       swap_chain1;
  SwapChainPtr       swap_chain2;
  ViewPtr            render_target_view1;
  ViewPtr            depth_stencil_view1;
  ViewPtr            render_target_view2;
  ViewPtr            depth_stencil_view2;
  DevicePtr          device;
  CallbackFn         redraw;

  Test (const wchar_t* title, const CallbackFn& in_redraw, const char* adapter_mask="*", const char* init_string="") :
    window1 (syslib::WindowStyle_Overlapped, 400, 400),
    window2 (syslib::WindowStyle_Overlapped, 400, 400),
    redraw (in_redraw)
  {
    TestLogFilterSingleton::Instance (); //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ

    window1.SetTitle (title);
    window1.Maximize ();
    window1.SetClientSize(400, 400);
    window1.SetPosition(100, 100);

    window2.SetTitle (title);
    window2.Maximize ();
    window2.SetClientSize(400, 400);
    window2.SetPosition(100, 550);

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 16;
    desc.frame_buffer.depth_bits   = 16;
    desc.buffers_count             = 2;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.window_handle             = window1.Handle ();

    DriverManager::CreateSwapChainAndDevice ("OpenGL", adapter_mask, desc, init_string, swap_chain1, device);

    desc.window_handle             = window2.Handle ();

    render_target_view1 = device->GetImmediateContext()->OSGetRenderTargetView(0);
    depth_stencil_view1 = device->GetImmediateContext()->OSGetDepthStencilView();

    swap_chain2 = DriverManager::CreateSwapChain ("OpenGL", adapter_mask, desc);

    SwapChainDesc swap_chain_desc;

    swap_chain2->GetDesc (swap_chain_desc);

    xtl::com_ptr<ITexture> color_texture (device->CreateRenderTargetTexture(swap_chain2.get(), 1), false),
                           depth_stencil_texture (device->CreateDepthStencilTexture(swap_chain2.get()), false);

    ViewDesc view_desc;

    memset (&view_desc, 0, sizeof (view_desc));

    render_target_view2 = ViewPtr (device->CreateView(color_texture.get (), view_desc), false);
    depth_stencil_view2 = ViewPtr (device->CreateView (depth_stencil_texture.get (), view_desc), false);

//    swap_chain->SetFullscreenState (true);

    OnResize ();

    window1.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Test::OnRedraw, this));
    window1.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Test::OnResize, this));
    window1.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnClose, this));

    window1.Invalidate ();
    window2.Invalidate ();
  }

  void OnResize ()
  {
    try
    {
      syslib::Rect rect = window1.ClientRect ();

      Viewport vp;

      vp.x         = rect.left;
      vp.y         = rect.top;
      vp.width     = rect.right - rect.left;
      vp.height    = rect.bottom - rect.top;
      vp.min_depth = 0;
      vp.max_depth = 1;

      DeviceCaps caps;

      device->GetCaps (caps);

//      if (caps.has_right_hand_viewport)
//        vp.y = window.Height () - vp.height - vp.y;

      device->GetImmediateContext ()->RSSetViewport (0, vp);
    }
    catch (std::exception& e)
    {
      printf ("resize exception: %s\n", e.what ()); fflush (stdout);
    }
  }

  void OnRedraw ()
  {
    try
    {
      Color4f clear_color;

      clear_color.red   = 0;
      clear_color.green = 0.7f;
      clear_color.blue  = 0.7f;
      clear_color.alpha = 1.0f;

      unsigned int rt_index = 0;

      device->GetImmediateContext ()->OSSetRenderTargetView (0, render_target_view1.get ());
      device->GetImmediateContext ()->OSSetDepthStencilView (depth_stencil_view1.get ());

      device->GetImmediateContext ()->ClearViews (ClearFlag_All, 1, &rt_index, &clear_color, 1.0f, 0);

      if (redraw)
        redraw (*this);

      device->GetImmediateContext ()->Flush ();

      swap_chain1->Present ();

      device->GetImmediateContext ()->OSSetRenderTargetView (0, render_target_view2.get ());
      device->GetImmediateContext ()->OSSetDepthStencilView (depth_stencil_view2.get ());

      clear_color.red   = 0.7;
      clear_color.green = 0.0f;
      clear_color.blue  = 0.7f;
      clear_color.alpha = 1.0f;

      device->GetImmediateContext ()->ClearViews (ClearFlag_All, 1, &rt_index, &clear_color, 1.0f, 0);

      if (redraw)
        redraw (*this);

      device->GetImmediateContext ()->Flush ();

      swap_chain2->Present ();
    }
    catch (std::exception& e)
    {
      printf ("redraw exception: %s\n", e.what ()); fflush (stdout);
    }
  }

  void OnClose ()
  {
    syslib::Application::Exit (0);
  }
};

//пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer ((size_t)file.Size (), ' ');

  file.Read (&buffer [0], (size_t)file.Size ());

  return buffer;
}

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

size_t frames_count = 0;

void redraw (Test& test)
{
  frames_count++;

  test.device->GetImmediateContext ()->DrawIndexed (PrimitiveType_TriangleList, 0, 3, 0);
}

void idle (Test& test)
{
  if (test.window1.IsClosed ())
    return;

  static size_t last_fps = 0;

  if (common::milliseconds () - last_fps > 1000)
  {
    printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
    fflush (stdout);

    last_fps = common::milliseconds ();
    frames_count = 0;
    return;
  }

  test.OnRedraw ();
}

int main ()
{
  printf ("Results of draw_indexed_two_windows_test:\n");

  try
  {
    Test test (L"OpenGL device test window (draw_indexed)", &redraw);

    printf ("Create vertex buffer\n");

    static const size_t VERTICES_COUNT = 3;

    BufferDesc vb_desc;

    memset (&vb_desc, 0, sizeof vb_desc);

    vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);

    static const MyVertex verts [] = {
      {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
      {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
      {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
    };

    vb->SetData (0, vb_desc.size, verts);

    printf ("Create index buffer\n");

    static unsigned short indices [] = {0, 1, 2};

    BufferDesc ib_desc;

    memset (&ib_desc, 0, sizeof ib_desc);

    ib_desc.size         = sizeof indices;
    ib_desc.usage_mode   = UsageMode_Default;
    ib_desc.bind_flags   = BindFlag_IndexBuffer;
    ib_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr ib (test.device->CreateBuffer (ib_desc), false);

    ib->SetData (0, ib_desc.size, indices);

    printf ("Set input-stage\n");

    VertexAttribute attributes [] = {
      {test.device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal), InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
      {test.device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position), InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
      {test.device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color), InputDataFormat_Vector4, InputDataType_UByte, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
    };

    InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UShort;
    layout_desc.index_buffer_offset     = 0;

    InputLayoutPtr layout (test.device->CreateInputLayout (layout_desc), false);

    test.device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
    test.device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
    test.device->GetImmediateContext ()->ISSetIndexBuffer (ib.get ());

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
