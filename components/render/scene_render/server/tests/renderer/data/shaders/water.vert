uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

varying vec3 DiffuseTexcoord;
varying vec3 Normal;

attribute vec4 aVertex;
attribute vec3 aNormal;
attribute vec4 aColor;
attribute vec3 aTexCoord0;

void main(void)
{
  Normal = normalize (ModelViewMatrix * vec4 (aNormal, 0.0)).xyz;

  DiffuseTexcoord = aTexCoord0;

  gl_FrontColor  = aColor;
  gl_Position    = ModelViewProjectionMatrix * aVertex;
}                
