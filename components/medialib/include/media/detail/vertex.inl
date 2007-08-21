namespace detail
{

/*
    Определение идентификатора типа
*/

template <VertexAttributeType in_value> struct attribute_type_helper
{
  static const VertexAttributeType value = in_value;
};

template <class T> struct attribute_type;

template <> struct attribute_type<math::vec<float, 2> >         : public attribute_type_helper<VertexAttributeType_Float2> {};
template <> struct attribute_type<math::vec<float, 3> >         : public attribute_type_helper<VertexAttributeType_Float3> {};
template <> struct attribute_type<math::vec<float, 4> >         : public attribute_type_helper<VertexAttributeType_Float4> {};
template <> struct attribute_type<math::vec<short, 2> >         : public attribute_type_helper<VertexAttributeType_Short2> {};
template <> struct attribute_type<math::vec<short, 3> >         : public attribute_type_helper<VertexAttributeType_Short3> {};
template <> struct attribute_type<math::vec<short, 4> >         : public attribute_type_helper<VertexAttributeType_Short4> {};
template <> struct attribute_type<math::vec<unsigned char, 4> > : public attribute_type_helper<VertexAttributeType_UByte4> {};

/*
    Определение формата атрибутов вершины
*/

template <class Vertex, class T> struct vertex_format;

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, Position<T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Position, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, position));
  }
};

template <class Vertex, class T> struct vertex_format<Vertex, Normal<T> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Normal, attribute_type<math::vec<T, 3> >::value, offsetof (Vertex, normal));
  }
};

template <class Vertex, class T> struct vertex_format<Vertex, Tangent<T> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Tangent, attribute_type<math::vec<T, 3> >::value, offsetof (Vertex, tangent));
  }
};

template <class Vertex, class T> struct vertex_format<Vertex, Binormal<T> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Binormal, attribute_type<math::vec<T, 3> >::value, offsetof (Vertex, binormal));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, Color<T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Color, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, color));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<0, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord0, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord0));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<1, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord1, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord1));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<2, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord2, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord2));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<3, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord3, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord3));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<4, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord4, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord4));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<5, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord5, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord5));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<6, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord6, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord6));
  }
};

template <class Vertex, class T, size_t Size> struct vertex_format<Vertex, TexCoord<7, T, Size> >
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_TexCoord7, attribute_type<math::vec<T, Size> >::value, offsetof (Vertex, texcoord7));
  }
};

template <class Vertex> struct vertex_format<Vertex, VertexInfluence>
{
  static void get_format (VertexFormat& format)
  {
    format.AddAttribute (VertexAttributeSemantic_Influence, VertexAttributeType_Influence, offsetof (Vertex, first_joint));
  }
};

template <class Vertex> struct vertex_format<Vertex, EmptyAttribute>
{
  static void get_format (VertexFormat& format) {}
};

/*
    Определение формата вершины
*/

template <class Composite, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct vertex_format<Composite, Vertex<T0, T1, T2, T3, T4, T5, T6, T7> >
{
  static void get_format (VertexFormat& format)
  {
    vertex_format<Composite, T0>::get_format (format);
    vertex_format<Composite, T1>::get_format (format);
    vertex_format<Composite, T2>::get_format (format);
    vertex_format<Composite, T3>::get_format (format);
    vertex_format<Composite, T4>::get_format (format);
    vertex_format<Composite, T5>::get_format (format);
    vertex_format<Composite, T6>::get_format (format);
    vertex_format<Composite, T7>::get_format (format);
  }
};

}

/*
    Генерация вершины
*/

//хранилище для вершинного атрибута
template <class T> struct VertexAttributeHolder: public T {};

//специализация для списка типов
template <class Head, class Tail>
struct VertexAttributeHolder<xtl::mpl::type_node<Head, Tail> >: public VertexAttributeHolder<Head>, public VertexAttributeHolder<Tail> {};

template <> struct VertexAttributeHolder<xtl::mpl::null_type> {};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct Vertex: public VertexAttributeHolder<T0>, public Vertex<T1, T2, T3, T4, T5, T6, T7> {};

template <> struct Vertex<>: public EmptyAttribute {};

/*
    Определение формата вершины
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void get_format (const Vertex<T0, T1, T2, T3, T4, T5, T6, T7>*, VertexFormat& format)
{
  typedef Vertex<T0, T1, T2, T3, T4, T5, T6, T7> CustomVertex;

  detail::vertex_format<CustomVertex, CustomVertex>::get_format (format);
}
