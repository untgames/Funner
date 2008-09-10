#include "shared.h"

size_t output_mode   = OutputMode_Default;
size_t success_tests = 0;
size_t wrong_tests   = 0;
size_t total_tests   = 0;

const char* get_argument_name (BlendArgument arg)
{
  switch (arg)
  {
    case BlendArgument_Zero:                    return "0";
    case BlendArgument_One:                     return "1";
    case BlendArgument_SourceColor:             return "src.color";
    case BlendArgument_SourceAlpha:             return "src.alpha";
    case BlendArgument_InverseSourceColor:      return "inv(src.color)";
    case BlendArgument_InverseSourceAlpha:      return "inv(src.alpha)";
    case BlendArgument_DestinationColor:        return "dst.color";
    case BlendArgument_DestinationAlpha:        return "dst.alpha";
    case BlendArgument_InverseDestinationColor: return "inv(dst.color)";
    case BlendArgument_InverseDestinationAlpha: return "inv(dst.alpha)";
    default:                                    return "???";
  }  
}

void print_blend_equation (BlendOperation op, BlendArgument arg1, BlendArgument arg2)
{
  const char* op_format = "?";
  
  switch (op)
  {
    default:                                op_format = ""; break;
    case BlendOperation_Add:                op_format = "%s + %s"; break;
    case BlendOperation_Subtraction:        op_format = "%s - %s"; break;
    case BlendOperation_ReverseSubtraction: op_format = "-%s + %s"; break;
    case BlendOperation_Min:                op_format = "min(%s, %s)"; break;
    case BlendOperation_Max:                op_format = "max(%s, %s)"; break;
  }

  printf (op_format, get_argument_name (arg1), get_argument_name (arg2));
}

void print_blend_desc (const BlendDesc& desc)
{
  printf ("color_write_mask=%s", get_name ((ColorWriteFlag)desc.color_write_mask));
  
  if (desc.sample_alpha_to_coverage)
    printf (", sample_alpha_to_coverage");
    
  if (desc.blend_enable)
  {
    printf (", color_equation='");
    print_blend_equation (desc.blend_color_operation, desc.blend_color_source_argument, desc.blend_color_destination_argument);
    printf ("', alpha_equation='");
    print_blend_equation (desc.blend_alpha_operation, desc.blend_alpha_source_argument, desc.blend_alpha_destination_argument);
    printf ("'");
  }
  else
  {
    printf (", no_blend");
  }
}

void test_blend_state (const BlendDesc& desc, IDevice* device)
{
  try
  {
    device->OSSetBlendState (device->CreateBlendState (desc));

    if (output_mode & OutputMode_Success)
    {      
      printf ("Ok: ");      
      
      print_blend_desc (desc);
      
      printf ("\n");
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
      printf ("Fail: ");
      print_blend_desc (desc);
      printf (", exception: %s\n", exception.what ());
    }
  }
  
  total_tests++;
  
  fflush (stdout);
}

int main ()
{
  printf ("Results of all_blend_states test:\n");

  try
  {
    Test test (L"OpenGL device test window");
    
    output_mode = test.log_mode;
    
    BlendDesc desc;
    
    desc.blend_enable             = false;
    desc.sample_alpha_to_coverage = false;
    desc.color_write_mask         = ColorWriteFlag_All;

    desc.blend_color_operation            = BlendOperation_Add;
    desc.blend_color_source_argument      = BlendArgument_Zero;
    desc.blend_color_destination_argument = BlendArgument_Zero;    
    desc.blend_alpha_operation            = BlendOperation_Add;
    desc.blend_alpha_source_argument      = BlendArgument_Zero;
    desc.blend_alpha_destination_argument = BlendArgument_Zero;
    
    test_blend_state (desc, test.device.get ());
    
    desc.blend_enable = true;

    for (int csrc = 0; csrc < BlendArgument_Num; csrc++)
    {
      desc.blend_color_source_argument = (BlendArgument) csrc;
      
      for (int cdst = 0; cdst < BlendArgument_Num; cdst++)
      {
        desc.blend_color_destination_argument = (BlendArgument) cdst;

        for (int csrc = 0; csrc < BlendArgument_Num; csrc++)
        {
          desc.blend_color_source_argument = (BlendArgument) csrc;
          
          for (int cdst = 0; cdst < BlendArgument_Num; cdst++)
          {
            desc.blend_color_destination_argument = (BlendArgument) cdst;
            
            test_blend_state (desc, test.device.get ());
          }
        }
      }
    }

    printf ("Total tests count:   %u\n", total_tests);
    printf ("Success tests count: %u\n", success_tests);
    printf ("Wrong tests count:   %u\n", wrong_tests);
    printf ("Fail tests count:    %u\n", total_tests - success_tests - wrong_tests);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
