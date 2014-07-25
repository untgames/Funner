#ifdef OPENGL_ES2_SUPPORT
precision highp float;
precision highp sampler2D;
#endif

uniform sampler2D FontTexture;

varying vec2 TexCoord;
varying vec4 VertexColor;

void main (void)
{
  vec4 font_color = texture2D (FontTexture, TexCoord.xy);
   
  gl_FragColor = font_color * VertexColor;
}
