#define DIRECT_LIGHTS_COUNT 2

uniform float Reflectivity;
uniform float Transparency;
uniform float Shininess;
uniform float BumpAmount;
uniform vec4  DiffuseColor;
uniform vec4  AmbientColor;
uniform vec4  SpecularColor;
uniform vec4  EmissionColor;

uniform vec4 DirectLightColor [DIRECT_LIGHTS_COUNT];

#define HasDiffuseTexture  1
#define HasAmbientTexture  0
#define HasBumpTexture     1
#define HasSpecularTexture 1
#define HasEmissionTexture 0

uniform sampler2D   DiffuseTexture;
uniform sampler2D   BumpTexture;
uniform sampler2D   SpecularTexture;

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec3 GlobalAmbientTexcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 DirectLightDirection [DIRECT_LIGHTS_COUNT];

vec2 SphereMap (const in vec3 r)
{
  vec3 f = r;

  f.z += 1.0;
  f    = normalize (f);

  return f.xy*0.5 + 0.5;
}

vec3 ComputeDiffuseColor (const in vec3 normal, const in vec3 light_dir, const in vec3 tex_diffuse_color)
{
  return (tex_diffuse_color + DiffuseColor.rgb) * max (dot (light_dir, normal), 0.2);
}

vec3 ComputeSpecularColor (const in vec3 normal, const in vec3 light_dir, const in vec3 eye_dir, const in vec3 tex_specular_color)
{
  float shininess = Shininess + 1.0;

  float specular_factor = pow (clamp (dot (reflect (-light_dir, normal), eye_dir), 0.0, 1.0), shininess);

  return (SpecularColor.rgb + tex_specular_color) * specular_factor;
}

void main (void)
{
  float dist_sqr = dot (EyeDirection, EyeDirection);
  vec3  eye_dir  = normalize (EyeDirection);
  vec3  normal   = vec3 (0.0);

  if (HasBumpTexture != 0)
  {
    normal = normalize (texture2D (BumpTexture, BumpTexcoord.xy).xyz * 2.0 - 1.0);
  }
  else
  {
    normal = Normal;
  }

  vec3 tex_specular_color = vec3 (0.0),
       tex_diffuse_color  = vec3 (0.0);

  float diffuse_transparency = 1.0;

  if (HasDiffuseTexture != 0)
  {
    vec4 diffuse_color = texture2D (DiffuseTexture, DiffuseTexcoord.xy);

    tex_diffuse_color += diffuse_color.xyz;
    diffuse_transparency = diffuse_color.w;
  }

  if (HasSpecularTexture != 0)
  {
    tex_specular_color = vec3 (texture2D (SpecularTexture, SpecularTexcoord.xy));
  }

  vec3 color = vec3 (0.0);

  for (int i=0; i<DIRECT_LIGHTS_COUNT; i++)
  {
    vec3 lighted_color = vec3 (0);
    vec3 light_dir     = DirectLightDirection [i];

    lighted_color += ComputeDiffuseColor (normal, light_dir, tex_diffuse_color);
    lighted_color += ComputeSpecularColor (normal, light_dir, eye_dir, tex_specular_color);
    lighted_color *= DirectLightColor [i].rgb;

    color += lighted_color;
  }
    
  gl_FragColor = vec4 (color, Transparency * diffuse_transparency);
}
