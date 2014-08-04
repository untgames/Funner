uniform vec4 DiffuseColor;

uniform sampler2D DiffuseTexture;

varying vec4 VertexColor;
varying vec3 DiffuseTexcoord;

void main (void)
{
  vec4 diffuse_color = texture2D (DiffuseTexture, DiffuseTexcoord.xy) * VertexColor;
   
  gl_FragColor = diffuse_color;
}
