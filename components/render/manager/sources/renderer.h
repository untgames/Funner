class MaterialImpl;
class EntityImpl;
class ProgramParametersLayout;
class DynamicPrimitive;
class ShaderOptionsCache;
class RectAreaImpl;
class BatchingManager;

template <class T> class DynamicPrimitiveBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершина спрайта / вершина линии
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DynamicPrimitiveVertex
{
  math::vec3f  position;
  math::vec3f  normal;
  math::vec2f  tex_coord;
  math::vec4ub color;
};

typedef unsigned short                                 DynamicPrimitiveIndex;
typedef DynamicPrimitiveBuffer<DynamicPrimitiveIndex>  DynamicIndexBuffer;
typedef DynamicPrimitiveBuffer<DynamicPrimitiveVertex> DynamicVertexBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание примитива рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitive
{
  MaterialImpl*                       material;         //материал
  render::low_level::IStateBlock*     state_block;      //блок состояний примитива
  bool                                indexed;          //является ли данный примитив индексированным или состоящим только из вершин без индексов
  render::low_level::PrimitiveType    type;             //тип примитива
  unsigned int                        first;            //индекс первой вершины/индекса
  unsigned int                        count;            //количество примитивов
  unsigned int                        base_vertex;      //индекс базовой вершины
  size_t                              tags_count;       //количество тэгов материала
  const size_t*                       tags;             //тэги материала
  const DynamicPrimitiveIndex* const* dynamic_indices;  //указатель на массив динамических индексов
  BatchingManager*                    batching_manager; //менеджер упаковки
  size_t                              batching_hash;    //хэш пакета
};

inline size_t get_batching_hash (const RendererPrimitive& p)
{
  return common::crc32 (&p.state_block, sizeof (p.state_block), common::crc32 (&p.type, sizeof (p.type), common::crc32 (&p.base_vertex, sizeof (p.base_vertex))));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание группы примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitiveGroup
{
  unsigned int             primitives_count; //количество примитивов в группе
  const RendererPrimitive* primitives;       //примитивы
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание группы динамических примитивов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererDynamicPrimitiveGroup: public RendererPrimitiveGroup
{
  DynamicPrimitive* dynamic_primitive;

  RendererDynamicPrimitiveGroup (const RendererPrimitiveGroup& group, DynamicPrimitive* primitive)
    : RendererPrimitiveGroup (group)
    , dynamic_primitive (primitive)
  {
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание операции рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererOperation
{
  EntityImpl*                     entity;                   //объект
  render::low_level::IStateBlock* state_block;              //блок состояний объекта
  ProgramParametersLayout*        entity_parameters_layout; //расположение параметров объекта
  const RendererPrimitive*        primitive;                //примитив
  Program*                        program;                  //программа (с учетом опций шейдера, задаваемых в EntityImpl)
  DynamicPrimitive*               dynamic_primitive;        //динамический примитив, соответствующий операции (может быть 0)
  const BoxAreaImpl*              scissor;                  //область отсечения (может быть null)
  low_level::IBlendState*         blend_state;              //низкоуровневое состояние режима блендинга объекта (может быть 0)
  size_t                          batching_hash;            //хэш пакета
};

inline size_t get_batching_hash (const RendererOperation& op)
{
  if (!op.primitive->batching_hash)
    return 0;

  return common::crc32 (&op.scissor, sizeof (op.scissor), common::crc32 (&op.state_block, sizeof (op.state_block),
    common::crc32 (&op.entity_parameters_layout, sizeof (op.entity_parameters_layout),
    common::crc32 (&op.program, sizeof (op.program),
    common::crc32 (&op.blend_state, sizeof (op.blend_state), op.primitive->batching_hash)))));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание списка операций
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererOperationList
{
  unsigned int             operations_count; //количество операций
  const RendererOperation* operations;       //операции
};
