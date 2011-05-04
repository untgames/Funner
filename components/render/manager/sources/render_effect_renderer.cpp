#include "shared.h"

using namespace render;

//TODO: work with properties
//TODO: check scissor enabled???
//TODO: set local textures

namespace
{

/*
    Константы
*/

const size_t RESERVE_OPERATION_ARRAY = 512; //резервируемое число операций в массиве операций прохода
const size_t RESERVE_FRAME_ARRAY     = 32;  //резервируемое число вложенных фреймов в эффекте

/*
    Вспомогательные структуры
*/

typedef stl::vector<const RendererOperation*> OperationArray;

///Проход рендеринга
struct RenderPass: public xtl::reference_counter
{  
  common::StringArray      color_targets;         //имя целевых буферов цвета
  stl::string              depth_stencil_target;  //имя целевого буфера отсечения
  SortMode                 sort_mode;             //режим сортировки
  LowLevelStateBlockPtr    state_block;           //блок состояний прохода
  ProgramParametersLayout  parameters_layout;     //расположение параметров
  float                    viewport_min_depth;    //минимальное значение глубины области вывода
  float                    viewport_max_depth;    //максимальное значение глубины области вывода
  OperationArray           operations;            //операции рендеринга
  const RendererOperation* last_operation;        //последняя добавленная операция

///Конструктор
  RenderPass (const LowLevelDevicePtr& device)
    : sort_mode (SortMode_Default)
    , parameters_layout (device)
    , viewport_min_depth (0.0f)
    , viewport_max_depth (1.0f)
    , last_operation (0)
  {
    operations.reserve (RESERVE_OPERATION_ARRAY);
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

typedef xtl::intrusive_ptr<RenderEffectOperation>             RenderEffectOperationPtr;
typedef stl::hash_multimap<size_t, RenderPass*>               RenderPassMap;
typedef stl::hash_multimap<size_t, RenderInstantiatedEffect*> RenderInstantiatedEffectMap;
typedef stl::vector<RenderEffectOperationPtr>                 RenderEffectOperationArray;
typedef stl::vector<size_t>                                   TagHashArray;

}

/*
    Описание реализации рендера эффектов
*/


struct EffectRenderer::Impl
{
  DeviceManagerPtr            device_manager; //менеджер устройства отрисовки
  TagHashArray                effect_tags;    //тэги эффекта
  RenderPassMap               passes;         //проходы рендеринга
  RenderInstantiatedEffectMap effects;        //инстанцированные эффекты
  RenderEffectOperationArray  operations;     //операции рендеринга
  
