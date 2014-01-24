uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;
varying vec3 DirectLightDirection;

void main(void)
{
  vec4 view_pos  = ModelViewMatrix * gl_Vertex;
  vec3 eye_dir   = -view_pos.xyz;
  vec3 normal    = normalize (ModelViewMatrix * vec4 (gl_Normal, 0.0)).xyz;
  vec3 tangent   = normalize (ModelViewMatrix * vec4 (gl_MultiTexCoord3.xyz, 0.0)).xyz;
  vec3 binormal  = cross (normal, tangent);

  EyeDirection          = vec3 (dot (eye_dir, tangent), dot (eye_dir, binormal), dot (eye_dir, normal));
  ReflectionDirection   = reflect (eye_dir, normal);
  Normal                = gl_Normal;

  DiffuseTexcoord  = gl_MultiTexCoord0;
  BumpTexcoord     = gl_MultiTexCoord0;
  SpecularTexcoord = gl_MultiTexCoord0;

  vec3 light_dir = -(ViewMatrix * vec4 (0, 0, 1, 0)).xyz;

  DirectLightDirection = vec3 (dot (light_dir, tangent), dot (light_dir, binormal), dot (light_dir, normal));

  gl_FrontColor  = gl_Color;
  gl_Position    = ModelViewProjectionMatrix * gl_Vertex;
}                
