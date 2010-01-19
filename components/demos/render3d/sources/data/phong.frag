uniform mat4  ProjectionMatrix;
uniform mat4  ViewMatrix;
uniform mat4  ObjectMatrix;
uniform vec3  LightPosition;
uniform vec3  LightDirection;
uniform int   ShaderType;
uniform float Reflectivity;
uniform float Transparency;
uniform float Shininess;
uniform float BumpAmount;
uniform int   BumpTextureChannel;
uniform int   DiffuseTextureChannel;
uniform int   SpecularTextureChannel;
uniform int   AmbientTextureChannel;
uniform int   EmissionTextureChannel;
uniform vec4  DiffuseColor;
uniform vec4  AmbientColor;
uniform vec4  SpecularColor;
uniform vec4  EmissionColor;

uniform sampler2D DiffuseTexture;
uniform sampler2D BumpTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D EmissionTexture;
uniform sampler2D AmbientTexture;
uniform sampler2D ReflectionTexture;

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec3 Normal;
varying vec3 LocalLightDirection;
varying vec3 EyeLightDirection;
varying vec3 EyeDirection;

vec2 SphereMap (const in vec3 r)
{
  vec3 f = r;

  f.z += 1.0;
  f    = normalize (f);

  return f.xy*0.5 + 0.5;
}

vec3 ComputeDiffuseColor ()
{
  return vec3 (texture2D (DiffuseTexture, DiffuseTexcoord.xy)) + DiffuseColor.rgb;
}

vec3 ComputeSpecularColor ()
{
  return vec3 (texture2D (SpecularTexture, SpecularTexcoord.xy)) + SpecularColor.rgb;
}

vec3 ComputeReflectionColor (const in vec3 normal)
{
  vec3 reflection          = reflect (EyeDirection, normal);  
  vec2 reflection_texcoord = SphereMap (reflection);

  reflection_texcoord.y = 1.0 - reflection_texcoord.y;

  return vec3 (texture2D (ReflectionTexture, reflection_texcoord));
}

void main (void)
{
  vec3 normal;

  normal   = vec3 (texture2D (BumpTexture, BumpTexcoord.xy));
  normal   = (normal - 0.5) * 2.0;
//    normal.y = -normal.y;
  normal   = normalize (Normal + BumpAmount * (normal - vec3 (0,0,1)));

  float diffuse_factor  = max (dot (LocalLightDirection, normal), 0.0);
  float specular_factor = pow (max (dot (reflect (EyeLightDirection, normal), EyeDirection), 0.0), Shininess);

  vec3 color = ComputeDiffuseColor () * diffuse_factor + ComputeSpecularColor () * specular_factor +
               ComputeReflectionColor (normal) * Reflectivity;

  gl_FragColor = vec4 (color, 1.0);
}
