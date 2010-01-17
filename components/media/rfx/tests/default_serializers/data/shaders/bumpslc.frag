const int TEX_DIFFUSE_MAP  = 0;
const int TEX_REFLECT_MAP  = 1;
const int TEX_NORMAL_MAP   = 2;
const int TEX_OPACITY_MAP  = 3;
const int TEX_SHADOW_MAP   = 4;

const float diffuseFactor  = 1.0;
const float bumpFactor     = 100.0;
const float specularFactor = 1.0;
const float ambientFactor  = 0.4;
const float emissionFactor = 1.0;

#ifdef ENABLE_SHADOW
  uniform vec3  shadow_color;
  uniform float shadow_density;
  varying vec4  shadow_tc;
  uniform sampler2D shadow_map; 
#endif

#ifdef ENABLE_DIFFUSE
  uniform float diffuse_map_amount;
  varying vec4 diff_tc;
  uniform sampler2D diffuse_map;
#endif

#ifdef ENABLE_BUMP
  uniform float bump_map_amount;
  varying vec4 bump_tc;
  uniform sampler2D normal_map;
#endif

#ifdef ENABLE_REFLECTION
  uniform float reflect_map_amount;
  uniform sampler2D reflect_map; 
#endif

#ifdef ENABLE_OPACITY
  uniform float opacity_map_amount;
  uniform sampler2D opacity_map; 
  varying vec4 opac_tc;
#endif

varying vec3 normal;
varying vec3 eye;
varying vec3 light;
varying vec3 lightDir;

vec2 SphereMap (const in vec3 r)
{
  vec3 f = r;

  f.z += 1.0;
  f    = normalize (f);

  return f.xy*0.5 + 0.5;
}

void main (void)
{
    vec3  color = vec3 (0);
    float alpha = 1.0;
    vec3  norm;
    vec3  diff_color;

#ifdef ENABLE_BUMP
    norm   = vec3(texture2D(normal_map,bump_tc.xy));
    norm   = (norm - 0.5) * 2.0;    
//    norm.y = -norm.y;
    norm   = normalize (normal+bump_map_amount*bumpFactor*(norm-vec3(0,0,1)));
#else
    norm = normal;
#endif

    float angle = dot (lightDir,light);
    float diff  = max (dot (lightDir,norm),0.0)*diffuseFactor;
    float spec  = min (pow (max(dot (reflect (light,norm),eye),0.0),gl_FrontMaterial.shininess),1.0)*specularFactor;

/*    if (angle < gl_LightSource [0].spotCosCutoff)
    {
      diff = 0.0;
      spec = 0.0;
    } 
    else
      diff *= pow (angle,gl_LightSource [0].spotExponent);
*/

#ifdef ENABLE_DIFFUSE
    diff_color = vec3 (texture2D(diffuse_map,diff_tc.xy))*diffuse_map_amount;
#else
    diff_color = vec3 (1,1,1);
#endif

#ifdef ENABLE_REFLECTION
    vec3 r  = reflect (eye,norm);  
    vec2 refl_tc = SphereMap (r);
           
//    refl_tc.y = 1.0 - refl_tc.y;

    diff_color += vec3 (texture2D(reflect_map,refl_tc))*reflect_map_amount;
#endif

    vec3 self_color    = vec3 (0.0);
    vec3 lighted_color = vec3 (0.0);

    self_color += gl_FrontMaterial.emission.xyz * emissionFactor;
    self_color += gl_LightModel.ambient.xyz;

    lighted_color += gl_LightSource [0].ambient.xyz  * gl_FrontMaterial.ambient.xyz  * ambientFactor;
    lighted_color += gl_LightSource [0].diffuse.xyz  * gl_FrontMaterial.diffuse.xyz  * diff_color * diff;
    lighted_color += gl_LightSource [0].specular.xyz * gl_FrontMaterial.specular.xyz * spec;

#ifdef ENABLE_SHADOW
    vec3 tc   = shadow_tc.xyz;
    vec4 shad = texture2D (shadow_map,tc.xy); 

    if (shad.z < tc.z) 
    {
      color = mix(lighted_color,self_color+shadow_color,shadow_density*(1.0-tc.z+shad.z));
    }
    else color += self_color + lighted_color;
#else
    color += self_color + lighted_color;
#endif

#ifdef ENABLE_OPACITY
      alpha = texture2D (opacity_map,opac_tc.xy).a;
#endif

    gl_FragColor = vec4 (clamp (color,0.0,1.0),alpha);
}
