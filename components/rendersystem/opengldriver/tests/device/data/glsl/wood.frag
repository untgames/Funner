
//
// wood.frag: Simple fragment shader for wood
//
// author: John Kessenich
//
// Copyright (c) 2002: 3Dlabs, Inc.
//

uniform float GrainSizeRecip;
uniform vec3  DarkColor;
uniform vec3  colorSpread;

varying float lightIntensity; 
varying vec3 Position;

void main (void)
{
    //
    // cheap noise
    //
    vec3 location = Position;

    vec3 floorvec = vec3(floor(Position.x * 10.0), 0.0, floor(Position.z * 10.0));
    vec3 noise = Position * 10.0 - floorvec - 0.5;
    noise *= noise;
    location += noise * 0.12;

    //
    // distance from axis
    //
    float dist = location.x * location.x + location.z * location.z;
    float grain = dist * GrainSizeRecip;

    //
    // grain effects as function of distance
    //
    float brightness = fract(grain);
    if (brightness > 0.5) 
        brightness = (1.0 - brightness);
    vec3 color = DarkColor + brightness * (colorSpread);
    
    brightness = fract(grain*7.0);    
    if (brightness > 0.5) 
        brightness = 1.0 - brightness;
    color -= brightness * colorSpread;

    //
    // apply lighting effects from vertex processor
    //
    color = clamp(color * lightIntensity, 0.0, 1.0); 
  
    gl_FragColor = vec4(color, 1.0);
}