  Impl (const DeviceManagerPtr& in_device_manager) : device_manager (in_device_manager) {}
};

/*
    Конструктор / деструктор
*/

EffectRenderer::EffectRenderer (const EffectPtr& effect, const DeviceManagerPtr& device_manager, ProgramParametersLayout* parent_layout)
  : impl (new Impl (device_manager))
{
  try
  {
    if (!effect)
      throw xtl::make_null_argument_exception ("", "effect");
      
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (effect->TagsCount ())
      impl->effect_tags.assign (effect->TagHashes (), effect->TagHashes () + effect->TagsCount ());

    impl->operations.reserve (effect->OperationsCount ());

    for (size_t i=0, count=effect->OperationsCount (); i<count; i++)
    {
      EffectPassPtr            src_pass                = effect->OperationPass (i);
      InstantiatedEffectPtr    src_instantiated_effect = effect->OperationEffect (i);
      RenderEffectOperationPtr operation;
      
      if (src_pass)
      {
        RenderPassPtr pass (new RenderPass (&device_manager->Device ()), false);
        
        for (size_t j=0, count=src_pass->TagsCount (); j<count; j++)
        {
          size_t hash = src_pass->TagHash (j);
          
          impl->passes.insert_pair (hash, &*pass);
        }
        
        pass->color_targets        = src_pass->ColorTargets ().Clone ();
        pass->depth_stencil_target = src_pass->DepthStencilTarget ();
        pass->sort_mode            = src_pass->SortMode ();
        pass->state_block          = src_pass->StateBlock ();
        pass->viewport_min_depth   = src_pass->ViewportMinDepth ();
        pass->viewport_max_depth   = src_pass->ViewportMaxDepth ();

        if (parent_layout)
          pass->parameters_layout.Attach (*parent_layout);

        operation = RenderEffectOperationPtr (new RenderEffectOperation (pass), false);
      }
      else if (src_instantiated_effect)
      {
        RenderInstantiatedEffectPtr instantiated_effect (new RenderInstantiatedEffect, false);

        for (size_t j=0, count=src_instantiated_effect->TagsCount (); j<count; j++)
        {
          size_t hash = src_instantiated_effect->TagHash (j);
          
          impl->effects.insert_pair (hash, &*instantiated_effect);
        }

        operation = RenderEffectOperationPtr (new RenderEffectOperation (instantiated_effect), false);
      }            
      else throw xtl::format_operation_exception ("", "Wrong effect operation %u. Operation may have pass or effect", i);
      
      impl->operations.push_back (operation);      
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EffectRenderer::EffectRenderer");
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
  render::low_level::IBuffer*  property_buffer,
  ProgramParametersLayout*     property_layou)
{
    //TODO: work with properties

  static const char* METHOD_NAME = "render::EffectRenderer::AddOperations(const RendererOperationList&)";

  if (!operations_desc.operations && operations_desc.operations_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "operations.operations");
    
  RenderPassMap& passes = impl->passes;
    
  const RendererOperation* operation = operations_desc.operations;
    
  for (size_t i=0, count=operations_desc.operations_count; i!=count; i++, operation++)
  {
    size_t        tags_count = operation->primitive->tags_count;
    const size_t* tag        = operation->primitive->tags;

    for (size_t j=0; j!=tags_count; j++, tag++)
    {
      stl::pair<RenderPassMap::iterator, RenderPassMap::iterator> range = passes.equal_range (*tag);

      for (;range.first!=range.second; ++range.first)
      {
        RenderPass& pass = *range.first->second;
        
        if (pass.last_operation == operation)
          continue;

        pass.operations.push_back (operation);

        pass.last_operation = operation;
      }
    }
  }
}

void EffectRenderer::AddOperations (FrameImpl& frame)
{
  EffectRendererPtr renderer = frame.EffectRenderer ();
  
  if (!renderer)
    return;
    
  size_t        src_tags_count = renderer->impl->effect_tags.size ();
  const size_t* src_tag        = src_tags_count ? &renderer->impl->effect_tags [0] : (const size_t*)0;

  RenderInstantiatedEffectMap& effects = impl->effects;
  
  for (size_t i=0; i!=src_tags_count; i++, src_tag++)
  {
    stl::pair<RenderInstantiatedEffectMap::iterator, RenderInstantiatedEffectMap::iterator> range = effects.equal_range (*src_tag);
    
    for (;range.first!=range.second; ++range.first)
    {
      RenderInstantiatedEffect& instantiated_effect = *range.first->second;
      
      if (instantiated_effect.last_frame == &frame)
        continue;
        
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

      operation.pass->last_operation = 0;
    }

    if (operation.effect)
    {
      operation.effect->frames.clear ();

      operation.effect->last_frame = 0;
    }
  }

  impl->operations.clear ();
}

/*
    Выполнение операций
*/

void EffectRenderer::ExecuteOperations (RenderingContext& context)
{
  try
  {
    render::low_level::IDevice& device                     = impl->device_manager->Device ();
    ProgramParametersManager&   program_parameters_manager = impl->device_manager->ProgramParametersManager ();

    for (RenderEffectOperationArray::iterator iter=impl->operations.begin (), end=impl->operations.end (); iter!=end; ++iter)
    {
      RenderEffectOperation& operation = **iter;
      
      if (operation.pass)
      {        
        RenderPass& pass = *operation.pass;
        
          //применение состояния прохода
        
        if (pass.state_block)
          pass.state_block->Apply ();
          
          //сортировка операций

//  SortMode                 sort_mode;             //режим сортировки          

          //установка целевых буферов отрисовки
          
        if (pass.color_targets.Size () > 1)
          throw xtl::format_not_supported_exception ("", "MRT not supported");
          
        render::low_level::IView *render_target_view = 0, *depth_stencil_view = 0;
        RectAreaPtr              viewport, scissor;
        int                      target_width = 0, target_height = 0;

        if (!pass.color_targets.IsEmpty ())        
        {
          RenderTargetDescPtr desc = context.FindRenderTarget (pass.color_targets [0]);

          if (desc && desc->render_target)
          {
            render_target_view = &desc->render_target->View ();
            viewport           = desc->viewport;
            scissor            = desc->scissor;
            target_width       = desc->render_target->Width ();
            target_height      = desc->render_target->Height ();
          }
        }
        
        if (!pass.depth_stencil_target.empty ())
        {
          RenderTargetDescPtr desc = context.FindRenderTarget (pass.depth_stencil_target.c_str ());
          
          if (desc && desc->render_target)
          {
            depth_stencil_view = &desc->render_target->View ();
            
            if (desc->viewport && viewport && viewport->Rect () != desc->viewport->Rect ())
              throw xtl::format_operation_exception ("", "Different viewport rect areas for color target and depth-stencil target");
              
            if (!viewport && desc->viewport)
              viewport = desc->viewport;

            if (desc->scissor && scissor && scissor->Rect () != desc->scissor->Rect ())
              throw xtl::format_operation_exception ("", "Different scissor rect areas for color target and depth-stencil target");              
              
            if (!scissor && desc->scissor)
              scissor = desc->scissor;
              
            if (render_target_view)
            {
              if (desc->render_target->Width () != target_width || desc->render_target->Height () != target_height)
                throw xtl::format_operation_exception ("", "Different render target sizes: render target sizes (%u, %u) mismatch depth-stencil sizes (%u, %u)",
                  target_width, target_height, desc->render_target->Width (), desc->render_target->Height ());
            }
          }                    
        }

        device.OSSetRenderTargets (render_target_view, depth_stencil_view);

          //настройка области вывода                

        if (viewport)
        {
          render::low_level::Viewport viewport_desc;
          const Rect&                 rect = viewport->Rect ();
          
          memset (&viewport_desc, 0, sizeof (viewport_desc));
          
          viewport_desc.x         = rect.x;
          viewport_desc.y         = rect.y;
          viewport_desc.width     = rect.width;
          viewport_desc.height    = rect.height;
          viewport_desc.min_depth = pass.viewport_min_depth;
          viewport_desc.max_depth = pass.viewport_max_depth;

          device.RSSetViewport (viewport_desc);
        }
        else
        {
          render::low_level::Viewport viewport_desc;
          
          memset (&viewport_desc, 0, sizeof (viewport_desc));
          
          viewport_desc.x         = 0;
          viewport_desc.y         = 0;
          viewport_desc.width     = target_width;
          viewport_desc.height    = target_height;
          viewport_desc.min_depth = pass.viewport_min_depth;
          viewport_desc.max_depth = pass.viewport_max_depth;
          
          device.RSSetViewport (viewport_desc);
        }

          //настройка области отсечения
          
        if (scissor)
        {
          render::low_level::Rect dst_rect;
          const Rect&             src_rect = scissor->Rect ();
          
          memset (&dst_rect, 0, sizeof (dst_rect));
          
          dst_rect.x      = src_rect.x;
          dst_rect.y      = src_rect.y;
          dst_rect.width  = src_rect.width;
          dst_rect.height = src_rect.height;
          
          device.RSSetScissor (dst_rect);
        }
        
        //TODO: check scissor enabled???
        
          //установка локальных текстур
          
        ///???
                
          //выполнение операций

        for (OperationArray::iterator iter=pass.operations.begin (), end=pass.operations.end (); iter!=end; ++iter)
        {
          const RendererOperation& operation = **iter;
          const RendererPrimitive& primitive = *operation.primitive;
          
            //применение состояния операции

          operation.state_block->Apply ();
          
            //установка расположения параметров
            //TODO: кэшировать по entity

//          ProgramParametersLayoutPtr program_parameters_layout = program_parameters_manager.GetParameters (???, *operation.parameters_layout);

//          device.SSSetProgramParametersLayout (&*program_parameters_layout->DeviceLayout ());

            //рисование

          if (primitive.indexed) device.DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
          else                   device.Draw        (primitive.type, primitive.first, primitive.count);
        }
      }

      if (operation.effect)
      {
          //рисование вложенных кадров
        
        for (FrameArray::iterator iter=operation.effect->frames.begin (), end=operation.effect->frames.end (); iter!=end; ++iter)
          (*iter)->Draw (&context);
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EffectRenderer::ExecuteOperations");
    throw;
  }
}
