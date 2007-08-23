/*
    Объявление вершины
*/

inline VertexDeclaration::VertexDeclaration (const VertexFormat& in_format)
  : format (&in_format), vertex_size (in_format.GetMinimalVertexSize ())
  {}

inline VertexDeclaration::VertexDeclaration (const VertexFormat& in_format, size_t in_vertex_size)
  : format (&in_format), vertex_size (in_vertex_size)
{
  if (vertex_size < in_format.GetMinimalVertexSize ())
    vertex_size = in_format.GetMinimalVertexSize ();
}

inline const VertexFormat& VertexDeclaration::Format () const
{
  return *format;
}

inline size_t VertexDeclaration::VertexSize () const
{
  return vertex_size;
}

template <class Vertex>
inline VertexDeclaration make_vertex_declaration ()
{
  return VertexDeclaration (get_vertex_format<Vertex> (), sizeof (Vertex));
}

/*
    Получение данных вершинного массива
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

/*
    VertexWeight
*/

inline VertexWeight::VertexWeight ()
  : joint_index (0), joint_weight (0.0f)
  {}
