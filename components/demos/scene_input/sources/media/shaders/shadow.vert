uniform mat4 ModelViewProjectionMatrix;

void main (void)
{
  gl_Position = ModelViewProjectionMatrix * gl_Vertex;
}                
