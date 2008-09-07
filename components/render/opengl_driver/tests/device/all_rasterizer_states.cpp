#include "shared.h"

size_t output_mode   = OutputMode_Default;
size_t total_tests   = 0;
size_t success_tests = 0;
size_t wrong_tests   = 0;
size_t fail_tests    = 0;

void print_desc(const RasterizerDesc& desc)
{
  printf("RasterizerDesc parameters:\n");
  printf("================================================================================\n");
  printf("  Fill mode: %s\n", get_name(desc.fill_mode));
  printf("  Culling mode: %s\n", get_name(desc.cull_mode));
  printf("  Front detection mode: %s\n", desc.front_counter_clockwise ? "clockwise" : "counterclockwise");
  printf("  Depth bias: %d\n", desc.depth_bias);
  printf("  Scissor: %s\n", desc.scissor_enable ? "enabled" : "disabled");
  printf("  Multisample: %s\n", desc.multisample_enable ? "enabled" : "disabled");
  printf("  Line antialiasing: %s\n", desc.antialiased_line_enable ? "enabled" : "disabled");
  printf("================================================================================\n");
  fflush(stdout);
}

void test_state(const RasterizerDesc& desc, IDevice* device)
{
	total_tests++;
  try
  {
    device->RSSetState(device->CreateRasterizerState(desc));
    if (output_mode & OutputMode_Success)
    {
      print_desc(desc);
      printf("Testing... OK\n");
    }
    success_tests++;
  }
  catch (xtl::argument_exception&)
  {
  	wrong_tests++;
  }
  catch (std::exception& exception)
  {
    if (output_mode & OutputMode_Fail)
    {
      print_desc(desc);
      printf ("Testing... FAIL! exception: %s\n", exception.what ());
    }
    fail_tests++;
  }
}

int main ()
{
  printf ("Results of all_blend_states test:\n");

  try
  {
    Test test (L"OpenGL device test window (all_blend_states test)");
    
    output_mode = test.log_mode;
    
    RasterizerDesc desc;
    desc.depth_bias = 0;
    
    for (int fm = 0; fm < FillMode_Num; fm++)
    {
      desc.fill_mode = (FillMode) fm;
      for (int cm = 0; cm < CullMode_Num; cm++)
      {
        desc.cull_mode = (CullMode) cm;
        for (int front = 0; front < 2; front++)
        {
          desc.front_counter_clockwise = (bool) front;
          for (int scissor = 0; scissor < 2; scissor++)
          {
            desc.scissor_enable = (bool) scissor;
            for (int mult = 0; mult < 2; mult++)
            {
              desc.multisample_enable = (bool) mult;
              for (int anti = 0; anti < 2; anti++)
              {
                desc.antialiased_line_enable = (bool) anti;
                test_state(desc, test.device.get());
              }
            }
          }
        }
      }
    }
    printf ("Total tests count:   %u\n", total_tests);
    printf ("Success tests count: %u\n", success_tests);
    printf ("Wrong tests count:   %u\n", wrong_tests);
    printf ("Fail tests count:    %u\n", fail_tests);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
