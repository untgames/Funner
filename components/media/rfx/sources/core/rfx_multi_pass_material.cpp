#include "shared.h"

using namespace media::rfx;
using namespace stl;
using namespace common;

namespace
{

const size_t DEFAULT_MATERIAL_PASSES_RESERVE = 4; //резервируемое количество проходов

/*
    Слот для прохода визуализации материала
*/

struct PassImpl
{
  Material::Pointer material; //материал
  PassState         state;    //состояние прохода
  
  PassImpl (const Material::Pointer& in_material, PassState in_state=PassState_Enabled)
    : material (in_material), state (in_state) {}
};

typedef vector<PassImpl> PassArray;

}

/*
    Описание реализации MultiPassMaterial
*/

struct MultiPassMaterial::Impl
{
  PassArray passes; //проходы
  
  Impl () { passes.reserve (DEFAULT_MATERIAL_PASSES_RESERVE); }
};

/*
    Конструкторы / деструктор / присваивание
*/

MultiPassMaterial::MultiPassMaterial ()
  : impl (new Impl)
  {}

MultiPassMaterial::MultiPassMaterial (const MultiPassMaterial& mtl)
  : media::rfx::Material (mtl), impl (new Impl (*mtl.impl))
  {}

MultiPassMaterial::~MultiPassMaterial ()
{
}

/*
    Создание
*/

MultiPassMaterial::Pointer MultiPassMaterial::Create ()
{
  return Pointer (new MultiPassMaterial, false);
}

/*
    Динамическая диспетчеризация
*/

void MultiPassMaterial::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    media::rfx::Material::AcceptCore (visitor);
}

/*
    Реализация копирования
*/

media::rfx::Material* MultiPassMaterial::CloneCore () const
{
  return new MultiPassMaterial (*this);
}

/*
    Количество проходов
*/

size_t MultiPassMaterial::PassesCount () const
{
  return impl->passes.size ();
}

/*
    Получение материалов
*/

media::rfx::Material::ConstPointer MultiPassMaterial::Material (size_t index) const
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::Material", "index", index, impl->passes.size ());
    
  return impl->passes [index].material;
}

media::rfx::Material::Pointer MultiPassMaterial::Material (size_t index)
{
  return xtl::const_pointer_cast<media::rfx::Material> (const_cast<const MultiPassMaterial&> (*this).Material (index));
}

/*
    Добавление / удаление проходов
*/

void MultiPassMaterial::InsertPass (size_t index, const Material::Pointer& material_ptr)
{
  if (index > impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::InsertPass", "index", index, impl->passes.size ());

  if (!material_ptr)
    RaiseNullArgument ("media::rfx::MultiPassMaterial::InsertPass", "material_ptr");

  impl->passes.insert (impl->passes.begin () + index, PassImpl (material_ptr));
}

size_t MultiPassMaterial::AddPass (const Material::Pointer& material_ptr)
{
  InsertPass (impl->passes.size (), material_ptr);

  return impl->passes.size () - 1;
}

void MultiPassMaterial::RemovePass (size_t index)
{
  if (index >= impl->passes.size ())
    return;
    
  impl->passes.erase (impl->passes.begin () + index);
}

void MultiPassMaterial::RemoveAllPasses ()
{
  impl->passes.clear ();
}

/*
    Установка шейдера
*/

void MultiPassMaterial::SetMaterial (size_t index, const Material::Pointer& material_ptr)
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::SetMaterial", "index", index, impl->passes.size ());

  if (!material_ptr)
    RaiseNullArgument ("media::rfx::MultiPassMaterial::SetMaterial", "material_ptr");

  impl->passes [index].material = material_ptr;
}

/*
    Перемемещение прохода
*/

void MultiPassMaterial::MovePass (size_t source_index, size_t destination_index)
{
  if (source_index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::MovePass", "source_index", source_index, impl->passes.size ());

  if (destination_index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::MovePass", "destination_index", destination_index, impl->passes.size ());
    
  PassImpl slot = impl->passes [source_index];
    
  impl->passes.erase (impl->passes.begin () + source_index);
  impl->passes.insert (impl->passes.begin () + destination_index, slot);
}

void MultiPassMaterial::SwapPasses (size_t index1, size_t index2)
{
  if (index1 >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::SwapPasses", "index1", index1, impl->passes.size ());

  if (index2 >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::SwapPasses", "index2", index2, impl->passes.size ());
    
  stl::swap (impl->passes [index1], impl->passes [index2]);
}

/*
    Поиск проходов
*/

int MultiPassMaterial::FindPass (const char* name) const
{
  if (!name)
    RaiseNullArgument ("media::rfx::MultiPassMaterial::FindPass", "name");
    
  size_t hash = strhash (name);
  
  for (PassArray::const_iterator i=impl->passes.begin (), end=impl->passes.end (); i!=end; ++i)
    if (i->material->NameHash () == hash)
      return i - impl->passes.begin ();

  return -1;
}

/*
    Поиск шейдера
*/

Material::ConstPointer MultiPassMaterial::FindMaterial (const char* name) const
{
  int pass_index = FindPass (name);
  
  if (pass_index == -1)
    return 0;
    
  return impl->passes [pass_index].material;
}

Material::Pointer MultiPassMaterial::FindMaterial (const char* name)
{
  return xtl::const_pointer_cast<media::rfx::Material> (const_cast<const MultiPassMaterial&> (*this).FindMaterial (name));
}

/*
    Разрешение / запрещение проходов
*/

media::rfx::PassState MultiPassMaterial::PassState (size_t index) const
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::PassState", "index", index, impl->passes.size ());
    
  return impl->passes [index].state;
}

void MultiPassMaterial::SetPassState (size_t index, media::rfx::PassState state)
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::MultiPassMaterial::SetPassState", "index", index, impl->passes.size ());

  switch (state)
  {
    case PassState_Enabled:
    case PassState_Disabled:
      break;
    default:
      RaiseInvalidArgument ("media::rfx::MultiPassMaterial::SetPassState", "state", state);
      break;
  }

  impl->passes [index].state = state;
}

/*
    Перечисление проходов
*/

void MultiPassMaterial::VisitEachPass (Visitor& visitor)
{
  for (PassArray::iterator i=impl->passes.begin (), end=impl->passes.end (); i!=end; ++i)
    if (i->state)
      i->material->Accept (visitor);
}
