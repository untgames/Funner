uniform mat4  ProjectionMatrix;
uniform mat4  ViewMatrix;
uniform vec3  ViewLightPosition;
uniform vec3  ViewLightDirection;
uniform float Reflectivity;
uniform float Transparency;
uniform float Shininess;
uniform float BumpAmount;
uniform float HighlightAmount;
uniform vec4  DiffuseColor;
uniform vec4  AmbientColor;
uniform vec4  SpecularColor;
uniform vec4  EmissionColor;
uniform vec3  ShadowColor;
uniform vec3  LightColor;

uniform int HasDiffuseTexture;
uniform int HasAmbientTexture;
uniform int HasBumpTexture;
uniform int HasSpecularTexture;
uniform int HasEmissionTexture;

uniform sampler2D   DiffuseTexture;
uniform sampler2D   BumpTexture;
uniform sampler2D   SpecularTexture;
uniform sampler2D   EmissionTexture;
uniform sampler2D   AmbientTexture;
uniform sampler2D   ReflectionTexture;
uniform sampler2D   ShadowTexture;
uniform samplerCube GlobalAmbientTexture;

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec4 ShadowTexcoord;
varying vec3 GlobalAmbientTexcoord;
varying vec3 LightDirection;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;

vec2 SphereMap (const in vec3 r)
{
  vec3 f = r;

  f.z += 1.0;
  f    = normalize (f);

  return f.xy*0.5 + 0.5;
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

vec3 ComputeEmissionColor ()
{
  vec3 color = vec3 (0.0);

  if (HasEmissionTexture != 0)
    color += vec3 (texture2D (EmissionTexture, DiffuseTexcoord.xy));

  return color;
}

vec3 ComputeSpecularColor (const in vec3 normal, const in vec3 light_dir, const in vec3 eye_dir, out vec3 tex_specular_color)
{
//#ifndef BLINN_SHADING
  float shininess = Shininess + 1.0;
//#else
//  float shininess = 1.0 / Shininess;
//#endif

  if (HasSpecularTexture != 0)
  {
    tex_specular_color = vec3 (texture2D (SpecularTexture, SpecularTexcoord.xy));
  }
  else
  {
    tex_specular_color = vec3 (0.0);
  }

  float specular_factor = pow (clamp (dot (reflect (-light_dir, normal), eye_dir), 0.0, 1.0), shininess);

  return (SpecularColor.rgb + tex_specular_color) * specular_factor;
}

void main (void)
{
//  float dist_sqr    = dot (LightDirection, LightDirection);
  float dist_sqr             = dot (EyeDirection, EyeDirection);
  float attenuation          = clamp (1.0 - sqrt (dist_sqr) / 6000.0, 0.0, 1.0);
  vec3  light_dir            = LightDirection;// * inversesqrt (dist_sqr);
  vec3  eye_dir              = normalize (EyeDirection);
  vec3  normal               = vec3 (0.0);
  float diffuse_transparency = 1.0;

  if (HasBumpTexture != 0)
  {
    normal = normalize (texture2D (BumpTexture, BumpTexcoord.xy).xyz * 2.0 - 1.0);
  }
  else
  {
    normal = Normal;
  }

  vec3 lighted_color = vec3 (0.0), tex_specular_color = vec3 (0.0);

  lighted_color += ComputeSpecularColor (normal, light_dir, eye_dir, tex_specular_color) * 0.4;
  lighted_color += ComputeReflectionColor (normal, eye_dir, tex_specular_color);
  lighted_color *= LightColor;
  lighted_color += ComputeEmissionColor ();
  
  lighted_color.r += HighlightAmount;

  vec3 tex_diffuse_color = vec3 (texture2D (DiffuseTexture, DiffuseTexcoord.xy));  
  vec3 self_color        = (EmissionColor.rgb + ShadowColor) * tex_diffuse_color;
  
  vec4 shadow_texcoord = ShadowTexcoord.xyzw / ShadowTexcoord.w;

  float shadow_depth = texture2D (ShadowTexture, shadow_texcoord.st).x + 0.005,
        shade        = 0.0;

  if (ShadowTexcoord.w > 0.0)
  {
    if (shadow_texcoord.x < 0.0 || shadow_texcoord.x > 1.0 || shadow_texcoord.y < 0.0 || shadow_texcoord.y > 1.0)
      shade = 1.0;
    else if (shadow_depth < shadow_texcoord.z)
      shade = 1.0 - (shadow_texcoord.z - shadow_depth);
  }

  vec3 color = mix (lighted_color, self_color, shade);
//  vec3 color = lighted_color;
    
  gl_FragColor = vec4 (color, Transparency * diffuse_transparency);
}
