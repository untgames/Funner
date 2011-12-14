#include "shared.h"

using namespace media::rfx;

const size_t TEXMAP_RESERVE_SIZE = 8; //резервируемое количество текстурных карт

/*
    Описание реализации Material
*/

typedef stl::vector<Texmap> TexmapArray;
typedef stl::vector<size_t> TagHashArray;

struct Material::Impl: public xtl::reference_counter
{
  stl::string         name;       //имя материала
  stl::string         program;    //имя программы
  common::StringArray tags;       //тэги материала  
  TagHashArray        tag_hashes; //хэши тэгов
  common::PropertyMap properties; //свойства материала
  TexmapArray         texmaps;    //текстурные карты  
  
  Impl ()
  {
    texmaps.reserve (TEXMAP_RESERVE_SIZE);
  }

  Impl (const Impl& impl)
    : name (impl.name)
    , program (impl.program)
    , tags (impl.tags.Clone ())
    , tag_hashes (impl.tag_hashes)
    , properties (impl.properties.Clone ())
  {
    texmaps.reserve (impl.texmaps.size ());
    
    for (size_t i=0; i<impl.texmaps.size (); i++)
      texmaps.push_back (impl.texmaps [i].Clone ());
  }
};

/*
    Конструкторы / деструктор
*/

Material::Material ()
  : impl (new Impl)
{
}

Material::Material (Impl* in_impl)
  : impl (in_impl)
{
}

Material::Material (const Material& material)
  : impl (material.impl)
{
  addref (impl);
}

Material::~Material ()
{
  release (impl);
}

Material& Material::operator = (const Material& material)
{
  Material (material).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

Material Material::Clone () const
{
  try
  {
    return Material (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rfx::Material::Clone");
    throw;
  }
}

/*
    Идентификатор
*/
   
size_t Material::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя материала
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

void Material::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Material::SetName", "name");
        
  impl->name = name;
}

/*
    Имя программы
*/

void Material::SetProgram (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Material::SetProgram", "name");
    
  impl->program = name;
}

const char* Material::Program () const
{
  return impl->program.c_str ();
}

/*
    Свойства материала
*/

common::PropertyMap& Material::Properties ()
{
  return impl->properties;
}

const common::PropertyMap& Material::Properties () const
{
  return impl->properties;
}

/*
    Тэги
*/

void Material::SetTags (const char* tags)
{
  if (!tags)
    throw xtl::make_null_argument_exception ("media::rfx::Material::SetTags", "tags");
    
  SetTags (common::split (tags, " "));
}

void Material::SetTags (const common::StringArray& tags)
{
  TagHashArray new_tag_hashes;
  
  new_tag_hashes.resize (tags.Size ());
  
  for (size_t i=0, count=tags.Size (); i<count; i++)
    new_tag_hashes [i] = common::strhash (tags [i]);
    
  impl->tags = tags;

  impl->tag_hashes.swap (new_tag_hashes);
}

size_t Material::TagsCount () const
{
  return impl->tags.Size ();
}

const char* Material::Tag (size_t index) const
{
  if (index >= impl->tags.Size ())
    throw xtl::make_range_exception ("media::rfx::Material::Tag", "index", index, impl->tags.Size ());
    
  return impl->tags [index];
}

size_t Material::TagHash (size_t index) const
{
  if (index >= impl->tag_hashes.size ())
    throw xtl::make_range_exception ("media::rfx::Material::TagHash", "index", index, impl->tag_hashes.size ());
    
  return impl->tag_hashes [index];
}

const char** Material::Tags () const
{
  if (impl->tags.IsEmpty ())
    return 0;
    
  return impl->tags.Data ();
}

const size_t* Material::TagHashes () const
{
  if (impl->tag_hashes.empty ())
    return 0;
    
  return &impl->tag_hashes [0];
}

/*
    Текстурные карты
*/

size_t Material::TexmapCount () const
{
  return impl->texmaps.size ();
}

media::rfx::Texmap& Material::Texmap (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("media::rfx::Material::Texmap", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index];
}

const media::rfx::Texmap& Material::Texmap (size_t index) const
{
  return const_cast<Material&> (*this).Texmap (index);
}

void Material::SetTexmap (size_t index, const media::rfx::Texmap& texmap)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("media::rfx::Material::SetTexmap", "index", index, impl->texmaps.size ());
  
  impl->texmaps [index] = texmap;
}

size_t Material::AddTexmap (const media::rfx::Texmap& texmap)
{
  impl->texmaps.push_back (texmap);
  
  return impl->texmaps.size () - 1;
}

void Material::RemoveTexmap (size_t index)
{
  if (index >= impl->texmaps.size ())
    return;
    
  impl->texmaps.erase (impl->texmaps.begin () + index);
}

void Material::RemoveAllTexmaps ()
{
  impl->texmaps.clear ();
}

/*
    Обмен
*/

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);
}

namespace media
{

namespace rfx
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
