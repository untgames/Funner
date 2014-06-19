#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации списка спрайтов
*/

struct LineList::Impl
{
  manager::Entity&                   entity;                //сущность
  RenderManager                      render_manager;        //менеджер рендеринга
  PrimitiveUsage                     usage;                 //режим использования
  stl::string                        batch_name;            //имя пакета
  bool                               need_create_primitive; //требование пересоздать примитив
  stl::auto_ptr<manager::LineList>   list;                  //список спрайтов
  size_t                             descs_count;           //количество спрайтов

/// Конструктор
  Impl (RenderManager& in_render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , render_manager (in_render_manager)
    , usage (interchange::PrimitiveUsage_Batching)
    , need_create_primitive (true)
    , descs_count ()
  {
  }

/// Сброс примитива
  manager::Primitive GetPrimitive ()
  {
    try
    {
      if (!need_create_primitive)
        return entity.Primitive ();

      manager::PrimitiveBuffers buffers   = render_manager.BatchingManager ().GetBatch (batch_name.c_str ());
      manager::Primitive        primitive = render_manager.Manager ().CreatePrimitive (buffers);

      entity.SetPrimitive (primitive);

      need_create_primitive = false;

      return primitive;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::LineList::Impl::GetPrimitive");
      throw;
    }
  }

/// Обновление списка спрайтов
  void ResetLineList (PrimitiveUsage in_usage)
  {
    size_t reserve_size = 0;
    
    stl::string material_name;

    if (list)
    {
      reserve_size  = list->Capacity ();
      material_name = list->Material ();
    }

    manager::Primitive primitive = GetPrimitive ();

    switch (in_usage)
    {
      case interchange::PrimitiveUsage_Batching:
      {
        list.reset (new manager::LineList (primitive.AddBatchingLineList ()));
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

        list.reset (new manager::LineList (primitive.AddStandaloneLineList (usage, usage)));

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "usage", in_usage);
    }

    if (reserve_size)    
      list->Reserve (reserve_size);

    list->SetMaterial (material_name.c_str ());
  }

  void ResetLineList ()
  {
    ResetLineList (usage);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

LineList::LineList (RenderManager& render_manager)
  : VisualModel (render_manager)
{
  try
  {
    impl.reset (new Impl (render_manager, Entity ()));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::LineList");
    throw;
  }
}

LineList::~LineList ()
{
}

/*
    Основные параметры
*/

void LineList::SetParams (PrimitiveUsage in_usage)
{
  try
  {
    impl->ResetLineList (in_usage);

    impl->usage = in_usage;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::SetParams");
    throw;
  }
}

PrimitiveUsage LineList::Usage () const
{
  return impl->usage;
}

/*
    Имя материала
*/

void LineList::SetMaterial (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't set material for line list '%s'. Call SetParams first", Name ());

    impl->list->SetMaterial (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::SetMaterial");
    throw;
  }
}

const char* LineList::Material () const
{
  return impl->list ? impl->list->Material () : "";
}

/*
    Имя пакета
*/

void LineList::SetBatch (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->batch_name = name;

    bool need_reset = !impl->need_create_primitive;

    impl->need_create_primitive = true;
 
    if (need_reset)
      impl->ResetLineList ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::SetMaterial");
    throw;
  }
}

const char* LineList::Batch () const
{
  return impl->batch_name.c_str ();
}

/*
    Количество спрайтов / размер буфера
*/

size_t LineList::Size () const
{
  return impl->descs_count;
}

size_t LineList::Capacity () const
{
  return impl->list ? impl->list->Capacity () : 0;
}

/*
    Изменение размера списка спрайтов / резервирование места для хранения спрайтов
*/

void LineList::Resize (size_t count)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't resize/reserve for line list '%s'. Call SetParams first", Name ());

    if (count < impl->list->Size ())
      impl->list->Remove (count, impl->list->Size () - count);

    impl->descs_count = count;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::Resize");
    throw;
  }
}

void LineList::Reserve (size_t count)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't resize/reserve for line list '%s'. Call SetParams first", Name ());

    impl->list->Reserve (count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::Reserve");
    throw;
  }
}

/*
    Обновление дескрипторов спрайтов
*/

void LineList::SetDescs (size_t first, size_t count, const LineDesc* descs)
{
  try
  {
    if (!impl->list)
      throw xtl::format_operation_exception ("", "Can't set descs for line list '%s'. Call SetParams first", Name ());

    if (first >= impl->descs_count)
      throw xtl::make_range_exception ("", "first", first, impl->descs_count);

    if (first + count > impl->descs_count)
      throw xtl::make_range_exception ("", "count", count, impl->descs_count);

    size_t real_count   = impl->list->Size (),
           update_count = real_count >= first ? real_count - first : 0,
           add_count    = count - update_count;

    static const int ct_check = xtl::compile_time_assert<sizeof (manager::Line) == sizeof (interchange::LineDesc)>::value;

    if (update_count)
      impl->list->Update (first, update_count, reinterpret_cast<const manager::Line*> (descs));

    if (add_count)
      impl->list->Add (add_count, reinterpret_cast<const manager::Line*> (descs) + update_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::LineList::SetDescs");
    throw;
  }
}
