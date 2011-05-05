uniform mat4 ViewMatrix;
uniform mat4 ObjectMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform mat4  ShadowMatrix;
uniform vec3 ViewLightPosition;
uniform vec3 ViewLightDirection;

varying vec4 DiffuseTexcoord;
varying vec4 BumpTexcoord;
varying vec4 SpecularTexcoord;
varying vec4 ShadowTexcoord;
varying vec3 GlobalAmbientTexcoord;
varying vec3 LightDirection;
varying vec3 EyeDirection;
varying vec3 ReflectionDirection;
varying vec3 Normal;

void main(void)
{
  vec4 view_pos  = ModelViewMatrix * gl_Vertex;
  vec3 light_dir = normalize ((ViewMatrix * vec4 (-391.0,446.0,-325.0,0)).xyz);//normalize (view_pos.xyz - ViewLightPosition);
//  vec3 light_dir = ViewLightDirection;
  vec3 eye_dir   = -view_pos.xyz;
  vec3 normal    = normalize (ModelViewMatrix * vec4 (gl_Normal, 0.0)).xyz;
  vec3 tangent   = normalize (ModelViewMatrix * vec4 (gl_MultiTexCoord3.xyz, 0.0)).xyz;
  vec3 binormal  = cross (normal, tangent);

  GlobalAmbientTexcoord = -normalize (ObjectMatrix * gl_Vertex).xyz;
  LightDirection        = -vec3 (dot (light_dir, tangent), dot (light_dir, binormal), dot (light_dir, normal));
  EyeDirection          = vec3 (dot (eye_dir, tangent), dot (eye_dir, binormal), dot (eye_dir, normal));
  ReflectionDirection   = reflect (eye_dir, normal);
  Normal                = normal;

  DiffuseTexcoord  = gl_MultiTexCoord0;
  BumpTexcoord     = gl_MultiTexCoord0;
  SpecularTexcoord = gl_MultiTexCoord0;
  ShadowTexcoord   = ShadowMatrix * gl_Vertex;

  gl_FrontColor = gl_Color;
  gl_Position   = ModelViewProjectionMatrix * gl_Vertex;
}                
