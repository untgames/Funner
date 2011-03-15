struct RendererEntity;
struct RendererFrame;
struct RendererPrimitive;

///Описание фрейма для рендеринга
struct RendererFrame
{
    //статика / динамика????
  IStateBlock*   state_block;  //блок состояний кадра
  size_t         passes_count; //количество проходов
  RendererPass** passes;       //проходы
};

///Описание прохода
struct RendererPass
{
  IStateBlock*        state_block;      //блок состояний объекта
  size_t              primitives_count; //количество примитивов прохода
  RendererPrimitive** primitives;       //примитивы прохода
  size_t              passes_count;     //количество вложенных проходов
  RendererPass**      passes;           //вложенные проходы
};

///Описание примитива
struct RendererPrimitive
{
  IStateBlock*                     material_state_block; //блок состояний материала примитива
  IStateBlock*                     entity_state_block;   //блок состояний объекта примитива
  render::low_level::PrimitiveType type;                 //тип примитива
  size_t                           first;                //индекс первой вершины/индекса
  size_t                           count;                //количество примитивов
};

///Рисование прохода
void draw (IDevice& device, RendererPass& pass);

