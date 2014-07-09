#include "shared.h"

using namespace render::manager;

//TODO: shared EffectRenderer buffers (operations, passes, mvp_matrices, normalized_scissors, etc)
//TODO: program/parameters_layout/local_textures FIFO cache

/*
  Sprite integration:
    - operations merge condition:
      * RendererPrimitive::indexed == true
      * same RendererOperation::batching_hash
*/

namespace
{

using render::low_level::DEVICE_RENDER_TARGET_SLOTS_COUNT;

/*
    Константы
*/

const size_t RESERVE_OPERATION_ARRAY          = 512; //резервируемое число операций в массиве операций прохода
const size_t RESERVE_FRAME_ARRAY              = 32;  //резервируемое число вложенных фреймов в эффекте
const size_t RESERVE_MVP_MATRIX_ARRAY         = 512; //резервируемое число матриц MVP (для динамических примитивов)
const size_t RESERVE_BATCHES_ARRAY            = 128; //резервируемое количество пакетов
const size_t RESERVE_NORMALIZED_SCISSOR_ARRAY = 16;  //резервируемое количество областей отсечения

/*
    Вспомогательные структуры
*/

///Операция прохода
struct PassOperation: public RendererOperation
{
  ProgramParametersLayout*     frame_entity_parameters_layout; //расположение параметров пары фрейм-объект
  render::low_level::IBuffer*  frame_entity_parameters_buffer; //буфер параметров пары фрейм-объект
  size_t                       eye_distance;                   //расстояние от z-near
  int                          mvp_matrix_index;               //ссылка на матрицу Model-View-Projection
  int                          normalized_scissor_index;       //ссылка на область отсечения в относительных координатах
  
///Конструктор
  PassOperation (const RendererOperation&    base,
                 ProgramParametersLayout*    in_frame_entity_parameters_layout,
                 render::low_level::IBuffer* in_frame_entity_parameters_buffer,
                 size_t                      in_eye_distance,
                 int                         in_mvp_matrix_index,
                 int                         in_normalized_scissor_index)
    : RendererOperation (base)
    , frame_entity_parameters_layout (in_frame_entity_parameters_layout)
    , frame_entity_parameters_buffer (in_frame_entity_parameters_buffer)
    , eye_distance (in_eye_distance)
    , mvp_matrix_index (in_mvp_matrix_index)
    , normalized_scissor_index (in_normalized_scissor_index)
  {
      //обновление хэша

    if (batching_hash)
    {
      batching_hash = common::crc32 (&frame_entity_parameters_layout, sizeof (frame_entity_parameters_layout),
        common::crc32 (&frame_entity_parameters_buffer, sizeof (frame_entity_parameters_buffer), batching_hash));
    }
  }
};

///Дескриптор менеджера упаковки
struct BatchingManagerDesc
{
  BatchingManager* manager;
  size_t           saved_dynamic_vertices_count;

  BatchingManagerDesc (BatchingManager* in_manager) : manager (in_manager), saved_dynamic_vertices_count (manager->AllocatedDynamicVerticesCount ()) {}
};

typedef stl::vector<PassOperation*>      OperationPtrArray;
typedef stl::vector<PassOperation>       OperationArray;
typedef stl::vector<BatchingManagerDesc> BatchingManagerArray;

///Компаратор от наиболее близкого объекта к наиболее удаленному
struct FrontToBackComparator
{
  bool operator () (const PassOperation* op1, const PassOperation* op2) const
  {
    return op1->eye_distance < op2->eye_distance;
  }
};

///Компаратор от наиболее удаленного объекта к наиболее близкому
struct BackToFrontComparator
{
  bool operator () (const PassOperation* op1, const PassOperation* op2) const
  {
    return op1->eye_distance > op2->eye_distance;
  }
};

///Компаратор по кэшированию состояний
struct StateSwitchComparator
{
  template <class T> bool compare (const T& obj1, const T& obj2, bool& result) const
  {
    if (obj1 == obj2)
      return false;
      
    result = obj1 < obj2;
    
    return true;
  }

