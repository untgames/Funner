#include "shared.h"

size_t output_mode   = OutputMode_Default;
size_t total_tests   = 0;
size_t success_tests = 0;
size_t wrong_tests   = 0;
size_t fail_tests    = 0;

void print_desc(const DepthStencilDesc& desc)
{
  static const char* header_names[] = {"StencilDesc", "Compare mode", "Stencil fail operation", "Depth fail operation", "Stencil pass operation"};

  printf("DepthStencilDesc parameters:\n");
  printf("  Depth test: %s\n", desc.depth_test_enable ? "enabled" : "disabled");
  printf("  Depth write: %s\n", desc.depth_write_enable ? "enabled" : "disabled");
  printf("  Stencil test: %s\n", desc.stencil_test_enable ? "enabled" : "disabled");
  printf("  Depth compare mode: %s\n", get_name(desc.depth_compare_mode));
  printf("  Stencil read mask: %02X\n", desc.stencil_read_mask);
  printf("  Stencil write mask: %02X\n", desc.stencil_write_mask);
  printf("=====================================================================================================================================\n");
  printf(" %-15s  | %-24s | %-26s | %-26s | %-26s\n",
    header_names[0], header_names[1], header_names[2], header_names[3], header_names[4]);
  printf("-------------------------------------------------------------------------------------------------------------------------------------\n");
  for (int i = 0; i < FaceMode_Num; i++)
  {
    printf(" %-15s  | %-24s | %-26s | %-26s | %-26s\n",
      get_name((FaceMode) i),
      get_name(desc.stencil_desc[i].stencil_compare_mode),
      get_name(desc.stencil_desc[i].stencil_fail_operation),
      get_name(desc.stencil_desc[i].depth_fail_operation),
      get_name(desc.stencil_desc[i].stencil_pass_operation)
    );
  }
  printf("=====================================================================================================================================\n");
  fflush(stdout);
}

void test_state(const DepthStencilDesc& desc, IDevice* device)
{
 	total_tests++;
  try
  {
    device->OSSetDepthStencilState(device->CreateDepthStencilState(desc));
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

void test_state_recursive(DepthStencilDesc& desc, IDevice* device, int iteration = 0)
{
  for (int scm = 0; scm < CompareMode_Num; scm++)
  {
    desc.stencil_desc[iteration].stencil_compare_mode = (CompareMode) scm;
    for (int sfo = 0; sfo < StencilOperation_Num; sfo++)
    {
      desc.stencil_desc[iteration].stencil_fail_operation = (StencilOperation) sfo;
      for (int dfo = 0; dfo < StencilOperation_Num; dfo++)
      {
        desc.stencil_desc[iteration].depth_fail_operation = (StencilOperation) dfo;
        for (int spo = 0; spo < StencilOperation_Num; spo++)
        {
          desc.stencil_desc[iteration].stencil_pass_operation = (StencilOperation) spo;
          if (iteration < FaceMode_Num - 1)
            test_state_recursive(desc, device, iteration+1);
          else
            test_state(desc, device);
        }
      }
    }
  }
}

int main ()
{
  printf ("Results of all_depth_stencil_states_test:\n");

  try
  {
    Test test (L"OpenGL device test window (all_input_states_test)");
    
    output_mode = test.log_mode;
    
    DepthStencilDesc desc;
    desc.stencil_read_mask = 0xFF;
    desc.stencil_write_mask = 0xFF;
    
    for (int dt = 0; dt < 2; dt++)
    {
      desc.depth_test_enable = (bool) dt;
      for (int dw = 0; dw < 2; dw++)
      {
        desc.depth_write_enable = (bool) dw;
        for (int st = 0; st < 2; st++)
        {
          desc.stencil_test_enable = (bool) st;
          for (int dcm = 0; dcm < CompareMode_Num; dcm++)
          {
            desc.depth_compare_mode = (CompareMode) dcm;
            test_state_recursive(desc, test.device.get());
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
