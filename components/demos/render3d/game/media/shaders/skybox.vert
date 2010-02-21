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

void main(void)
{
  DiffuseTexcoord = normalize (gl_Vertex);
  gl_Position     = ModelViewProjectionMatrix * gl_Vertex;
}                
