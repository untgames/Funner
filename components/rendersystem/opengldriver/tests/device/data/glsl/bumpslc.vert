
//
// bumpslc.vert: Vertex shader for bump mapping in surface local coordinates
//
// author: Randi Rost
//
// Copyright (c) 2002: 3Dlabs, Inc.
//

varying vec3 lightDir; 	  // interpolated surface local coordinate light direction 
varying vec3 viewDir;     // interpolated surface local coordinate view direction

uniform vec3 LightPosition;  // eye space position of light
uniform mat4 Transform;

void main(void)
{
    vec3 b;
    vec3 n;
    vec3 t;
    vec3 pos;
    vec3 lightVec;
    vec3 r;
    vec3 v;

    // Do standard vertex stuff

//    gl_Position  = gl_ModelViewProjectionMatrix * Transform * gl_Vertex;
    gl_Position  = gl_ModelViewProjectionMatrix * gl_Vertex;
//    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[0] = vec4 (gl_Vertex.x + 1.0, gl_Vertex.y + 1.0, 0, 0) / 2.0;

    // Compute the binormal

    n = normalize(gl_NormalMatrix * gl_Normal);
    t = normalize(gl_NormalMatrix * vec3 (gl_Color));
    b = cross(n, t);

    // Transform light position into surface local coordinates

    vec3 lightPos = vec3 (Transform * vec4 (LightPosition, 1.0));

    v.x = dot(lightPos, t);
    v.y = dot(lightPos, b);
    v.z = dot(lightPos, n);

    lightDir = normalize(v);

//    pos      = vec3 (gl_ModelViewMatrix * Transform * gl_Vertex);
    pos      = vec3 (gl_ModelViewMatrix * gl_Vertex);

    v.x = dot(pos, t);
    v.y = dot(pos, b);
    v.z = dot(pos, n);

    viewDir = normalize(v);

}
