#include "shared.h"

using namespace render;

/*
    Описание реализации эффекта
*/

typedef stl::vector<size_t> TagHashArray;

struct Effect::Impl
{
  common::StringArray tags;       //тэги прохода
  TagHashArray        tag_hashes; //хэши тэгов  
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
    throw xtl::make_null_argument_exception ("render::Effect::SetTags", "tags");

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
    throw xtl::make_range_exception ("render::Effect::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t Effect::TagHash (size_t index)
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("render::Effect::TagHash", "index", index, impl->tag_hashes.size ());
    
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

size_t Effect::PassesCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Effect::ChildrenCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Effect::AddOperation (const EffectPassPtr&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Effect::AddOperation (const EffectPtr&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Effect::RemoveOperation (size_t order_number)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Effect::RemoveAllOperations ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Создание рендера эффекта
*/

EffectRendererPtr Effect::CreateRenderer ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
