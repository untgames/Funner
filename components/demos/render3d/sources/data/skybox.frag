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

uniform samplerCube DiffuseTexture;

varying vec4 DiffuseTexcoord;

void main (void)
{
  gl_FragColor = textureCube (DiffuseTexture, DiffuseTexcoord.xyz);
}
