#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace math;

typedef xtl::com_ptr<render::low_level::IDriver> DriverPtr;
typedef xtl::com_ptr<ISwapChain>                 SwapChainPtr;
typedef xtl::com_ptr<IDevice>                    DevicePtr;

const size_t WINDOW_WIDTH   = 1024;
const size_t WINDOW_HEIGHT  = 768;
const char* TEXTURE_NAME    = "data/texture_hut.tga";
const size_t SPRITES_COUNT  = 1000;

float rotation_angle = 0;

void idle (syslib::Window& window, render::mid_level::renderer2d::IRenderer* renderer, render::mid_level::renderer2d::IFrame* frame, IPrimitive* primitive)
{
  if (window.IsClosed ())
    return;

  primitive->SetTransform (rotatef (rotation_angle, 0, 0, 1));
  
  renderer->AddFrame (frame);
     
  rotation_angle += 0.05f;

  renderer->DrawFrames ();
}

//получение ортографической матрицы проекции
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar)
{
  math::mat4f proj_matrix;
  
  float width  = right - left,
        height = top - bottom,
        depth  = zfar - znear;  

    //выбрана матрица проецирования, используемая gluOrtho2D

  proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = math::vec4f (0, 0, -2.0f / depth, - (znear + zfar) / depth);
  proj_matrix [3] = math::vec4f (0, 0, 0, 1);
  
  return proj_matrix;
}

void log_handler (const char* log_name, const char* log_message)
{
  printf ("Log message from log '%s': '%s'\n", log_name, log_message);
}

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

int main ()
{
  try
  {
    printf ("Low level driver name is '%s'\n", LowLevelDriver::Name ());

    common::LogSystem::RegisterLogHandler ("*", &log_handler);

    render::mid_level::IDriver *low_level_driver = LowLevelDriver::Driver ();

    printf ("Driver description: '%s'\n", low_level_driver->GetDescription ());
    printf ("Renderers count is %u\n", low_level_driver->GetRenderersCount ());

    printf ("Renderers: ");
    for (size_t i = 0; i < low_level_driver->GetRenderersCount (); i++)
      printf ("%s; ", low_level_driver->GetRendererName (i));
    printf ("\n");

    syslib::Window window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.Show ();

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&syslib::Application::Exit, 0));

    SwapChainPtr   swap_chain;
    DevicePtr      device;
    SwapChainDesc  desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 0;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();
    
    render::low_level::DriverManager::CreateSwapChainAndDevice ("*", desc, "", swap_chain, device);

    LowLevelDriver::RegisterRenderer ("MyRenderer", device.get (), swap_chain.get ());

    xtl::com_ptr<render::mid_level::renderer2d::IRenderer> renderer (dynamic_cast<render::mid_level::renderer2d::IRenderer*> (render::mid_level::DriverManager::CreateRenderer (LowLevelDriver::Name (), "MyRenderer")), false);

    if (!renderer)
      throw xtl::format_operation_exception ("", "No 2d renderer");

    xtl::com_ptr<render::mid_level::renderer2d::IFrame> frame (dynamic_cast<render::mid_level::renderer2d::IFrame*> (renderer->CreateFrame ()), false);

    if (!frame)
      throw xtl::format_operation_exception ("", "Can't create 2d frame");

    media::Image texture_image (TEXTURE_NAME, media::PixelFormat_RGBA8);

    xtl::com_ptr<render::mid_level::renderer2d::ITexture>   texture (renderer->CreateTexture (texture_image), false);
    xtl::com_ptr<render::mid_level::renderer2d::IPrimitive> primitive (renderer->CreatePrimitive (), false);

    primitive->SetTexture (texture.get ());

    primitive->SetBlendMode (BlendMode_Translucent);

    for (size_t i = 0; i < SPRITES_COUNT; i++)
    {
      Sprite sprite;

      sprite.position   = vec3f (frand (-100, 100), frand (-100, 100), 0);
      sprite.size       = vec2f (frand (1, 20), frand (1, 20));
      sprite.color      = vec4f (frand (), frand (), frand (), frand ());
      sprite.tex_offset = vec2f (0.f, 0.f);
      sprite.tex_size   = vec2f (1.f, 1.f);

      primitive->AddSprites (1, &sprite);
    }

    // building frame

    render::mid_level::Viewport viewport = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    frame->SetProjection (get_ortho_proj (-100, 100, -100, 100, -1000, 1000));
    frame->SetViewport (viewport);
    frame->SetClearColor (math::vec4f (0.7f, 0.f, 0.f, 0.f));
    frame->SetClearBuffers (true, true);
    frame->SetRenderTargets (renderer->GetColorBuffer (), renderer->GetDepthStencilBuffer ());
    frame->AddPrimitive (primitive.get ());

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (window), renderer.get (), frame.get (), primitive.get ()));

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
