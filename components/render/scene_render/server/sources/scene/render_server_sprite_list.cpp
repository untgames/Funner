#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Константы
*/

const size_t RESERVE_SUBLIST_COUNT = 4; //резервируемое количество вложенных списков

/*
    Описание реализации списка спрайтов
*/

namespace
{

/// Вложенный список спрайтов
struct Sublist: public xtl::reference_counter
{
  SpriteMode          mode;         //режим спрайтов
  PrimitiveUsage      usage;        //режим использования
  math::vec3f         up;           //вектор "вверх"
  stl::string         batch_name;   //имя пакета
  size_t              descs_count;  //количество спрайтов
  manager::SpriteList sprites;      //спрайты  

/// Конструктор
  Sublist (SpriteMode in_mode, PrimitiveUsage in_usage, const char* in_batch_name, const manager::SpriteList& in_sprites)
    : mode (in_mode)
    , usage (in_usage)
    , batch_name (in_batch_name)
    , descs_count ()
    , sprites (in_sprites)
  {
  }
};

typedef xtl::intrusive_ptr<Sublist> SublistPtr;
typedef stl::vector<SublistPtr>     SublistArray;

}

struct SpriteList::Impl
{
  manager::Entity&  entity;                //сущность
  RenderManager     render_manager;        //менеджер рендеринга
  SublistArray      sublists;              //вложенные списки
  size_t            active_sublists_count; //количество активных списков

/// Конструктор
  Impl (RenderManager& in_render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , render_manager (in_render_manager)
    , active_sublists_count ()
  {
    sublists.reserve (RESERVE_SUBLIST_COUNT);
  }

/// Получение вложенного списка
  Sublist& Item (subid_t subid, const char* source = "")
  {
    if (subid < sublists.size ())
    {
      SublistPtr& list = sublists [subid];

      if (list)
        return *list;

      throw xtl::format_operation_exception ("", "Sublist with subid %u has not been initialized", subid);
    }

    throw xtl::make_range_exception (source, "subid", subid, sublists.size ());
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

SpriteList::SpriteList (RenderManager& render_manager)
  : VisualModel (render_manager)
{
  try
  {
    impl.reset (new Impl (render_manager, Entity ()));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SpriteList");
    throw;
  }
}

SpriteList::~SpriteList ()
{
}

/*
    Создание / удаление списка
*/

void SpriteList::CreateList (subid_t subid, SpriteMode in_mode, PrimitiveUsage in_usage, const math::vec3f& up, const char* batch)
{
  try
  {
    if (!batch)
      throw xtl::make_null_argument_exception ("", "batch");

    bool need_create_primitive = !impl->entity.HasPrimitive ();

    for (SublistArray::iterator begin=impl->sublists.begin (), iter=begin, end=impl->sublists.end (); iter!=end; ++iter)
      if (iter-begin != subid && *iter && (*iter)->batch_name != batch)
        throw xtl::format_operation_exception ("", "Attempt to add sublist with batch '%s' to sprite list with batch '%s'", batch, (*iter)->batch_name.c_str ());

    if (subid >= impl->sublists.size ())
      impl->sublists.resize (subid + 1);

    SublistPtr& list = impl->sublists [subid];

    bool is_active = list != SublistPtr ();

    manager::SpriteMode mode;

    switch (in_mode)
    {
      case interchange::SpriteMode_Billboard:         mode = manager::SpriteMode_Billboard; break;
      case interchange::SpriteMode_Oriented:          mode = manager::SpriteMode_Oriented; break;
      case interchange::SpriteMode_OrientedBillboard: mode = manager::SpriteMode_OrientedBillboard; break;
      default:                                        throw xtl::make_argument_exception ("", "mode", in_mode);
    }

    switch (in_usage)
    {
      case interchange::PrimitiveUsage_Batching:
      {
        if (need_create_primitive)
        {      
          manager::PrimitiveBuffers buffers   = impl->render_manager.BatchingManager ().GetBatch (batch);
          manager::Primitive        primitive = impl->render_manager.Manager ().CreatePrimitive (buffers);

          SublistPtr new_list (new Sublist (in_mode, in_usage, batch, primitive.AddBatchingSpriteList (mode, up)), false);

          impl->entity.SetPrimitive (primitive);

          list = new_list;
        }
        else
        {
          list = SublistPtr (new Sublist (in_mode, in_usage, batch, impl->entity.Primitive ().AddBatchingSpriteList (mode, up)), false);
        }

        break;
      }
      case interchange::PrimitiveUsage_Static:
      case interchange::PrimitiveUsage_Dynamic:
      case interchange::PrimitiveUsage_Stream:
      {
        manager::MeshBufferUsage usage;

        switch (in_usage)
        {
          default:
          case interchange::PrimitiveUsage_Static:  usage = manager::MeshBufferUsage_Static; break;
          case interchange::PrimitiveUsage_Dynamic: usage = manager::MeshBufferUsage_Dynamic; break;
          case interchange::PrimitiveUsage_Stream:  usage = manager::MeshBufferUsage_Stream; break;
        }

        if (need_create_primitive)
        {
          manager::Primitive primitive = impl->render_manager.Manager ().CreatePrimitive ();

          SublistPtr new_list (new Sublist (in_mode, in_usage, batch, primitive.AddStandaloneSpriteList (mode, up, usage, usage)), false);

          impl->entity.SetPrimitive (primitive);

          list = new_list;
        }
        else
        {
          list = SublistPtr (new Sublist (in_mode, in_usage, batch, impl->entity.Primitive ().AddStandaloneSpriteList (mode, up, usage, usage)), false);
        }

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "usage", in_usage);
    }

    if (!is_active)
      impl->active_sublists_count++;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::CreateList");
    throw;
  }
}

void SpriteList::RemoveList (subid_t subid)
{
  if (subid >= impl->sublists.size ())
    return;

  SublistPtr& list = impl->sublists [subid];

  bool is_active = list != SublistPtr ();

  list = SublistPtr ();

  if (is_active)
    impl->active_sublists_count--;

  if (!impl->active_sublists_count)
    impl->entity.ResetPrimitive ();
}

void SpriteList::ReserveLists (subid_t count)
{
  impl->sublists.reserve (count);
}

/*
    Основные параметры
*/

SpriteMode SpriteList::Mode (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Mode").mode;
}

PrimitiveUsage SpriteList::Usage (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Usage").usage;
}

const math::vec3f& SpriteList::OrtUp (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::OrtUp").up;
}

const char* SpriteList::Batch (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Batch").batch_name.c_str ();
}

/*
    Имя материала
*/

void SpriteList::SetMaterial (subid_t subid, const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->Item (subid).sprites.SetMaterial (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SetMaterial");
    throw;
  }
}

const char* SpriteList::Material (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Material").sprites.Material ();
}

/*
    Количество спрайтов / размер буфера
*/

size_t SpriteList::Size (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Size").descs_count;
}

size_t SpriteList::Capacity (subid_t subid) const
{
  return impl->Item (subid, "render::scene::server::SpriteList::Capacity").sprites.Capacity ();
}

/*
    Изменение размера списка спрайтов / резервирование места для хранения спрайтов
*/

void SpriteList::Resize (subid_t subid, size_t count)
{
  try
  {
    Sublist& list = impl->Item (subid);

    if (count < list.sprites.Size ())
      list.sprites.Remove (count, list.sprites.Size () - count);

    list.descs_count = count;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::Resize");
    throw;
  }
}

void SpriteList::Reserve (subid_t subid, size_t count)
{
  try
  {
    Sublist& list = impl->Item (subid);

    list.sprites.Reserve (count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::Reserve");
    throw;
  }
}

/*
    Обновление дескрипторов спрайтов
*/

void SpriteList::SetDescs (subid_t subid, size_t first, size_t count, const SpriteDesc* descs)
{
  try
  {
    Sublist& list = impl->Item (subid);

    if (first >= list.descs_count)
      throw xtl::make_range_exception ("", "first", first, list.descs_count);

    if (first + count > list.descs_count)
      throw xtl::make_range_exception ("", "count", count, list.descs_count);

    size_t real_count   = list.sprites.Size (),
           update_count = real_count >= first ? real_count - first : 0,
           add_count    = count - update_count;

    static const int ct_check = xtl::compile_time_assert<sizeof (manager::Sprite) == sizeof (interchange::SpriteDesc)>::value;

    if (update_count)
      list.sprites.Update (first, update_count, reinterpret_cast<const manager::Sprite*> (descs));

    if (add_count)
      list.sprites.Add (add_count, reinterpret_cast<const manager::Sprite*> (descs) + update_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SetDescs");
    throw;
  }
}
