#define DIRECT_LIGHTS_COUNT 2

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

uniform vec4 DirectLightWorldDirection [DIRECT_LIGHTS_COUNT];

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec3 GlobalAmbientTexcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 DirectLightDirection [DIRECT_LIGHTS_COUNT];

void main(void)
{
  vec4 view_pos  = ModelViewMatrix * gl_Vertex;
  vec3 eye_dir   = -view_pos.xyz;
  vec3 normal    = normalize (ModelViewMatrix * vec4 (gl_Normal, 0.0)).xyz;
  vec3 tangent   = normalize (ModelViewMatrix * vec4 (gl_MultiTexCoord3.xyz, 0.0)).xyz;
  vec3 binormal  = cross (normal, tangent);

  GlobalAmbientTexcoord = -normalize (ObjectMatrix * gl_Vertex).xyz;
  EyeDirection          = vec3 (dot (eye_dir, tangent), dot (eye_dir, binormal), dot (eye_dir, normal));
  ReflectionDirection   = reflect (eye_dir, normal);
  Normal                = gl_Normal;

  DiffuseTexcoord  = gl_MultiTexCoord0;
  BumpTexcoord     = gl_MultiTexCoord0;
  SpecularTexcoord = gl_MultiTexCoord0;

  for (int i=0; i<DIRECT_LIGHTS_COUNT; i++)
  {
    vec3 light_dir = -(ViewMatrix * DirectLightWorldDirection [i]).xyz;

    DirectLightDirection [i] = vec3 (dot (light_dir, tangent), dot (light_dir, binormal), dot (light_dir, normal));
  }

  gl_FrontColor  = gl_Color;
  gl_Position    = ModelViewProjectionMatrix * gl_Vertex;
}                
