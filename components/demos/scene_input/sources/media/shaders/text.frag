uniform vec4 DiffuseColor;

uniform sampler2D FontTexture;

varying vec4 DiffuseTexcoord;

void main (void)
{
  vec3 font_color = texture2D (FontTexture, DiffuseTexcoord.xy).xyz;
   
  gl_FragColor = vec4 (DiffuseColor.xyz, font_color.x);
}
