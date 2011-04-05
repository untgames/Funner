#include "shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t RESERVE_OPERATION_ARRAY = 512; //резервируемое число операций в массиве операций прохода
const size_t RESERVE_FRAME_ARRAY     = 32;  //резервируемое число вложенных фреймов в эффекте

/*
    Вспомогательны структуры
*/

typedef stl::vector<const RendererOperation*> OperationArray;

///Проход рендеринга
struct RenderPass: public xtl::reference_counter
{  
  common::StringArray      color_targets;         //имя целевых буферов цвета
  stl::string              depth_stencil_target;  //имя целевого буфера отсечения
  SortMode                 sort_mode;             //режим сортировки
  LowLevelStateBlockPtr    state_block;           //блок состояний прохода
  OperationArray           operations;            //операции рендеринга
  const RendererOperation* last_operation;        //последняя добавленная операция

///Конструктор
  RenderPass ()
    : sort_mode (SortMode_Default)
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
  TagHashArray                effect_tags; //тэги эффекта
  RenderPassMap               passes;      //проходы рендеринга
  RenderInstantiatedEffectMap effects;     //инстанцированные эффекты
  RenderEffectOperationArray  operations;  //операции рендеринга
};

/*
    Конструктор / деструктор
*/

EffectRenderer::EffectRenderer (const EffectPtr& effect)
  : impl (new Impl)
{
  try
  {
    if (!effect)
      throw xtl::make_null_argument_exception ("", "effect");
      
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
        RenderPassPtr pass (new RenderPass, false);
        
        for (size_t j=0, count=src_pass->TagsCount (); j<count; j++)
        {
          size_t hash = src_pass->TagHash (j);
          
          impl->passes.insert_pair (hash, &*pass);
        }
        
        pass->color_targets        = src_pass->ColorTargets ().Clone ();
        pass->depth_stencil_target = src_pass->DepthStencilTarget ();
        pass->sort_mode            = src_pass->SortMode ();
        pass->state_block          = src_pass->StateBlock ();

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

void EffectRenderer::AddOperations (const RendererOperationList& operations_desc)
{
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
    Выполнение операций
*/

void EffectRenderer::ExecuteOperations ()
{
}
