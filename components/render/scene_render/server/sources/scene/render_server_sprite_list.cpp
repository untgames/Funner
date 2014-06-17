#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

//TODO: add shared primitive buffers support

/*
    �������� ���������� ������ ��������
*/

struct SpriteList::Impl
{
  manager::Entity&                   entity;         //��������
  SpriteMode                         mode;           //����� ��������
  PrimitiveUsage                     usage;          //����� �������������
  math::vec3f                        up;             //������ "�����"
  manager::Primitive                 primitive;      //�������� ����������
  stl::auto_ptr<manager::SpriteList> list;           //������ ��������
  size_t                             descs_count;    //���������� ��������

/// �����������
  Impl (RenderManager& render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , mode (interchange::SpriteMode_Billboard)
    , usage (interchange::PrimitiveUsage_Batching)
    , up (0, 1.0f, 0)
    , primitive (render_manager.Manager ().CreatePrimitive ()) //TODO: shared buffers
    , descs_count ()
  {
    entity.SetPrimitive (primitive);

    primitive.Buffers ().ReserveDynamicBuffers (8192, 8192); //TODO: remove after shared buffers implementation
  }
};

/*
    ������������ / ���������� / ������������
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
    �������� ���������
*/

void SpriteList::SetParams (SpriteMode in_mode, PrimitiveUsage in_usage, const math::vec3f& up)
{
  try
  {
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
        impl->list.reset (new manager::SpriteList (impl->primitive.AddBatchingSpriteList (mode, up)));
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

        impl->list.reset (new manager::SpriteList (impl->primitive.AddStandaloneSpriteList (mode, up, usage, usage)));

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "usage", in_usage);
    }

    impl->mode  = in_mode;
    impl->usage = in_usage;
    impl->up    = up;
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

/*
    ��� ���������
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
    ���������� �������� / ������ ������
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
    ��������� ������� ������ �������� / �������������� ����� ��� �������� ��������
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
    ���������� ������������ ��������
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
