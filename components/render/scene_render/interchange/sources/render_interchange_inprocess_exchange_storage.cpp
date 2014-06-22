#include "shared.h"

using namespace render::scene::interchange;

namespace
{

using detail::Attachment;
using detail::BasicAttachment;

/// Реализация внутрипроцессного хранилища
class InprocessExchangeStorageImpl
{
  public:
/// Конструктор
    InprocessExchangeStorageImpl ()
      : current_owner_id (1)
      , current_object_id (1)
    {
    }

/// Деструктор
    ~InprocessExchangeStorageImpl ()
    {
      while (!attachments.empty ())
      {
        delete attachments.begin ()->second;

        attachments.erase (attachments.begin ());
      }
    }

///Выделение идентификатора владельца
    object_id_t AllocateOwnerId ()
    {
      if (!current_owner_id)
        throw xtl::format_operation_exception ("render::scene::interchange::InprocessExchangeStorageImpl::AllocateOwnerId", "Id pool overflow");

      return current_owner_id++;
    }

/// Регистрация объекта
    object_id_t Attach (stl::auto_ptr<BasicAttachment>& attachment)
    {
      if (!current_object_id)
        throw xtl::format_operation_exception ("render::scene::interchange::InprocessExchangeStorageImpl::Attach", "Id pool overflow");

      attachments [current_object_id] = attachment.release ();

      return current_object_id++;
    }

    void Detach (object_id_t id)
    {
      AttachmentMap::iterator iter = attachments.find (id);

      if (iter == attachments.end ())
        return;

      delete iter->second;

      attachments.erase (iter);
    }

    void DetachAllWithOwner (object_id_t owner_id)
    {
      for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end ();)
        if (iter->second->owner_id == owner_id)
        {
          AttachmentMap::iterator next = iter;

          ++next;

          delete iter->second;

          attachments.erase (iter);

          iter = next;
        }
        else ++iter;
    }

    BasicAttachment* FindAttachment (object_id_t id, const std::type_info& type)
    {
      AttachmentMap::iterator iter = attachments.find (id);

      if (iter == attachments.end ())
        return 0;

      BasicAttachment* attachment = iter->second;

      if (attachment->type != &type)
        return 0;

      return attachment;
    }

  private:
    typedef stl::hash_map<object_id_t, BasicAttachment*> AttachmentMap;

  private:
    object_id_t   current_owner_id;
    object_id_t   current_object_id;
    AttachmentMap attachments;
};

typedef common::Singleton<InprocessExchangeStorageImpl> InprocessExchangeStorageSingleton;

}

/*
    Обертки над вызовами
*/

object_id_t InprocessExchangeStorage::AllocateOwnerId ()
{
  return InprocessExchangeStorageSingleton::Instance ()->AllocateOwnerId ();
}

void InprocessExchangeStorage::DeallocateOwnerId (object_id_t)
{
}

void InprocessExchangeStorage::Detach (object_id_t id)
{
  InprocessExchangeStorageSingleton::Instance ()->Detach (id);
}

void InprocessExchangeStorage::DetachAllWithOwner (object_id_t owner_id)
{
  InprocessExchangeStorageSingleton::Instance ()->DetachAllWithOwner (owner_id);
}

object_id_t InprocessExchangeStorage::Attach (BasicAttachment* in_attachment)
{
  stl::auto_ptr<BasicAttachment> attachment (in_attachment);

  return InprocessExchangeStorageSingleton::Instance ()->Attach (attachment);
}

BasicAttachment* InprocessExchangeStorage::FindAttachment (object_id_t id, const std::type_info& type)
{
  try
  {
    return InprocessExchangeStorageSingleton::Instance ()->FindAttachment (id, type);
  }
  catch (...)
  {
    return 0;
  }
}

void InprocessExchangeStorage::RaiseAttachmentError (const char* source, object_id_t id, const std::type_info& type)
{
  throw xtl::format_operation_exception (source, "Can't find attachment with id %llu and type %s", id, type.name ());
}
