uniform mat4 ModelViewProjectionMatrix;

varying vec4 DiffuseTexcoord;

void main(void)
{
  DiffuseTexcoord = gl_MultiTexCoord0;

  gl_FrontColor  = gl_Color;
  gl_Position    = ModelViewProjectionMatrix * gl_Vertex;
}                
