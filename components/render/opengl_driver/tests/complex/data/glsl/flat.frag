#ifdef OPENGL_ES2_SUPPORT
precision highp float;
#endif

uniform sampler2D DiffuseTexture;

varying vec2 Texcoord;
varying vec4 VertexColor;

void main (void)
{
  vec4 diffuse_color = texture2D (DiffuseTexture, Texcoord);
   
  gl_FragColor = diffuse_color * VertexColor;
}
