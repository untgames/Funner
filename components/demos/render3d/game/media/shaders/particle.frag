uniform float Transparency;

uniform sampler2D DiffuseTexture;

varying vec4 DiffuseTexcoord;

void main (void)
{
  vec4 texel = texture2D (DiffuseTexture, DiffuseTexcoord.xy);

  gl_FragColor = vec4 (vec3 (texel) * vec3 (gl_Color), texel.w * gl_Color.w * Transparency);
}
