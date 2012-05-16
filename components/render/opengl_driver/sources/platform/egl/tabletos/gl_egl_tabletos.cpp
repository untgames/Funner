#include "../shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace render
{

namespace low_level
{

namespace opengl
{

namespace egl
{

void setup_window_buffers (const void* window_handle, const SwapChainDesc& desc)
{
  try
  {
    screen_window_t window = (screen_window_t)window_handle;

      //Set pixel format
    
    int format = SCREEN_FORMAT_RGBX8888;
   
    switch (desc.frame_buffer.color_bits)
    {
      case 32:
        if (desc.frame_buffer.alpha_bits)
          throw xtl::format_operation_exception ("", "Can't create primary swap chain with color bits %u alpha bits %u", desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits);

        format = SCREEN_FORMAT_RGBX8888;

        break;
      case 24:
        switch (desc.frame_buffer.alpha_bits)
        {
          case 0:
            format = SCREEN_FORMAT_RGBX8888;
            break;
          case 8:
            format = SCREEN_FORMAT_RGBA8888;
            break;
          default:
            throw xtl::format_operation_exception ("", "Can't create primary swap chain with color bits %u alpha bits %u", desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits);
        }

        break;
      case 16:
        switch (desc.frame_buffer.alpha_bits)
        {
          case 0:
            format = SCREEN_FORMAT_RGBX5551;
            break;
          case 1:
            format = SCREEN_FORMAT_RGBA5551;
            break;
          case 4:
            format = SCREEN_FORMAT_RGBA4444;
            break;
          default:
            throw xtl::format_operation_exception ("", "Can't create primary swap chain with color bits %u alpha bits %u", desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits);
        }

        break;
      default:
        throw xtl::format_operation_exception ("", "Can't create primary swap chain with color bits %u", desc.frame_buffer.color_bits);
    }

    if (screen_set_window_property_iv (window, SCREEN_PROPERTY_FORMAT, &format))
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
      
      //Set usage
      
    int usage = 0;

    if (screen_get_window_property_iv (window, SCREEN_PROPERTY_USAGE, &usage))
      raise_error ("screen_get_window_property_iv(SCREEN_PROPERTY_USAGE)");
   
    usage |= SCREEN_USAGE_OPENGL_ES1;

    if (screen_set_window_property_iv (window, SCREEN_PROPERTY_USAGE, &usage))
      raise_error ("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");

      //Get display

    screen_display_t screen_disp;
    if(screen_get_window_property_pv(window, SCREEN_PROPERTY_DISPLAY, (void **)&screen_disp))
      raise_error ("::screen_get_window_property_pv SCREEN_PROPERTY_DISPLAY");

    screen_display_mode_t screen_mode;
    if(screen_get_display_property_pv(screen_disp, SCREEN_PROPERTY_MODE, (void**)&screen_mode))
      raise_error ("::screen_get_display_property_pv SCREEN_PROPERTY_MODE");


    int angle = atoi(getenv("ORIENTATION"));
    int size[2];

    if(screen_get_window_property_iv(window, SCREEN_PROPERTY_BUFFER_SIZE, size))
      raise_error ("::screen_get_window_property_iv SCREEN_PROPERTY_BUFFER_SIZE");

    int buffer_size[2] = {size[0], size[1]};

    if ((angle == 0) || (angle == 180)) 
    {
      if (((screen_mode.width > screen_mode.height) && (size[0] < size[1])) ||
          ((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) 
      {
        buffer_size[1] = size[0];
        buffer_size[0] = size[1];
      }
    } 
    else if ((angle == 90) || (angle == 270))
    {
      if (((screen_mode.width > screen_mode.height) && (size[0] > size[1])) ||
          ((screen_mode.width < screen_mode.height && size[0] < size[1]))) 
      {
        buffer_size[1] = size[0];
        buffer_size[0] = size[1];
      }
    } 
    else 
      raise_error ("Can't determine buffer size");

    if(screen_set_window_property_iv (window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size))
      raise_error ("::screen_set_window_property_iv SCREEN_PROPERTY_BUFFER_SIZE");

    if(screen_set_window_property_iv (window, SCREEN_PROPERTY_ROTATION, &angle))
      raise_error ("::screen_set_window_property_iv SCREEN_PROPERTY_ROTATION");

    if(screen_create_window_buffers (window, desc.buffers_count))
      raise_error ("::screen_create_window_buffers");
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::egl::setup_window_buffers");
    throw;
  }
}

}

}

}

}
