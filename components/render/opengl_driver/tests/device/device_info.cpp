#include "shared.h"

const char* tostring (bool value)
{
  return value ? "true" : "false";
}

int main ()
{
  printf ("Results of device_info_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (device_info)");

    printf ("Device: '%s'\n", test.device->GetName ());

    IPropertyList* properties = test.device->GetProperties ();

    for (size_t j=0; j<properties->GetSize (); j++)
      printf ("  %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));
      
    printf ("Device capabilities:\n");      
    printf ("  shader_profiles: '%s'\n", test.device->GetCapString (DeviceCapString_ShaderProfiles));
    printf ("  texture_compression_formats: '%s'\n", test.device->GetCapString (DeviceCapString_TextureCompressionFormats));
    
    DeviceCaps caps;
    
    test.device->GetCaps (caps);
    
    printf ("  max_texture_1d_size:            %u\n", caps.max_texture_1d_size);
    printf ("  max_texture_2d_size:            %u\n", caps.max_texture_2d_size);
    printf ("  max_texture_3d_size:            %u\n", caps.max_texture_3d_size);
    printf ("  max_texture_cubemap_size:       %u\n", caps.max_texture_cubemap_size);
    printf ("  max_texture_anisotropy:         %u\n", caps.max_texture_anisotropy);
    printf ("  samplers_count:                 %u\n", caps.samplers_count);
    printf ("  has_advanced_blend:             %s\n", tostring (caps.has_advanced_blend));
    printf ("  has_depth_texture:              %s\n", tostring (caps.has_depth_texture));
    printf ("  has_multisample:                %s\n", tostring (caps.has_multisample));
    printf ("  has_occlusion_queries:          %s\n", tostring (caps.has_occlusion_queries));
    printf ("  has_shadow_maps:                %s\n", tostring (caps.has_shadow_maps));
    printf ("  has_texture_anisotropic_filter: %s\n", tostring (caps.has_texture_anisotropic_filter));
    printf ("  has_texture_cube_map:           %s\n", tostring (caps.has_texture_cube_map));    
    printf ("  has_texture_lod_bias:           %s\n", tostring (caps.has_texture_lod_bias));
    printf ("  has_texture_mirrored_repeat:    %s\n", tostring (caps.has_texture_mirrored_repeat));
    printf ("  has_texture_non_power_of_two:   %s\n", tostring (caps.has_texture_non_power_of_two));
    printf ("  has_texture1d:                  %s\n", tostring (caps.has_texture1d));
    printf ("  has_texture3d:                  %s\n", tostring (caps.has_texture3d));
    printf ("  has_two_side_stencil:           %s\n", tostring (caps.has_two_side_stencil));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
