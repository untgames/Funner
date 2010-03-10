#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации карты высот
*/

typedef stl::vector<HeightMap::VertexDesc>             VertexArray;
typedef xtl::signal<void (HeightMap&, HeightMapEvent)> HeightMapSignal;

struct HeightMap::Impl
{
  VertexArray     vertices;               //вершины карты высот
  size_t          rows_count;             //количество строк
  size_t          columns_count;          //количество столбцов
  HeightMapSignal update_vertices_signal; //сигнал обновления вершин
  HeightMapSignal update_sizes_signal;    //сигнал обновления размеров поля
  
  Impl () : rows_count (0), columns_count (0) {}
};

/*
    Конструктор / деструктор
*/

HeightMap::HeightMap ()
  : impl (new Impl)
{
}

HeightMap::~HeightMap ()
{
}

/*
    Создание карты
*/

HeightMap::Pointer HeightMap::Create ()
{
  return Pointer (new HeightMap, false);
}

/*
    Размеры карты
*/

void HeightMap::SetRowsCount (size_t rows_count)
{
  SetCellsCount (rows_count, ColumnsCount ());
}

void HeightMap::SetColumnsCount (size_t columns_count)
{
  SetCellsCount (RowsCount (), columns_count);
}

void HeightMap::SetCellsCount (size_t rows_count, size_t columns_count)
{
  if (!rows_count && !columns_count)
  {
    impl->vertices.clear ();
    
    impl->rows_count     = 0;
    impl->columns_count  = 0;

    try
    {
      impl->update_sizes_signal (*this, HeightMapEvent_OnSizesUpdate);
    }
    catch (...)
    {      
    }
    
    return;
  }
  
  static const char* METHOD_NAME = "scene_graph::HeightMap::SetCellsCount";

  if (!rows_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "rows_count");
    
  if (!columns_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "columns_count");
    
  VertexArray new_vertices (rows_count * columns_count);
    
  for (size_t i=0, count=stl::min (impl->rows_count, rows_count); i<count; i++)
  {
    const VertexDesc* src = &impl->vertices [i * impl->columns_count];
    VertexDesc*       dst = &new_vertices [i * rows_count];

    stl::copy (src, src + stl::min (impl->columns_count, columns_count), dst);
  }

  impl->vertices.swap (new_vertices);

  impl->rows_count    = rows_count;
  impl->columns_count = columns_count;
  
  try
  {
    impl->update_sizes_signal (*this, HeightMapEvent_OnSizesUpdate);
  }
  catch (...)
  {      
  }  
}

size_t HeightMap::RowsCount () const
{
  return impl->rows_count;
}

size_t HeightMap::ColumnsCount () const
{
  return impl->columns_count;
}

/*
    Работа с вершинами
*/

const HeightMap::VertexDesc* HeightMap::Vertices () const
{
  return impl->rows_count && impl->columns_count ? &impl->vertices [0] : 0;
}

HeightMap::VertexDesc* HeightMap::Vertices ()
{
  return impl->rows_count && impl->columns_count ? &impl->vertices [0] : 0;
}

const HeightMap::VertexDesc& HeightMap::Vertex (size_t row, size_t column) const
{
  static const char* METHOD_NAME = "scene_graph::HeightMap::Vertex";

  if (row >= impl->rows_count)
    throw xtl::make_range_exception (METHOD_NAME, "row", row, impl->rows_count);
    
  if (column >= impl->columns_count)
    throw xtl::make_range_exception (METHOD_NAME, "column", column, impl->columns_count);

  return impl->vertices [row * impl->columns_count + column];
}

HeightMap::VertexDesc& HeightMap::Vertex (size_t row, size_t column)
{
  return const_cast<VertexDesc&> (const_cast<const HeightMap&> (*this).Vertex (row, column));
}

/*
    Оповещение об обновлении вершин
*/

void HeightMap::UpdateVerticesNotify ()
{
  try
  {
    if (impl->update_vertices_signal.empty ())
      return;

    impl->update_vertices_signal (*this, HeightMapEvent_OnVerticesUpdate);
  }
  catch (...)
  {
  }
}

/*
    Регистрация обработчиков событий
*/

xtl::connection HeightMap::RegisterEventHandler (HeightMapEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case HeightMapEvent_OnVerticesUpdate:
    case HeightMapEvent_OnSizesUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::HeightMap::RegisterEventHandler", "event", event);
  }

  return impl->update_vertices_signal.connect (handler);
}

/*
    Динамическая диспетчеризация
*/

void HeightMap::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
