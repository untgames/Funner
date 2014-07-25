uniform mat4 ModelViewProjectionMatrix;

varying vec2 TexCoord;
varying vec4 VertexColor;

attribute vec4 aVertex;
attribute vec4 aColor;
attribute vec3 aTexCoord0;

void main(void)
{
  TexCoord    = aTexCoord0.xy;
  VertexColor = aColor;

  gl_Position = ModelViewProjectionMatrix * aVertex;
}                
