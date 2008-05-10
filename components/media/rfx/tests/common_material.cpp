#include "shared.h"

template <size_t Size>
void dump (const vec<float, Size>& v)
{
  printf ("[");
  
  for (size_t i=0; i<Size; i++)
    printf (" %g", v [i]);
    
  printf (" ]");
}

template <size_t Size>
void dump (const matrix<float, Size>& m)
{
  printf ("[");
  
  for (size_t i=0; i<Size; i++)
    dump (m [i]);
    
  printf (" ]");
}

void dump (const Texmap& texmap)
{
  printf ("    image: '%s'\n", texmap.Image ());
  printf ("    transform: ");
  dump   (texmap.Transform ());
  printf ("\n");
  
  for (size_t i=0; i<Texcoord_Num; i++)
  {
    static const char* texcoord_name [] = {"s", "t", "q"};    
    
    printf ("    texcoord_%s: ", texcoord_name [i]);
    printf ("source=");
  
    switch (texmap.Source ((Texcoord)i))
    {
      case TexcoordSource_SphereMap:     printf ("sphere_map"); break;
      case TexcoordSource_ReflectionMap: printf ("reflection_map"); break;
      default:                           printf ("channel%d", texmap.Source ((Texcoord)i)); break;
    }

    static const char* wrap_name [] = {"repeat", "mirror", "clamp", "clamp_to_border"};

    printf (" wrap=%s\n", wrap_name [texmap.Wrap ((Texcoord)i)]);
  }
  
  for (size_t i=0; i<TexmapFilter_Num; i++)
  {
    static const char* filter_prefix [] = {"min", "mag", "mip"};
    static const char* filter_type_name [] = {"default", "point", "bilinear", "trilinear", "anisotropic"};
    
    printf ("    %s_filter: %s\n", filter_prefix [i], filter_type_name [texmap.FilterType ((TexmapFilter)i)]);
  }
}

void dump (const CommonMaterial& shader)
{
  printf ("Common material '%s'\n", shader.Name ());
  printf ("  type: ");

  switch (shader.ShaderType ())
  {
    case CommonMaterialShaderType_Flat:    printf ("flat"); break;
    case CommonMaterialShaderType_Gourand: printf ("gourand"); break;
    case CommonMaterialShaderType_Phong:   printf ("phong"); break;
    default:                       printf ("unknown"); break;
  }
  
  printf ("\n");
  
  static const char* color_name [] = {"ambient", "diffuse", "specular", "emission"};
  
  for (size_t i=0; i<CommonMaterialColor_Num; i++)
  {
    printf ("  color '%s': ", color_name [i]);
    dump   (shader.Color ((CommonMaterialColor)i));
    printf ("\n");
  }
  
  printf ("  shininess: %g\n", shader.Shininess ());
  printf ("  transparency: %g\n", shader.Transparency ());
  
  static const char* blend_equation_format [] = {
    "source.rgba * %s + destination.rgba * %s",
    "source.rgba * %s - destination.rgba * %s",
    "destination.rgba * %s - source.rgba * %s",
    "min (source.rgba * %s, destination.rgba * %s)",
    "max (source.rgba * %s, destination.rgba * %s)"
  };
  
  static const char* blend_argument_name [] = {"0", "1", "source.rgb", "source.alpha", "(1 - source.rgb)", 
                                               "(1 - source.alpha)", "destination.rgb", "destination.alpha",
                                               "(1 - destination.rgb)", "(1 - destination.alpha)"};
  
  printf ("  blend: ");
  printf (blend_equation_format [shader.Blend ().equation], blend_argument_name [shader.Blend ().argument [0]],
          blend_argument_name [shader.Blend ().argument [1]]);
  printf ("\n");
  
  static const char* alpha_test_format [] = {"fail", "pass", "alpha == %g", "alpha != %g", "alpha < %g", "alpha <= %g",
                                             "alpha > %g", "alpha >= %g"};

  printf ("  alpha_test: ");
  printf (alpha_test_format [shader.AlphaTestMode ()], shader.AlphaTestReference ());
  printf ("\n");

  static const char* map_name [] = {"diffuse", "ambient", "specular", "transparent", "emission", "reflective", "bump"};
  
  for (size_t i=0; i<CommonMaterialMap_Num; i++)
  {        
    printf ("  texmap '%s'\n", map_name [i]);    
    printf ("    state: %s\n", shader.IsMapEnabled ((CommonMaterialMap)i) ? "enabled" : "disabled");
    printf ("    weight: %g\n", shader.MapWeight ((CommonMaterialMap)i));
    dump   (shader.Map ((CommonMaterialMap)i));
  }  
}

int main ()
{
  printf ("Results of common_material_test:\n");
  
  CommonMaterial::Pointer shader = CommonMaterial::Create ();
  
  shader->Rename ("shader1");
  shader->SetColor (CommonMaterialColor_Emission, 0.2f, 1.2f, -2.0f);
  shader->SetShininess (320);
  shader->SetTransparency (0.4f);
  shader->SetBlend (make_blend_filter ()); 
  shader->SetAlphaTest (CompareMode_LessEqual, 1.3f);
  
  shader->EnableMap (CommonMaterialMap_Transparent);
  shader->SetMapWeight (CommonMaterialMap_Transparent, -0.5f);

  Texmap& texmap = shader->Map (CommonMaterialMap_Transparent);

  texmap.SetTransform  (translatef (1.0f, 0.5f, 0.0f));
  texmap.SetImage      ("image1");
  texmap.SetWrap       (Texcoord_T, TexcoordWrap_ClampToBorder);
  texmap.SetSource     (Texcoord_Q, TexcoordSource_ReflectionMap);
  texmap.SetFilterType (TexmapFilter_Min, TexmapFilterType_Point);
  texmap.SetFilterType (TexmapFilter_Mag, TexmapFilterType_Anisotropic);
  texmap.SetFilterType (TexmapFilter_Mip, TexmapFilterType_Bilinear);
  
  dump (*shader);

  return 0;
}
