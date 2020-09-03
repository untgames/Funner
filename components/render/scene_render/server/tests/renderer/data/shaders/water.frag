uniform vec4 DiffuseColor;

uniform sampler2D DiffuseTexture;

varying vec3 DiffuseTexcoord;
varying vec3 Normal;

void main (void)
{
  vec4 diffuse_color = texture2D (DiffuseTexture, DiffuseTexcoord.xy);
   
  gl_FragColor = diffuse_color;
//  gl_FragColor = vec4 (Normal, 1.0);
}
