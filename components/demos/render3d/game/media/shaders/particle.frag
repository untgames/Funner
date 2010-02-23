uniform float Transparency;

uniform sampler2D DiffuseTexture;

varying vec4 DiffuseTexcoord;

void main (void)
{
  gl_FragColor = vec4 (vec3 (texture2D (DiffuseTexture, DiffuseTexcoord.xy)), gl_Color.w * Transparency);
}
