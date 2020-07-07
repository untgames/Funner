#ifdef OPENGL_ES2_SUPPORT
precision highp float;
#endif

uniform sampler2D DiffuseRTexture;
uniform sampler2D DiffuseGBTexture;

varying vec2 Texcoord;

void main (void)
{
  vec4 diffuse_r_color  = texture2D (DiffuseRTexture, Texcoord);
  vec4 diffuse_gb_color = texture2D (DiffuseGBTexture, Texcoord);
   
  gl_FragColor = (diffuse_r_color + diffuse_gb_color);// * 0.5;
}
