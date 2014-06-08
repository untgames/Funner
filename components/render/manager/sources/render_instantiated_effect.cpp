#include "shared.h"

using namespace render::manager;

/*
    Описание реализации инстанцированной эффекта
*/

typedef stl::vector<size_t> TagHashArray;

struct InstantiatedEffect::Impl
{
  common::StringArray tags;       //тэги прохода
  TagHashArray        tag_hashes; //хэши тэгов  
};

/*
    Конструктор / деструктор
*/

InstantiatedEffect::InstantiatedEffect ()
  : impl (new Impl)
{
}

InstantiatedEffect::~InstantiatedEffect ()
{
}

/*
    Тэги фрейма: используются для рекурсивного рендеринга
*/

void InstantiatedEffect::SetTags (const char* tags)
{
  if (!tags)
    throw xtl::make_null_argument_exception ("render::manager::InstantiatedEffect::SetTags", "tags");

  SetTags (common::split (tags, " ")); 
}

void InstantiatedEffect::SetTags (const common::StringArray& tags)
{
  TagHashArray new_tag_hashes;
  
  new_tag_hashes.resize (tags.Size ());
  
  for (size_t i=0, count=tags.Size (); i<count; i++)
    new_tag_hashes [i] = common::strhash (tags [i]);
    
  impl->tags = tags;

  impl->tag_hashes.swap (new_tag_hashes);
}

size_t InstantiatedEffect::TagsCount ()
{
  return impl->tags.Size ();
}

const char* InstantiatedEffect::Tag (size_t index)
{
  if (index >= impl->tags.Size ())
    throw xtl::make_range_exception ("render::manager::InstantiatedEffect::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t InstantiatedEffect::TagHash (size_t index)
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("render::manager::InstantiatedEffect::TagHash", "index", index, impl->tag_hashes.size ());
    
  return impl->tag_hashes [index];
}

const char** InstantiatedEffect::Tags ()
{
  if (impl->tags.IsEmpty ())
    return 0;
    
  return impl->tags.Data ();
}

const size_t* InstantiatedEffect::TagHashes ()
{
  if (impl->tag_hashes.empty ())
    return 0;
    
  return &impl->tag_hashes [0];
}