  bool operator () (const PassOperation* op1, const PassOperation* op2) const
  {
    bool result = false;

    const RendererPrimitive *primitive1 = op1->primitive, *primitive2 = op2->primitive;

    if (compare (primitive1->batching_hash, primitive2->batching_hash, result))
      return result;
          
    if (compare (primitive1, primitive2, result))
      return result;    
      
    if (compare (primitive1->material, primitive2->material, result))
      return result;

    if (compare (op1->entity, op2->entity, result))
      return result;    

    return false;    
  }
};

///Проход рендеринга
struct RenderPass: public xtl::reference_counter
{  
  common::StringArray      color_targets;           //имя целевых буферов цвета
  stl::string              depth_stencil_target;    //имя целевого буфера отсечения
  SortMode                 sort_mode;               //режим сортировки
  LowLevelStateBlockPtr    scissor_off_state_block; //блок состояний прохода
  LowLevelStateBlockPtr    scissor_on_state_block;  //блок состояний прохода с включенным тестом отсечения
  ProgramParametersLayout  parameters_layout;       //расположение параметров
  size_t                   clear_flags;             //флаги очистки экрана
  OperationArray           operations;              //операции рендеринга
  OperationPtrArray        operation_ptrs;          //указатели на операции
  const RendererOperation* last_operation;          //последняя добавленная операция
  FrameImpl&               frame;                   //ссылка на родительский кадр

///Конструктор
  RenderPass (const DeviceManagerPtr& device_manager, FrameImpl& in_frame)
    : sort_mode (SortMode_Default)
    , parameters_layout (&device_manager->Device (), &device_manager->Settings ())
    , clear_flags (ClearFlag_All)
    , last_operation ()
    , frame (in_frame)
  {
    operations.reserve (RESERVE_OPERATION_ARRAY);
    operation_ptrs.reserve (operations.size ());
  }
  
///Сортировка
  void Sort ()
  {
    switch (sort_mode)
    {
      case SortMode_FrontToBack:
        Sort (FrontToBackComparator ());
        break;
      case SortMode_BackToFront:
        Sort (BackToFrontComparator ());      
        break;
      case SortMode_StateSwitch:
        Sort (StateSwitchComparator ());      
        break;
      default:
      case SortMode_None:
        break;        
    }
  }
  
  template <class Compare> void Sort (Compare fn)
  {
    stl::sort (operation_ptrs.begin (), operation_ptrs.end (), fn);
  }
};

typedef stl::vector<FrameImpl*> FrameArray;

///Инстанцированный эффект
struct RenderInstantiatedEffect: public xtl::reference_counter
{
  FrameArray frames;     //вложенные фреймы
  FrameImpl* last_frame; //последний добавленный фрейм
  
///Конструктор
  RenderInstantiatedEffect ()
    : last_frame (0)
  {
    frames.reserve (RESERVE_FRAME_ARRAY);
  }
};

typedef xtl::intrusive_ptr<RenderPass>               RenderPassPtr;
typedef xtl::intrusive_ptr<RenderInstantiatedEffect> RenderInstantiatedEffectPtr;

///Операция эффекта
struct RenderEffectOperation: public xtl::reference_counter
{
  RenderPassPtr               pass;   //проход рендеринга
  RenderInstantiatedEffectPtr effect; //инстанцированный эффект
  
///Конструкторы
  RenderEffectOperation (const RenderPassPtr& in_pass) : pass (in_pass) {}
  
  RenderEffectOperation (const RenderInstantiatedEffectPtr& in_effect) : effect (in_effect) {}
};

typedef xtl::rect<float> rectf;

///Область отсечения в нормированных координатах [0;1]
struct NormalizedScissor
{
  rectf              rect;    //область отсечения
  const BoxAreaImpl* scissor; //ссылка на объект-контейнер

///Конструкторы
  NormalizedScissor (const BoxAreaImpl* in_scissor, const math::mat4f& vp_matrix)
    : scissor (in_scissor)
  {
    math::vec4f vmin = vp_matrix * math::vec4f (scissor->Box ().min_extent, 1.0f),
                vmax = vp_matrix * math::vec4f (scissor->Box ().max_extent, 1.0f);

    vmin  = max (math::vec4f (0.0f), min (math::vec4f (1.0f), (vmin / vmin.w + math::vec4f (1.0f)) * math::vec4f (0.5f, 0.5f, 0.5f, 1.0f)));
    vmax  = max (math::vec4f (0.0f), min (math::vec4f (1.0f), (vmax / vmax.w + math::vec4f (1.0f)) * math::vec4f (0.5f, 0.5f, 0.5f, 1.0f)));

    float min_x = vmin.x < vmax.x ? vmin.x : vmax.x,
          max_y = vmin.y > vmax.y ? vmin.y : vmax.y;

    rect = rectf (min_x, 1.0f - max_y, fabs (vmax.x - vmin.x), fabs (vmax.y - vmin.y));
  }
};

typedef xtl::intrusive_ptr<RenderEffectOperation>             RenderEffectOperationPtr;
typedef stl::hash_multimap<size_t, RenderPass*>               RenderPassMap;
typedef stl::hash_multimap<size_t, RenderInstantiatedEffect*> RenderInstantiatedEffectMap;
typedef stl::vector<RenderEffectOperationPtr>                 RenderEffectOperationArray;
typedef stl::vector<size_t>                                   TagHashArray;
typedef stl::vector<math::mat4f>                              MatrixArray;
typedef stl::vector<NormalizedScissor>                        NormalizedScissorArray;

}

/*
    Описание реализации рендера эффектов
*/

struct EffectRenderer::Impl
{
  DeviceManagerPtr            device_manager;      //менеджер устройства отрисовки
  TagHashArray                effect_tags;         //тэги эффекта
  RenderPassMap               passes;              //проходы рендеринга
  RenderInstantiatedEffectMap effects;             //инстанцированные эффекты
  RenderEffectOperationArray  operations;          //операции рендеринга
  MatrixArray                 mvp_matrices;        //матрицы
  NormalizedScissorArray      normalized_scissors; //нормированные области вывода
  BatchingManagerArray        batching_managers;   //менеджеры пакетирования
  bool                        right_hand_viewport; //является ли область вывода правосторонней
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , right_hand_viewport (false)
  {
    mvp_matrices.reserve (RESERVE_MVP_MATRIX_ARRAY);
    normalized_scissors.reserve (RESERVE_NORMALIZED_SCISSOR_ARRAY);
    batching_managers.reserve (RESERVE_BATCHES_ARRAY);
  }
};

