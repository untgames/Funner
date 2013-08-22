#include "shared.h"

using namespace render::manager;

namespace
{

/*
    Константы
*/

const size_t RESERVE_OPERATIONS_COUNT = 16;

/*
    Операция эффекта
*/

struct EffectOperation: public xtl::reference_counter
{
  EffectPassPtr         pass;   //проход, осуществляемый при выполнении операции
  InstantiatedEffectPtr effect; //эффект, осуществляемый при выполнении операции

  EffectOperation (const EffectPassPtr& in_pass) : pass (in_pass) {}

  EffectOperation (const InstantiatedEffectPtr& in_effect) : effect (in_effect) {}
};

typedef xtl::intrusive_ptr<EffectOperation> EffectOperationPtr;
typedef stl::vector<EffectOperationPtr>     EffectOperationArray;

}

/*
    Описание реализации эффекта
*/

typedef stl::vector<size_t> TagHashArray;

struct Effect::Impl
{
  common::StringArray  tags;       //тэги прохода
  TagHashArray         tag_hashes; //хэши тэгов
  EffectOperationArray operations; //операции рендеринга
  
  Impl ()
  {
    operations.reserve (RESERVE_OPERATIONS_COUNT);
  }
};

/*
    Конструктор / деструктор
*/

Effect::Effect ()
  : impl (new Impl)
{
}

Effect::~Effect ()
{
}

/*
    Тэги фрейма: используются для рекурсивного рендеринга
*/

void Effect::SetTags (const char* tags)
{
  if (!tags)
    throw xtl::make_null_argument_exception ("render::manager::Effect::SetTags", "tags");

  SetTags (common::split (tags, " ")); 
}

void Effect::SetTags (const common::StringArray& tags)
{
  TagHashArray new_tag_hashes;
  
  new_tag_hashes.resize (tags.Size ());
  
  for (size_t i=0, count=tags.Size (); i<count; i++)
    new_tag_hashes [i] = common::strhash (tags [i]);
    
  impl->tags = tags;

  impl->tag_hashes.swap (new_tag_hashes);
}

size_t Effect::TagsCount ()
{
  return impl->tags.Size ();
}

const char* Effect::Tag (size_t index)
{
  if (index >= impl->tags.Size ())
    throw xtl::make_range_exception ("render::manager::Effect::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t Effect::TagHash (size_t index)
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("render::manager::Effect::TagHash", "index", index, impl->tag_hashes.size ());
    
  return impl->tag_hashes [index];
}

const char** Effect::Tags ()
{
  if (impl->tags.IsEmpty ())
    return 0;
    
  return impl->tags.Data ();
}

const size_t* Effect::TagHashes ()
{
  if (impl->tag_hashes.empty ())
    return 0;
    
  return &impl->tag_hashes [0];
}
/*
    Проходы рендеринга / вложенные эффекты
*/

size_t Effect::OperationsCount ()
{
  return impl->operations.size ();
}

EffectPassPtr Effect::OperationPass (size_t order_number)
{
  if (order_number >= impl->operations.size ())
    throw xtl::make_range_exception ("render::manager::Effect::OperationPass", "order_number", order_number, impl->operations.size ());
    
  EffectOperationPtr operation = impl->operations [order_number];
  
  if (!operation || !operation->pass)
    return EffectPassPtr ();

  return operation->pass;
}

InstantiatedEffectPtr Effect::OperationEffect (size_t order_number)
{
  if (order_number >= impl->operations.size ())
    throw xtl::make_range_exception ("render::manager::Effect::OperationEffect", "order_number", order_number, impl->operations.size ());
    
  EffectOperationPtr operation = impl->operations [order_number];
  
  if (!operation || !operation->effect)
    return InstantiatedEffectPtr ();

  return operation->effect;
}

size_t Effect::AddOperation (const EffectPassPtr& pass)
{
  impl->operations.push_back (EffectOperationPtr (new EffectOperation (pass), false));
  
  return impl->operations.size () - 1;
}

size_t Effect::AddOperation (const InstantiatedEffectPtr& effect)
{
  impl->operations.push_back (EffectOperationPtr (new EffectOperation (effect), false));

  return impl->operations.size () - 1;
}

void Effect::RemoveOperation (size_t order_number)
{
  if (order_number >= impl->operations.size ())
    return;

  impl->operations.erase (impl->operations.begin () + order_number);
}

void Effect::RemoveAllOperations ()
{
  impl->operations.clear ();
}
