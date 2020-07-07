#ifdef OPENGL_ES2_SUPPORT
precision highp float;
#endif

uniform sampler2D DiffuseTexture;

varying vec2 Texcoord;

void main (void)
{
  vec4 diffuse_color = texture2D (DiffuseTexture, Texcoord);

#ifdef ONLY_R
  gl_FragColor = vec4 (diffuse_color.r, 0, 0, 0);
#elif defined (ONLY_GB)
  gl_FragColor = vec4 (0, diffuse_color.g, diffuse_color.b, 0);
#else
  gl_FragColor = diffuse_color;
#endif
}
