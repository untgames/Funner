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
const char* screenshot_name = "screenshots/screenshot1.tga";
const char* texture_name    = "data/texture_hut.tga";


float sprite_angle = 0;
float sprite_rotation = 0;
bool screenshot_made = false;

void idle (syslib::Window& window, render::mid_level::renderer2d::IRenderer* renderer, Sprite& sprite1, Sprite& sprite2, IPrimitive* primitive1, IPrimitive* primitive2)
{
  if (window.IsClosed ())
    return;

  renderer->DrawFrames ();

  if (!screenshot_made)
  {
    media::Image screenshot;

    renderer->GetColorBuffer ()->CaptureImage (screenshot);

    screenshot.Save (screenshot_name);

    screenshot_made = true;
  }

/*  sprite1.position = vec3f (cos (sprite_angle) * 10, sin (sprite_angle) * 10, 0.2f);  
  sprite2.position = vec3f (cos (sprite_angle + 180.f) * 10, sin (sprite_angle + 180.f) * 10, 0.1f);

  primitive1->RemoveAllSprites ();
  primitive1->AddSprites (1, &sprite1);

  primitive2->RemoveAllSprites ();
  primitive2->AddSprites (1, &sprite2);
  */

  mat4f tm1 = rotatef (sprite_rotation, 0, 0, 1) * translatef (cos (sprite_angle) * 10 + 5, sin (sprite_angle) * 10 + 5, 0.1f) * scalef (20, 20, 1) * rotatef (-sprite_rotation, 0, 0, 1);
  mat4f tm2 = translatef (cos (sprite_angle + 3.1415926535897932384626433832795f) * 10 + 5, sin (sprite_angle + 3.1415926535897932384626433832795f) * 10 + 5, 0.2f) * rotatef (sprite_rotation, 0, 0, 1) * scalef (20, 20, 1) * rotatef (-sprite_rotation, 0, 0, 1);

  primitive1->SetTransform (tm1);
  primitive2->SetTransform (tm2);
  
  sprite_angle += 0.0005f;
  sprite_rotation += 0.0005f;
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

    media::Image texture_image (texture_name, media::PixelFormat_RGBA8);

    xtl::com_ptr<render::mid_level::renderer2d::ITexture>   /*texture1  (renderer->CreateTexture (64, 64, media::PixelFormat_RGBA8), false),*/ texture2 (renderer->CreateTexture (texture_image), false), texture1 (texture2);
    xtl::com_ptr<render::mid_level::renderer2d::IPrimitive> primitive1 (renderer->CreatePrimitive (), false), primitive2 (renderer->CreatePrimitive (), false);

    Sprite sprite1 = {vec3f (0.f, 0.f, 0.f), vec2f (1.f, 1.f), vec4f (1.f, 1.f, 1.f, 0.5f), vec2f (0.f, 0.f), vec2f (1.f, 1.f)};
    Sprite sprite2 = {vec3f (0.f, 0.f, 0.f), vec2f (1.f, 1.f), vec4f (1.f, 1.f, 1.f, 0.5f), vec2f (0.f, 0.f), vec2f (1.f, 1.f)};

    primitive1->SetTexture (texture1.get ());
    primitive2->SetTexture (texture2.get ());

    primitive1->SetBlendMode (BlendMode_Additive);
    primitive2->SetBlendMode (BlendMode_Mask);

    printf ("Primitive1 sprites count is %u\n", primitive1->GetSpritesCount ());
    primitive1->AddSprites (1, &sprite1);
    primitive2->AddSprites (1, &sprite2);
    printf ("Primitive1 sprites count is %u\n", primitive1->GetSpritesCount ());

    // building frame

    render::mid_level::Viewport viewport = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    frame->SetProjection (get_ortho_proj (-100, 100, -100, 100, -1000, 1000));
    frame->SetViewport (viewport);
    frame->SetClearColor (math::vec4f (0.7f, 0.f, 0.f, 0.4f));
    frame->SetClearBuffers (true, true);
    frame->SetRenderTargets (renderer->GetColorBuffer (), renderer->GetDepthStencilBuffer ());

    printf ("Frame primitives count is %u\n", frame->PrimitivesCount ());
    frame->AddPrimitive (primitive1.get ());
    frame->AddPrimitive (primitive2.get ());
    printf ("Frame primitives count is %u\n", frame->PrimitivesCount ());

    renderer->AddFrame (frame.get ());
     
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (window), renderer.get (), xtl::ref(sprite1), xtl::ref (sprite2), primitive1.get (), primitive2.get ()));

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
