uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform vec4 LightWorldPosition;
uniform vec4 LightWorldDirection;
uniform mat4 LightShadowMatrix;

varying vec3 Texcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 PointToLightDirection;
varying vec4 LightShadowTexcoord;

attribute vec4 aVertex;
attribute vec3 aNormal;
attribute vec4 aColor;
attribute vec3 aTexCoord0;
attribute vec3 aTexCoord1;

void main(void)
{
  vec4 view_pos  = ModelViewMatrix * aVertex;
  vec3 eye_dir   = -view_pos.xyz;
  vec3 normal    = normalize (ModelViewMatrix * vec4 (aNormal, 0.0)).xyz;
  vec3 tangent   = normalize (ModelViewMatrix * vec4 (aTexCoord1.xyz, 0.0)).xyz;
  vec3 binormal  = cross (normal, tangent);

  EyeDirection          = vec3 (dot (eye_dir, tangent), dot (eye_dir, binormal), dot (eye_dir, normal));
  ReflectionDirection   = reflect (eye_dir, normal);
  Normal                = normal;

  Texcoord = aTexCoord0;

  vec3 light_dir = normalize ((ViewMatrix * vec4 (LightWorldPosition.xyz, 1.0)).xyz - view_pos.xyz);

  PointToLightDirection = vec3 (dot (light_dir, tangent), dot (light_dir, binormal), dot (light_dir, normal));

  LightShadowTexcoord = LightShadowMatrix * aVertex;

  gl_FrontColor  = aColor;
  gl_Position    = ModelViewProjectionMatrix * aVertex;
}                
