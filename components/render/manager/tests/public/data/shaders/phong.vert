#define DIRECT_LIGHTS_COUNT 2

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

uniform vec4 DirectLightWorldDirection [DIRECT_LIGHTS_COUNT];

varying vec3 DiffuseTexcoord;
varying vec3 BumpTexcoord;
varying vec3 SpecularTexcoord;
varying vec3 GlobalAmbientTexcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 DirectLightDirection [DIRECT_LIGHTS_COUNT];

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
  vec3 tangent   = normalize (ModelViewMatrix * vec4 (aTexCoord1, 0.0)).xyz;
  vec3 binormal  = cross (normal, tangent);

  GlobalAmbientTexcoord = -normalize (ObjectMatrix * aVertex).xyz;
  EyeDirection          = vec3 (dot (eye_dir, tangent), dot (eye_dir, binormal), dot (eye_dir, normal));
  ReflectionDirection   = reflect (eye_dir, normal);
  Normal                = aNormal;

  DiffuseTexcoord  = aTexCoord0;
  BumpTexcoord     = aTexCoord0;
  SpecularTexcoord = aTexCoord0;

  for (int i=0; i<DIRECT_LIGHTS_COUNT; i++)
  {
    vec3 light_dir = -(ViewMatrix * DirectLightWorldDirection [i]).xyz;

    DirectLightDirection [i] = vec3 (dot (light_dir, tangent), dot (light_dir, binormal), dot (light_dir, normal));
  }

  gl_FrontColor  = aColor;
  gl_Position    = ModelViewProjectionMatrix * aVertex;
}                
