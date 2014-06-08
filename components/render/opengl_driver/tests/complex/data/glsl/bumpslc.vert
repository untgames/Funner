
//
// bumpslc.vert: Vertex shader for bump mapping in surface local coordinates
//
// author: Randi Rost
//
// Copyright (c) 2002: 3Dlabs, Inc.
//

varying vec3 lightDir; 	  // interpolated surface local coordinate light direction 
varying vec3 viewDir;     // interpolated surface local coordinate view direction
varying vec4 texcoord;

uniform vec3 LightPosition;  // eye space position of light
uniform mat4 Transform;

attribute vec4 aVertex;
attribute vec3 aNormal;
attribute vec4 aColor;

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

    gl_Position  = aVertex;

    texcoord = vec4 (aVertex.x + 1.0, aVertex.y + 1.0, 0, 0) / 2.0;

    // Compute the binormal

    n = normalize(aNormal);
    t = normalize(vec3 (aColor));
    b = cross(n, t);

    // Transform light position into surface local coordinates

    vec3 lightPos = vec3 (Transform * vec4 (LightPosition, 1.0));

    v.x = dot(lightPos, t);
    v.y = dot(lightPos, b);
    v.z = dot(lightPos, n);

    lightDir = normalize(v);

    pos      = aVertex.xyz;

    v.x = dot(pos, t);
    v.y = dot(pos, b);
    v.z = dot(pos, n);

    viewDir = normalize(v);
}
