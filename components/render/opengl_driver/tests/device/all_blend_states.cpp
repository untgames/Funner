#include "shared.h"

OutputMode Mode = OutputMode_Default;

void print_blend_desc(const BlendDesc& desc)
{
  printf("BlendDesc parameters:\n");
  printf(" Blend enabled: %s\n", desc.blend_enable ? "yes" : "no");
  printf(" Sample alpha to coverage: %s\n", desc.sample_alpha_to_coverage ? "yes" : "no");
  printf(" Color write mask: %02X\n", desc.color_write_mask);  
  printf("=====================================================================================================================================\n");
  printf(" Channel | Operation                         | Source argument                       | Destination argument                  \n");
  printf("-------------------------------------------------------------------------------------------------------------------------------------\n");
  printf(" Color   | %-33s | %-37s | %-37s \n",
    get_name(desc.blend_color_operation), get_name(desc.blend_color_source_argument), get_name(desc.blend_color_destination_argument));
  printf(" Alpha   | %-33s | %-37s | %-37s \n",
    get_name(desc.blend_alpha_operation), get_name(desc.blend_alpha_source_argument), get_name(desc.blend_alpha_destination_argument));
  printf("=====================================================================================================================================\n");
  fflush(stdout);
}

void test_blend_state(const BlendDesc& desc, IDevice* device)
{
  try
  {
    device->OSSetBlendState(device->CreateBlendState(desc));
    if (Mode != OutputMode_FailOnly)
    {
      print_blend_desc(desc);
      printf("Testing... OK\n");
    }
  }
  catch (std::exception& exception)
  {
    if (Mode != OutputMode_SuccessOnly)
    {
      print_blend_desc(desc);
      printf ("Testing... FAIL! exception: %s\n", exception.what ());
    }
  }
}

int main ()
{
  printf ("Results of all_blend_states test:\n");

  try
  {
    Test test (L"OpenGL device test window (all_blend_states test)");
    
    Mode = test.log_mode;
    
    BlendDesc desc;
    
    desc.blend_enable = true;
    desc.sample_alpha_to_coverage = false;
    desc.color_write_mask = 0xFF;
    
    desc.blend_color_operation = BlendOperation_Add;
    desc.blend_color_source_argument = BlendArgument_Zero;
    desc.blend_color_destination_argument = BlendArgument_Zero;
    
    desc.blend_alpha_operation = BlendOperation_Add;
    desc.blend_alpha_source_argument = BlendArgument_Zero;
    desc.blend_alpha_destination_argument = BlendArgument_Zero;
    
    for (int blend = 0; blend < 2; blend++)
    {
      desc.blend_enable = (bool) blend;
      
      for (int sample = 0; sample < 2; sample++)
      {
        desc.sample_alpha_to_coverage = (bool) sample;
      
        for (int cop = 0; cop < BlendOperation_Num; cop++)
        {
          desc.blend_color_operation = (BlendOperation) cop;
          
          for (int csrc = 0; csrc < BlendArgument_Num; csrc++)
          {
            desc.blend_color_source_argument = (BlendArgument) csrc;
            
            for (int cdst = 0; cdst < BlendArgument_Num; cdst++)
            {
              desc.blend_color_destination_argument = (BlendArgument) cdst;
             
              for (int cop = 0; cop < BlendOperation_Num; cop++)
              {
                desc.blend_color_operation = (BlendOperation) cop;
                
                for (int csrc = 0; csrc < BlendArgument_Num; csrc++)
                {
                  desc.blend_color_source_argument = (BlendArgument) csrc;
                  
                  for (int cdst = 0; cdst < BlendArgument_Num; cdst++)
                  {
                    desc.blend_color_destination_argument = (BlendArgument) cdst;
                    
                    test_blend_state(desc, test.device.get());
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
