#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации списка спрайтов
*/

struct SpriteList::Impl
{
  manager::Entity&                   entity;                //сущность
  RenderManager                      render_manager;        //менеджер рендеринга
  SpriteMode                         mode;                  //режим спрайтов
  PrimitiveUsage                     usage;                 //режим использования
  math::vec3f                        up;                    //вектор "вверх"
  stl::string                        batch_name;            //имя пакета
  stl::auto_ptr<manager::SpriteList> list;                  //список спрайтов
  size_t                             descs_count;           //количество спрайтов

/// Конструктор
  Impl (RenderManager& in_render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , render_manager (in_render_manager)
    , mode (interchange::SpriteMode_Billboard)
    , usage (interchange::PrimitiveUsage_Batching)
    , up (0, 1.0f, 0)
    , descs_count ()
  {
  }

/// Обновление списка спрайтов
  void ResetSpriteList (SpriteMode in_mode, PrimitiveUsage in_usage, const math::vec3f& up, const char* batch_name)
  {
    entity.ResetPrimitive ();

    size_t reserve_size = 0;
    
    stl::string material_name;

    if (list)
    {
      reserve_size  = list->Capacity ();
      material_name = list->Material ();
    }

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
        manager::PrimitiveBuffers buffers   = render_manager.BatchingManager ().GetBatch (batch_name);
        manager::Primitive        primitive = render_manager.Manager ().CreatePrimitive (buffers);

        list.reset (new manager::SpriteList (primitive.AddBatchingSpriteList (mode, up)));

        entity.SetPrimitive (primitive);

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

        manager::Primitive primitive = render_manager.Manager ().CreatePrimitive ();

        list.reset (new manager::SpriteList (primitive.AddStandaloneSpriteList (mode, up, usage, usage)));

        entity.SetPrimitive (primitive);

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "usage", in_usage);
    }

    if (reserve_size)    
      list->Reserve (reserve_size);

    list->SetMaterial (material_name.c_str ());
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
    Основные параметры
*/

void SpriteList::SetParams (SpriteMode in_mode, PrimitiveUsage in_usage, const math::vec3f& up, const char* batch)
{
  try
  {
    if (!batch)
      throw xtl::make_null_argument_exception ("", "batch");

    stl::string batch_str = batch;

    impl->ResetSpriteList (in_mode, in_usage, up, batch);

    impl->mode  = in_mode;
    impl->usage = in_usage;
    impl->up    = up;

    stl::swap (batch_str, impl->batch_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SetParams");
    throw;
  }
}

SpriteMode SpriteList::Mode () const
{
  return impl->mode;
}

PrimitiveUsage SpriteList::Usage () const
{
  return impl->usage;
}

const math::vec3f& SpriteList::OrtUp () const
{
  return impl->up;
}

const char* SpriteList::Batch () const
{
  return impl->batch_name.c_str ();
}

/*
    Имя материала
*/

void SpriteList::SetMaterial (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't set material for sprite list '%s'. Call SetParams first", Name ());

    impl->list->SetMaterial (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SetMaterial");
    throw;
  }
}

const char* SpriteList::Material () const
{
  return impl->list ? impl->list->Material () : "";
}

/*
    Количество спрайтов / размер буфера
*/

size_t SpriteList::Size () const
{
  return impl->descs_count;
}

size_t SpriteList::Capacity () const
{
  return impl->list ? impl->list->Capacity () : 0;
}

/*
    Изменение размера списка спрайтов / резервирование места для хранения спрайтов
*/

void SpriteList::Resize (size_t count)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't resize/reserve for sprite list '%s'. Call SetParams first", Name ());

    if (count < impl->list->Size ())
      impl->list->Remove (count, impl->list->Size () - count);

    impl->descs_count = count;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::Resize");
    throw;
  }
}

void SpriteList::Reserve (size_t count)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't resize/reserve for sprite list '%s'. Call SetParams first", Name ());

    impl->list->Reserve (count);
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

void SpriteList::SetDescs (size_t first, size_t count, const SpriteDesc* descs)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't set descs for sprite list '%s'. Call SetParams first", Name ());

    if (first >= impl->descs_count)
      throw xtl::make_range_exception ("", "first", first, impl->descs_count);

    if (first + count > impl->descs_count)
      throw xtl::make_range_exception ("", "count", count, impl->descs_count);

    size_t real_count   = impl->list->Size (),
           update_count = real_count >= first ? real_count - first : 0,
           add_count    = count - update_count;

    static const int ct_check = xtl::compile_time_assert<sizeof (manager::Sprite) == sizeof (interchange::SpriteDesc)>::value;

    if (update_count)
      impl->list->Update (first, update_count, reinterpret_cast<const manager::Sprite*> (descs));

    if (add_count)
      impl->list->Add (add_count, reinterpret_cast<const manager::Sprite*> (descs) + update_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::SpriteList::SetDescs");
    throw;
  }
}
