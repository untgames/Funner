uniform mat4  ProjectionMatrix;
uniform mat4  ViewMatrix;
uniform mat4  ObjectMatrix;
uniform vec4  LightPosition;
uniform vec4  LightDirection;
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

varying vec4 DiffuseTexcoord;

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

void main (void)
{
  vec3 color = ComputeDiffuseColor ();

  gl_FragColor = vec4 (color, 1.0);
}
