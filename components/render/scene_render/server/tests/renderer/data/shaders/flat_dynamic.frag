uniform vec4 DiffuseColor;

uniform sampler2D DiffuseTexture;

uniform float dynamicParam;

varying vec4 DiffuseTexcoord;

void main (void)
{
  vec4 diffuse_color = texture2D (DiffuseTexture, DiffuseTexcoord.xy);
   
  gl_FragColor = diffuse_color * gl_Color + vec4 (sin (dynamicParam));
}
