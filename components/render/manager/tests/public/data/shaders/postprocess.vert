#ifdef OPENGL_ES2_SUPPORT
precision highp float;
#endif

uniform mat4 ModelViewProjectionMatrix;

varying vec2 Texcoord;

attribute vec4 aVertex;
attribute vec3 aTexCoord0;

void main(void)
{
  Texcoord    = aTexCoord0.xy;

  gl_Position = ModelViewProjectionMatrix * aVertex;
}                
