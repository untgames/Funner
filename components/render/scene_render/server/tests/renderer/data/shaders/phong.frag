#define SHADOW_MAP_PIXEL_SIZE 1.0 / 512.0

uniform float Reflectivity;
uniform float Shininess;
uniform float BumpAmount;
uniform vec4  DiffuseColor;
uniform vec4  AmbientColor;
uniform vec4  SpecularColor;
uniform vec4  EmissionColor;
uniform vec4  LightColor;

#define HasDiffuseTexture  1
#define HasAmbientTexture  0
#define HasBumpTexture     1
#define HasSpecularTexture 1
#define HasEmissionTexture 0

uniform sampler2D   DiffuseTexture;
uniform sampler2D   SpecularTexture;
uniform sampler2D   BumpTexture;
uniform sampler2D   ReflectionTexture;
uniform sampler2D   ShadowTexture;

varying vec3 Texcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 PointToLightDirection;
varying vec4 LightShadowTexcoord;

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

vec3 ComputeReflectionColor (const in vec3 reflection)
{
  vec2 reflection_texcoord = SphereMap (reflection);

  return vec3 (texture2D (ReflectionTexture, reflection_texcoord));
}

vec3 ComputeReflectionColor (const in vec3 normal, const in vec3 eye_dir, const in vec3 tex_specular_color)
{
  return ComputeReflectionColor (normalize (reflect (eye_dir, normal))) * Reflectivity * tex_specular_color;
}

float OffsetLookup(in sampler2D map, vec4 shadow_texcoord, vec2 offset)
{
  float shadow_depth = texture2D(map, shadow_texcoord.xy + offset * SHADOW_MAP_PIXEL_SIZE * shadow_texcoord.w).x + 0.00001;

  if (shadow_depth < shadow_texcoord.z)
    return shadow_texcoord.z - shadow_depth;
 
  return 1.0;
}

float PCF(in sampler2D shadow_texture, in vec4 shadow_texcoord)
{
  float sum = 0.0;
  float y = -1.5;

  const int STEPS_COUNT = 3;

  for (int i=0; i<STEPS_COUNT; i++, y += 1.5)
  { 
    float x = -1.5;

    for (int j=0; j<STEPS_COUNT; j++, x+= 1.5)
      sum += OffsetLookup(shadow_texture, shadow_texcoord, vec2(x, y));
  }

  return sum / float (STEPS_COUNT * STEPS_COUNT);
}

void main (void)
{
  float dist_sqr = dot (EyeDirection, EyeDirection);
  vec3  eye_dir  = vec3 (0, 0, 1);
  vec3  normal   = vec3 (0.0);

  if (HasBumpTexture != 0)
  {
    normal = normalize (texture2D (BumpTexture, Texcoord.xy).xyz * 2.0 - 1.0);
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
    vec4 diffuse_color = texture2D (DiffuseTexture, Texcoord.xy);

    tex_diffuse_color    += diffuse_color.xyz;
    diffuse_transparency  = diffuse_color.w;
  }

  if (HasSpecularTexture != 0)
  {
    tex_specular_color = vec3 (texture2D (SpecularTexture, Texcoord.xy));
  }

  vec3 color = vec3 (0.0);

  vec3 lighted_color = vec3 (0), light_dir = normalize (PointToLightDirection);

  lighted_color += ComputeDiffuseColor (normal, light_dir, tex_diffuse_color);
  lighted_color += ComputeSpecularColor (normal, light_dir, eye_dir, tex_specular_color);
//    lighted_color += ComputeReflectionColor (normal, eye_dir, tex_specular_color);
  lighted_color *= LightColor.xyz;

  color += lighted_color;
 
  vec4 shadow_texcoord = LightShadowTexcoord.xyzw / LightShadowTexcoord.w;

  float shade = 0.0;

  if (LightShadowTexcoord.w > 0.0)
  {
    if (shadow_texcoord.x < 0.0 || shadow_texcoord.x > 1.0 || shadow_texcoord.y < 0.0 || shadow_texcoord.y > 1.0)
    {
      shade = 1.0;
    }
    else
    {
      shade = PCF (ShadowTexture, shadow_texcoord);
    }
  }

  gl_FragColor = vec4 (lighted_color * shade, diffuse_transparency);
  gl_FragColor = vec4 (vec3 (shade), diffuse_transparency);

//  if (shade > 0.0)
//    gl_FragColor = vec4 (vec3 (1.0), diffuse_transparency);

//  gl_FragColor = vec4 (shadow_texcoord.xyz, diffuse_transparency);
}
