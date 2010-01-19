uniform mat4  ModelViewMatrix;
uniform mat4  ModelViewProjectionMatrix;
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

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec3 Normal;
varying vec3 LocalLightDirection;
varying vec3 EyeLightDirection;
varying vec3 EyeDirection;

void main(void)
{
  vec4 pos = ModelViewMatrix * gl_Vertex;

  Normal              = vec3 (normalize (ModelViewMatrix * vec4 (gl_Normal, 0.0)));
  EyeDirection        = normalize (pos.xyz);
  EyeLightDirection   = normalize (LightPosition - pos.xyz);
  LocalLightDirection = -normalize (LightDirection);

  DiffuseTexcoord  = gl_MultiTexCoord0;
  BumpTexcoord     = gl_MultiTexCoord1;
  SpecularTexcoord = gl_MultiTexCoord2;

  gl_FrontColor = gl_Color;
  gl_Position   = ModelViewProjectionMatrix * gl_Vertex;
}                
