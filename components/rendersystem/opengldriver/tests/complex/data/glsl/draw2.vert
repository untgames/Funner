uniform vec4 my_time;
uniform mat4 my_transform;
varying vec4 color;

void main (void)
{
  color       = my_time * gl_Color;
  gl_Position = my_transform * ftransform ();
}