/*
    Конструктор / деструктор
*/

EffectRenderer::EffectRenderer (const EffectPtr& effect, const DeviceManagerPtr& device_manager, FrameImpl& frame, ProgramParametersLayout* parent_layout)
  : impl (new Impl (device_manager))
{
  try
  {
      //проверка корректности аргументов
    
    if (!effect)
      throw xtl::make_null_argument_exception ("", "effect");
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");                  
      
      //получение свойств устройства      
    
    impl->right_hand_viewport = device_manager->DeviceCaps ().has_right_hand_viewport;
      
      //копирование тэго эффекта в состояние рендера
      
    if (effect->TagsCount ())
      impl->effect_tags.assign (effect->TagHashes (), effect->TagHashes () + effect->TagsCount ());
      
      //построение структуры диспетчеризации операций рендеринга

    impl->operations.reserve (effect->OperationsCount ());

    for (size_t i=0, count=effect->OperationsCount (); i<count; i++)
    {
        //операция рендеринга может быть либо проходом рендеринга, либо рисованием вложенной группы эффектов
      
      EffectPassPtr            src_pass                = effect->OperationPass (i);
      InstantiatedEffectPtr    src_instantiated_effect = effect->OperationEffect (i);
      RenderEffectOperationPtr operation;
      
      if (src_pass) 
      {
          //обработка операции рендеринга - прохода рендеринга
        
        RenderPassPtr pass (new RenderPass (device_manager, frame), false);
        
          //построение карты соответствия: тэг материала -> проход рендеринга
        
        for (size_t j=0, count=src_pass->TagsCount (); j<count; j++)
        {
          size_t hash = src_pass->TagHash (j);
          
          impl->passes.insert_pair (hash, &*pass);
        }
        
          //кэширование общих параметров рендеринга для всего прохода
        
        pass->color_targets           = src_pass->ColorTargets ().Clone ();
        pass->depth_stencil_target    = src_pass->DepthStencilTarget ();
        pass->sort_mode               = src_pass->SortMode ();
        pass->scissor_off_state_block = src_pass->StateBlock (false);
        pass->scissor_on_state_block  = src_pass->StateBlock (true);
        pass->clear_flags             = src_pass->ClearFlags ();
        
          //связывание объекта размещения параметров прохода с родительским объектом размещения параметров

        if (parent_layout)
          pass->parameters_layout.Attach (*parent_layout);
          
          //создание операции рендеринга

        operation = RenderEffectOperationPtr (new RenderEffectOperation (pass), false);
      }
      else if (src_instantiated_effect)
      {
          //обработка операции рендеринга - группы эффектов рендеринга
        
        RenderInstantiatedEffectPtr instantiated_effect (new RenderInstantiatedEffect, false);
        
          //построение карты соответствия: тэг эффекта -> группа эффектов рендеринга

        for (size_t j=0, count=src_instantiated_effect->TagsCount (); j<count; j++)
        {
          size_t hash = src_instantiated_effect->TagHash (j);
          
          impl->effects.insert_pair (hash, &*instantiated_effect);
        }
        
          //создание операции рендеринга        

        operation = RenderEffectOperationPtr (new RenderEffectOperation (instantiated_effect), false);
      }            
      else throw xtl::format_operation_exception ("", "Wrong effect operation %u. Operation may have pass or effect", i);     
      
        //добавленние операции рендеринга к списку операций эффекта
      
      impl->operations.push_back (operation);      
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectRenderer::EffectRenderer");
    throw;
  }
}

EffectRenderer::~EffectRenderer ()
{
}

/*
    Добавление операций в список отрисовки
*/

void EffectRenderer::AddOperations
 (const RendererOperationList& operations_desc,
  size_t                       eye_distance,
  const math::mat4f&           vp_matrix,
  const math::mat4f&           mvp_matrix,
  render::low_level::IBuffer*  entity_dependent_property_buffer,
  ProgramParametersLayout*     entity_dependent_property_layout,
  render::low_level::IBuffer*  entity_independent_property_buffer,
  ProgramParametersLayout*     entity_independent_property_layout)
{
  static const char* METHOD_NAME = "render::manager::EffectRenderer::AddOperations(const RendererOperationList&)";  
  
    //проверка корректности аргументов

  if (!operations_desc.operations && operations_desc.operations_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "operations.operations");  

    //получение идентификатора текущего кадра

  FrameId current_frame_id = impl->device_manager->CacheManager ().CurrentFrame ();
    
    //кэширование карты проходов
    
  RenderPassMap& passes = impl->passes;
  
    //перебор операций
    
  const RendererOperation* operation = operations_desc.operations;    

  for (size_t i=0, count=operations_desc.operations_count; i!=count; i++, operation++)
  {
      //определение списка тэгов данной операции
    
    size_t        tags_count = operation->primitive->tags_count;    
    const size_t* tag        = operation->primitive->tags;

      //определение буфера / лэйаута пары frame-entity

    render::low_level::IBuffer* property_buffer = entity_dependent_property_buffer;
    ProgramParametersLayout*    property_layout = entity_dependent_property_layout;
    
      //сопоставление тэга операции проходу рендеринга

    bool dynamic_primitives_added = false;

    for (size_t j=0; j!=tags_count; j++, tag++)
    {
        //поиск проходов по тэгу операции
      
      stl::pair<RenderPassMap::iterator, RenderPassMap::iterator> range = passes.equal_range (*tag);

        //добавление операции в проход

      int mvp_matrix_index = -1;
      
      for (;range.first!=range.second; ++range.first)
      {
          //предварительное обновление динамических примитивов

        if (!dynamic_primitives_added)
        {
          if (DynamicPrimitive* dynamic_primitive = operation->dynamic_primitive)
          {
            dynamic_primitive->UpdateCache ();

            BatchingManager* batching_manager = operation->primitive->batching_manager;

            if (batching_manager && batching_manager->ActiveFrame () != current_frame_id)
            {
              batching_manager->ResetDynamicBuffers ();

              batching_manager->SetActiveFrame (current_frame_id);

              impl->batching_managers.push_back (batching_manager);
            }

            dynamic_primitive->UpdateOnPrerender (current_frame_id, *operation->entity);

            if (dynamic_primitive->IsFrameDependent ())
            {
              impl->mvp_matrices.push_back (mvp_matrix);

              mvp_matrix_index = impl->mvp_matrices.size () - 1;
            }

            if (!dynamic_primitive->IsEntityDependent ())
            {
              property_buffer = entity_independent_property_buffer;
              property_layout = entity_independent_property_layout;
            }
          }

          dynamic_primitives_added = true;
        }

          //добавление операции в проход

        RenderPass& pass = *range.first->second;
        
          //обработка частного случая: добавление операции несколько раз в один и тот же проход
          //данная ситуация может проявиться если и операция и проход содержать более двух соответствующих тэгов
        
        if (pass.last_operation == operation)
          continue;

          //обновление области отсечения

        int normalized_scissor_index = -1;

        if (operation->scissor)
        {
          NormalizedScissorArray::iterator iter = impl->normalized_scissors.begin (), end = impl->normalized_scissors.end ();

          for (; iter!=end; ++iter)
            if (iter->scissor == operation->scissor)
            {
              normalized_scissor_index = iter - impl->normalized_scissors.begin ();
              break;
            }

          if (iter == end)
          {
            normalized_scissor_index = impl->normalized_scissors.size ();

            impl->normalized_scissors.push_back (NormalizedScissor (operation->scissor, vp_matrix));
          }
        }
          
          //добавление операции к списку операций прохода

        pass.operations.push_back (PassOperation (*operation, property_layout, property_buffer, eye_distance, mvp_matrix_index, normalized_scissor_index));

        PassOperation& result_operation = pass.operations.back ();

        pass.operation_ptrs.push_back (&result_operation);        
        
          //запоминание последней добавленной операции для обработки частного случая добавления операции в один и тот же проход несколько раз

        pass.last_operation = operation;
      }
    }
  }
}

void EffectRenderer::AddOperations (FrameImpl& frame)
{
    //проверка корректности аргументов

  EffectRendererPtr renderer = frame.EffectRenderer ();
  
  if (!renderer)
    return;
    
    //получение списка тэгов эффекта кадра
    
  size_t        src_tags_count = renderer->impl->effect_tags.size ();
  const size_t* src_tag        = src_tags_count ? &renderer->impl->effect_tags [0] : (const size_t*)0;
  
    //кэширование инстанцированных эффектов

  RenderInstantiatedEffectMap& effects = impl->effects;
  
    //сопоставление тэгов переданного кадра группе рендеринга эффектов
  
  for (size_t i=0; i!=src_tags_count; i++, src_tag++)
  {
      //поиск группы эффектов по тэгу эффекта
    
    stl::pair<RenderInstantiatedEffectMap::iterator, RenderInstantiatedEffectMap::iterator> range = effects.equal_range (*src_tag);
    
    for (;range.first!=range.second; ++range.first)
    {
      RenderInstantiatedEffect& instantiated_effect = *range.first->second;
      
        //обработка частного случая повторного добавления кадра в группу
      
      if (instantiated_effect.last_frame == &frame)
        continue;
        
        //добавление кадра в группу
        
      instantiated_effect.frames.push_back (&frame);
      
      instantiated_effect.last_frame = &frame;
    }
  }
}

/*
    Очистка
*/

void EffectRenderer::RemoveAllOperations ()
{
  for (RenderEffectOperationArray::iterator iter=impl->operations.begin (), end=impl->operations.end (); iter!=end; ++iter)
  {
    RenderEffectOperation& operation = **iter;
    
    if (operation.pass)
    {
      operation.pass->operations.clear ();
      operation.pass->operation_ptrs.clear ();

      operation.pass->last_operation = 0;
    }

    if (operation.effect)
    {
      operation.effect->frames.clear ();

      operation.effect->last_frame = 0;
    }
  }

  impl->mvp_matrices.clear ();

  impl->normalized_scissors.clear ();

  impl->batching_managers.clear ();
}

/*
    Выполнение операций
*/

namespace
{

///Контекст целевых буферов рендеринга
struct RenderTargetContext
{
  size_t        targets_count;
  const rectf*  current_local_scissor;
  bool          has_scissors;
  Rect          viewport_rects [DEVICE_RENDER_TARGET_SLOTS_COUNT];
  RectAreaImpl* scissors [DEVICE_RENDER_TARGET_SLOTS_COUNT];

  RenderTargetContext ()
    : targets_count ()
    , current_local_scissor ()
    , has_scissors ()
  {  
  }
};

///Обобщение алгоритма рендеринга операций
struct RenderOperationsExecutor
{
  RenderingContext&                  context;                    //контекст рендеринга
  DeviceManager&                     device_manager;             //менеджер устройства отрисовки
  render::low_level::IDevice&        device;                     //устройство отрисовки
  render::low_level::IDeviceContext& device_context;             //контекст отрисовки
  ProgramParametersManager&          program_parameters_manager; //менеджер параметров программы шейдинга
  FrameImpl&                         frame;                      //фрейм
  ShaderOptionsCache&                frame_shader_options_cache; //кэш опций шейдеров для кадра
  LowLevelBufferPtr                  frame_property_buffer;      //буфер параметров кадра
  const MatrixArray&                 mvp_matrices;               //массив матриц Model-View-Projection
  const NormalizedScissorArray&      normalized_scissors;        //нормированные области отсечения
  BatchingManagerArray&              batching_managers;          //менеджеры пакетирования
  bool                               right_hand_viewport;        //является ли область вывода правосторонней
  
///Конструктор
  RenderOperationsExecutor (RenderingContext& in_context, DeviceManager& in_device_manager, const MatrixArray& in_mvp_matrices, const NormalizedScissorArray& in_normalized_scissors, BatchingManagerArray& in_batching_managers, bool in_right_hand_viewport)
    : context (in_context)
    , device_manager (in_device_manager)
    , device (device_manager.Device ())
    , device_context (device_manager.ImmediateContext ())
    , program_parameters_manager (device_manager.ProgramParametersManager ())
    , frame (context.Frame ())    
    , frame_shader_options_cache (frame.ShaderOptionsCache ())
    , frame_property_buffer (frame.DevicePropertyBuffer ())
    , mvp_matrices (in_mvp_matrices)
    , normalized_scissors (in_normalized_scissors)
    , batching_managers (in_batching_managers)
    , right_hand_viewport (in_right_hand_viewport)
  {
      //обновление динамических данных

    if (!batching_managers.empty ())
    {
      for (BatchingManagerArray::iterator iter=batching_managers.begin (), end=batching_managers.end (); iter!=end; ++iter)
      {
        BatchingManager& batching_manager = *iter->manager;

        batching_manager.DynamicVertexBuffer ().SyncBuffers (device);
      }  

      batching_managers.clear ();
    }
  }
  
///Выполнение массива операций
  void Draw (RenderEffectOperationArray& operations)
  {
    for (RenderEffectOperationArray::iterator iter=operations.begin (), end=operations.end (); iter!=end; ++iter)
      Draw (**iter);
  }
  
///Выполнение операции
  void Draw (RenderEffectOperation& operation)
  {
    if (operation.pass)
      DrawPass (*operation.pass);

    if (operation.effect)
      DrawEffect (*operation.effect);
  }
  
///Настройка целевых буферов отрисовки
  void SetRenderTargets (RenderPass& pass, RenderTargetContext& render_target_context)
  {
    render::low_level::IView* depth_stencil_view  = 0;
    size_t                    depth_stencil_width = 0, depth_stencil_height = 0;

      //получение информации о буфере отсечения

    if (!pass.depth_stencil_target.empty ())
    {
      RenderTargetDescPtr desc = context.FindRenderTarget (pass.depth_stencil_target.c_str ());

      if (desc && desc->render_target)
      {
//        if (desc->viewport)
//          throw xtl::format_operation_exception ("", "Depth-stencil target should not has a viewport");

//        if (desc->scissor)
//          throw xtl::format_operation_exception ("", "Depth-stencil target should not has a scissor");

        depth_stencil_view   = &desc->render_target->View (); 
        depth_stencil_width  = desc->render_target->Width ();
        depth_stencil_height = desc->render_target->Height ();
      }
    }

      //получение информации о буфера цвета

    render::low_level::IView*   render_target_views [DEVICE_RENDER_TARGET_SLOTS_COUNT];
    render::low_level::Viewport viewports [DEVICE_RENDER_TARGET_SLOTS_COUNT];

    size_t target_width       = 0, target_height = 0;
    bool   target_initialized = false;

    const char** color_target_names = pass.color_targets.Data ();

    render_target_context.targets_count = stl::min (pass.color_targets.Size (), DEVICE_RENDER_TARGET_SLOTS_COUNT);

    for (size_t i=0; i<render_target_context.targets_count; i++)
    {
      const char* target_name = color_target_names [i];

      RenderTargetDescPtr desc = context.FindRenderTarget (target_name);

      if (!desc || !desc->render_target)
      {
        render_target_views [i]                  = 0;
        render_target_context.scissors [i]       = 0;
        render_target_context.viewport_rects [i] = Rect ();

        continue;
      }

      ViewportImpl*       viewport        = &*desc->viewport;
      const math::vec2ui& viewport_offset = desc->render_target->ViewportOffset ();
      size_t              width           = desc->render_target->Width (),
                          height          = desc->render_target->Height ();

      render_target_views [i]            = &desc->render_target->View ();
      render_target_context.scissors [i] = &*desc->scissor;      

      if (desc->scissor)
        render_target_context.has_scissors = true;

      render::low_level::Viewport& viewport_desc = viewports [i];

      memset (&viewport_desc, 0, sizeof (viewport_desc));

      if (viewport)
      {      
        const Rect& rect = viewport->Area ().Rect ();
        
        viewport_desc.x         = rect.x + viewport_offset.x;
        viewport_desc.y         = rect.y + viewport_offset.y;
        viewport_desc.width     = rect.width;
        viewport_desc.height    = rect.height;
        viewport_desc.min_depth = viewport->MinDepth ();
        viewport_desc.max_depth = viewport->MaxDepth ();

        if (right_hand_viewport)
          viewport_desc.y = height - viewport_desc.height - viewport_desc.y;
      }
      else
      {
        viewport_desc.x         = viewport_offset.x;
        viewport_desc.y         = viewport_offset.y;
        viewport_desc.width     = width;
        viewport_desc.height    = height;
        viewport_desc.min_depth = 0.0f;
        viewport_desc.max_depth = 1.0f;
      }

      render_target_context.viewport_rects [i] = Rect (viewport_desc.x, viewport_desc.y, viewport_desc.width, viewport_desc.height);

      if (target_initialized)
      {
        if (target_width != viewport_desc.width || target_height != viewport_desc.height)
          throw xtl::format_operation_exception ("", "Render target %s sizes mismatch (%u, %u) with (%u, %u)", target_name, viewport_desc.width, viewport_desc.height, target_width, target_height);
      }
      else
      {
        target_width       = width;
        target_height      = height;
        target_initialized = true;
      }      
    }

      //проверка совместимости буфера глубины и цвета

    if (target_initialized && depth_stencil_view && (depth_stencil_width != target_width || depth_stencil_height != target_height))
      throw xtl::format_operation_exception ("", "Different render target sizes: render target sizes (%u, %u) mismatch depth-stencil sizes (%u, %u)",
        target_width, target_height, depth_stencil_width, depth_stencil_height);

      //установка целей и областей вывода

    device_context.OSSetRenderTargets (render_target_context.targets_count, render_target_views, depth_stencil_view);

    for (size_t i=0; i<render_target_context.targets_count; i++)
      device_context.RSSetViewport (i, viewports [i]);

      //установка областей отсечения

    if (render_target_context.has_scissors)
      SetScissorRects (render_target_context, 0);
  }
  
///Очистка целевых буферов
  void ClearViews (RenderPass& pass, const RenderTargetContext& render_target_context)
  {
    size_t src_clear_flags = frame.ClearFlags () & pass.clear_flags, dst_clear_flags = 0;
    
    if (src_clear_flags & ClearFlag_RenderTarget) dst_clear_flags |= render::low_level::ClearFlag_RenderTarget;
    if (src_clear_flags & ClearFlag_Depth)        dst_clear_flags |= render::low_level::ClearFlag_Depth;
    if (src_clear_flags & ClearFlag_Stencil)      dst_clear_flags |= render::low_level::ClearFlag_Stencil;
    if (src_clear_flags & ClearFlag_ViewportOnly) dst_clear_flags |= render::low_level::ClearFlag_ViewportOnly;
    
    if (dst_clear_flags)
    {
      size_t                     indices [DEVICE_RENDER_TARGET_SLOTS_COUNT];
      render::low_level::Color4f colors [DEVICE_RENDER_TARGET_SLOTS_COUNT], &color = (render::low_level::Color4f&)frame.ClearColor ();

      for (size_t i=0; i<render_target_context.targets_count; i++)
      {
        indices [i] = i;
        colors [i]  = color;
      }

      device_context.ClearViews (dst_clear_flags, render_target_context.targets_count, indices, colors, frame.ClearDepthValue (), frame.ClearStencilIndex ());    
    }
  }
  
///Установка состояния области отсечения
  void SetScissorState (RenderPass& pass, bool state)
  {
    if (state)
    {
      if (pass.scissor_on_state_block)
        pass.scissor_on_state_block->Apply (&device_context);
    }
    else
    {
      if (pass.scissor_off_state_block)
        pass.scissor_off_state_block->Apply (&device_context);
    }    
  }
  
  void SetScissorRects (const RenderTargetContext& render_target_context, const rectf* operation_scissor)
  {
    for (size_t i=0; i<render_target_context.targets_count; i++)
    {
      const Rect& common_scissor_rect = render_target_context.scissors [i] ? render_target_context.scissors [i]->Rect () : render_target_context.viewport_rects [i];

      if (!operation_scissor)
      {
        SetScissorRect (i, common_scissor_rect);

        continue;
      }

      const Rect& viewport_rect = render_target_context.viewport_rects [i];
      Rect        operation_scissor_rect;

      operation_scissor_rect.x      = int (operation_scissor->x * viewport_rect.width);
      operation_scissor_rect.y      = int (operation_scissor->y * viewport_rect.height);
      operation_scissor_rect.width  = size_t (operation_scissor->width * viewport_rect.width);
      operation_scissor_rect.height = size_t (operation_scissor->height * viewport_rect.height);

      Rect result;
      
      result.x = stl::max (common_scissor_rect.x, operation_scissor_rect.x);
      result.y = stl::max (common_scissor_rect.y, operation_scissor_rect.y);
      
      int right  = stl::min (common_scissor_rect.x + common_scissor_rect.width, operation_scissor_rect.x + operation_scissor_rect.width),
          bottom = stl::min (common_scissor_rect.y + common_scissor_rect.height, operation_scissor_rect.y + operation_scissor_rect.height);

      result.width  = size_t (right - result.x);
      result.height = size_t (bottom - result.y);

      SetScissorRect (i, result);
    }
  }

  void SetScissorRect (size_t rt_index, const Rect& src_rect)
  {
    render::low_level::Rect dst_rect;

    memset (&dst_rect, 0, sizeof (dst_rect));

    dst_rect.x      = src_rect.x;
    dst_rect.y      = src_rect.y;
    dst_rect.width  = src_rect.width;
    dst_rect.height = src_rect.height;

    device_context.RSSetScissor (rt_index, dst_rect);
  }

///Восстановление состояния менеджеров упаковки
  void RestoreBatchingManagers ()
  {
    for (BatchingManagerArray::iterator iter=batching_managers.begin (), end=batching_managers.end (); iter!=end; ++iter)
    {
      BatchingManager& batching_manager = *iter->manager;

      batching_manager.SetPassUserData (0);
      batching_manager.SetAllocatedDynamicVerticesCount (iter->saved_dynamic_vertices_count);
    }

    batching_managers.clear ();
  }

///Обновление динамических примитивов
  void UpdateDynamicPrimitives (RenderPass& pass)
  {
    FrameImpl& frame = pass.frame;

    try
    {
      for (OperationPtrArray::iterator iter=pass.operation_ptrs.begin (), end=pass.operation_ptrs.end (); iter!=end; ++iter)
      {
        PassOperation&    operation         = **iter;
        DynamicPrimitive* dynamic_primitive = operation.dynamic_primitive;
        BatchingManager*  batching_manager  = operation.primitive->batching_manager;

        if (!dynamic_primitive || !dynamic_primitive->IsFrameDependent () || operation.mvp_matrix_index == -1)
          continue;

        if (batching_manager && batching_manager->PassUserData () != this)
        {
          batching_managers.push_back (batching_manager);

          batching_manager->SetPassUserData (this);
        }

        const math::mat4f& mvp_matrix = mvp_matrices [operation.mvp_matrix_index];

        dynamic_primitive->UpdateOnRender (frame, *operation.entity, context, mvp_matrix);
      }

        //обновление вершинных буферов

      for (BatchingManagerArray::iterator iter=batching_managers.begin (), end=batching_managers.end (); iter!=end; ++iter)
      {
        BatchingManager& batching_manager = *iter->manager;

        batching_manager.DynamicVertexBuffer ().SyncBuffers (device, iter->saved_dynamic_vertices_count);
      }

        //восстановление состояния менеджеров упаковки

      RestoreBatchingManagers ();
    }
    catch (...)
    {
        //восстановление состояния менеджеров упаковки

      RestoreBatchingManagers ();

      throw;
    }    
  }
  
///Рендеринг прохода
  void DrawPass (RenderPass& pass)
  {
      //обновление динамических примитивов
 
    UpdateDynamicPrimitives (pass); 

      //сортировка операций
      
    pass.Sort ();

      //установка целевых буферов отрисовки
    
    RenderTargetContext render_target_context;  
    
    SetRenderTargets (pass, render_target_context);
          
      //применение состояния прохода
      
    SetScissorState (pass, render_target_context.has_scissors);
    
      //очистка экрана
      
    ClearViews (pass, render_target_context);
      
      //установка константного буфера кадра

    device_context.SSSetConstantBuffer (ProgramParametersSlot_Frame, frame_property_buffer.get ());

      //выполнение операций                    

    for (OperationPtrArray::iterator iter=pass.operation_ptrs.begin (), end=pass.operation_ptrs.end (); iter!=end; ++iter)
      DrawPassOperation (pass, iter, end, render_target_context);
  }
  
///Выполнение операции прохода рендеринга
  void DrawPassOperation (RenderPass& pass, OperationPtrArray::iterator& operation_iter, OperationPtrArray::iterator operation_end, RenderTargetContext& render_target_context)
  {
    const PassOperation&     operation                   = **operation_iter;
    const RendererPrimitive& primitive                   = *operation.primitive;
    ShaderOptionsCache&      entity_shader_options_cache = *operation.shader_options_cache;
    const rectf*             operation_scissor           = operation.normalized_scissor_index != -1 ? &normalized_scissors [operation.normalized_scissor_index].rect : (const rectf*)0;

    if (!primitive.count)
      return;

      //поиск программы (TODO: кэширование поиска по адресам кэшей, FIFO)
      
    Program* program = &*primitive.material->Program ();

    if (!program)
      return;
      
    program = &program->DerivedProgram (frame_shader_options_cache, entity_shader_options_cache);

    device_context.SSSetProgram (program->LowLevelProgram ().get ());

      //применение состояния операции

    primitive.state_block->Apply (&device_context);

    if (operation.state_block)
      operation.state_block->Apply (&device_context);

      //обработка области отсечения объекта

    if (operation_scissor != render_target_context.current_local_scissor)
    {
      bool scissor_state = render_target_context.has_scissors || operation_scissor;

      SetScissorState (pass, scissor_state);

      if (scissor_state)
        SetScissorRects (render_target_context, operation_scissor);
      
      render_target_context.current_local_scissor = operation_scissor;
    }
    
      //установка локальных текстур
    
    if (program->HasFramemaps ())
    {
      const TexmapDesc* texmaps = program->Texmaps ();
      
      for (size_t i=0, count=program->TexmapsCount (); i<count; i++)
      {
        const TexmapDesc& texmap = texmaps [i];
        
        if (!texmap.is_framemap)
          continue;
          
          //TODO: use FIFO cache

        TexturePtr texture = context.FindLocalTexture (texmap.semantic.c_str ());
                       
        device_context.SSSetTexture (texmap.channel, texture ? texture->DeviceTexture ().get () : (render::low_level::ITexture*)0);
      }
    }

      //установка расположения параметров
      //TODO: кэшировать по entity (FIFO)

    ProgramParametersLayoutPtr program_parameters_layout = program_parameters_manager.GetParameters (&pass.parameters_layout, operation.entity_parameters_layout, operation.frame_entity_parameters_layout);

    device_context.SSSetProgramParametersLayout (&*program_parameters_layout->DeviceLayout ());
    device_context.SSSetConstantBuffer (ProgramParametersSlot_FrameEntity, operation.frame_entity_parameters_buffer);

      //рисование

    if (primitive.indexed)
    {
      if (operation.batching_hash)
      {
        size_t           batching_hash    = operation.batching_hash;
        BatchingManager* batching_manager = operation.primitive->batching_manager;

          //сброс текущего пакета

        batching_manager->ResetLinearIndexBuffer ();

          //определение границ пакета и копирование индексов в линейный буфер

        OperationPtrArray::iterator iter = operation_iter;

        size_t indices_count = 0;

        for (;iter!=operation_end && (*iter)->batching_hash == batching_hash; ++iter)
        {
          const RendererPrimitive& primitive = *(*iter)->primitive;

          const DynamicPrimitiveIndex* src_indices = *primitive.dynamic_indices + primitive.first;
          DynamicPrimitiveIndex*       dst_indices = batching_manager->AllocateDynamicIndices (IndexPoolType_Linear, primitive.count);

          memcpy (dst_indices, src_indices, primitive.count * sizeof (DynamicPrimitiveIndex));

          indices_count += primitive.count;
        }

        operation_iter = iter - 1;

          //синхронизация буферов

        batching_manager->DynamicIndexBuffer ().SyncBuffers (device);

          //рисование

        device_context.DrawIndexed (primitive.type, 0, indices_count, 0);
      }
      else
      {
        device_context.DrawIndexed (primitive.type, primitive.first, primitive.count, primitive.base_vertex);
      }
    }
    else
    {
      device_context.Draw (primitive.type, primitive.first + primitive.base_vertex, primitive.count);
    }
  }
  
///Рендеринг вложенного эффекта
  void DrawEffect (RenderInstantiatedEffect& effect)
  {
      //рисование вложенных кадров
    
    for (FrameArray::iterator iter=effect.frames.begin (), end=effect.frames.end (); iter!=end; ++iter)
      (*iter)->Draw (&context);
  }
};

}

void EffectRenderer::ExecuteOperations (RenderingContext& context)
{
  try
  {
      //исполнение операций

    RenderOperationsExecutor executor (context, *impl->device_manager, impl->mvp_matrices, impl->normalized_scissors, impl->batching_managers, impl->right_hand_viewport);

    executor.Draw (impl->operations);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectRenderer::ExecuteOperations");
    throw;
  }
}
