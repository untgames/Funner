namespace detail
{

//хранилище формата вершины
template <class Vertex> struct VertexFormatHolder
{
  VertexFormatHolder () { get_vertex_format (static_cast<const Vertex*> (0), format); }

  VertexFormat format;
};

}

/*
    Получение формата вершин
*/

template <class Vertex>
inline const VertexFormat& get_vertex_format ()
{
  return common::Singleton<detail::VertexFormatHolder<Vertex> >::Instance ()->format;
}
