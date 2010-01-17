const int TEX_DIFFUSE_MAP = 0;
const int TEX_REFLECT_MAP = 1;
const int TEX_NORMAL_MAP  = 2;
const int TEX_OPACITY_MAP = 3;
const int TEX_SHADOW_MAP  = 4;

varying vec3 normal;
varying vec3 eye;
varying vec3 light;
varying vec3 lightDir;

#ifdef ENABLE_SHADOW
  varying vec4 shadow_tc;
  uniform mat4 shadow_tm;
#endif

#ifdef ENABLE_DIFFUSE
  varying vec4 diff_tc;
  uniform mat4 diffuse_tm;
#endif

#ifdef ENABLE_BUMP
  varying vec4 bump_tc;
  uniform mat4 bump_tm;
#endif

#ifdef ENABLE_OPACITY
  varying vec4 opac_tc;
  uniform mat4 opacity_tm;
#endif

void main(void)
{
    vec4 pos = gl_ModelViewMatrix * gl_Vertex;

    normal   = normalize (gl_NormalMatrix * gl_Normal);
    eye      = normalize (pos.xyz);
    light    = normalize (gl_LightSource[0].position.xyz - pos.xyz);
    lightDir = -normalize (gl_LightSource[0].spotDirection);

#ifdef ENABLE_DIFFUSE
    diff_tc = diffuse_tm * gl_MultiTexCoord0;
#endif

#ifdef ENABLE_BUMP
    bump_tc = bump_tm * gl_MultiTexCoord0;
#endif

#ifdef ENABLE_OPACITY
    opac_tc = opacity_tm * gl_MultiTexCoord0;
#endif

#ifdef ENABLE_SHADOW
    shadow_tc.x = dot (pos,gl_EyePlaneS [TEX_SHADOW_MAP]);
    shadow_tc.y = dot (pos,gl_EyePlaneT [TEX_SHADOW_MAP]);
    shadow_tc.z = dot (pos,gl_EyePlaneR [TEX_SHADOW_MAP]);
    shadow_tc.w = dot (pos,gl_EyePlaneQ [TEX_SHADOW_MAP]);

    shadow_tc  = shadow_tm * shadow_tc;
    shadow_tc /= shadow_tc.w;
#endif

    gl_FrontColor = gl_Color;    
    gl_Position   = ftransform ();
}                
