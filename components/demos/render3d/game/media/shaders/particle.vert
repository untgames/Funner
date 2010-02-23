uniform mat4  ViewMatrix;
uniform mat4  ModelViewProjectionMatrix;
uniform float Transparency;
uniform int   DiffuseTextureChannel;

varying vec4 DiffuseTexcoord;

void main(void)
{
  DiffuseTexcoord  = gl_MultiTexCoord0;

  gl_FrontColor = gl_Color;
  gl_Position   = ModelViewProjectionMatrix * gl_Vertex;
}                
