/*
    ѕолучение данных вершинного массива
*/

template <class Vertex>
const Vertex* VertexStream::Data () const
{ 
  if (get_vertex_format<Vertex> () != Format ())
    return 0;
    
  return static_cast<const Vertex*> (this->Data ());
}

template <class Vertex>
inline Vertex* VertexStream::Data ()
{
  return const_cast<Vertex*> (const_cast<const VertexStream&> (*this).Data<Vertex> ());
}
