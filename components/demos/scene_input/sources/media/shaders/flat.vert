uniform mat4 ModelViewProjectionMatrix;

varying vec3 DiffuseTexcoord;
varying vec4 VertexColor;

attribute vec4 aVertex;
attribute vec3 aTexCoord0;
attribute vec4 aColor;

void main(void)
{
  DiffuseTexcoord = aTexCoord0;
  VertexColor     = aColor;

  gl_Position    = ModelViewProjectionMatrix * aVertex;
}                
