#ifndef RENDER_SCENE_INTERCHANGE_INPROCESS_EXCHANGE_STORAGE_HEADER
#define RENDER_SCENE_INTERCHANGE_INPROCESS_EXCHANGE_STORAGE_HEADER

#include <typeinfo>

#include <xtl/functional_fwd>

#include <render/scene/interchange/types.h>

namespace render
{

namespace scene
{

namespace interchange
{

namespace detail
{

struct BasicAttachment;
template <class T> struct Attachment;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Хранилище для обмена объектами внутри одного процесса
///////////////////////////////////////////////////////////////////////////////////////////////////
class InprocessExchangeStorage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение идентификатора владельца
///////////////////////////////////////////////////////////////////////////////////////////////////
    static object_id_t AllocateOwnerId   ();
    static void        DeallocateOwnerId (object_id_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static object_id_t Attach             (object_id_t owner_id, const T& object);
                       static void        Detach             (object_id_t id);
                       static void        DetachAllWithOwner (object_id_t owner_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static T& Attachment     (object_id_t id);
    template <class T> static T* FindAttachment (object_id_t id);
    
  private:
    static object_id_t              Attach (detail::BasicAttachment*);
    static detail::BasicAttachment* FindAttachment (object_id_t id, const std::type_info& type);
    static void                     RaiseAttachmentError (const char*, object_id_t, const std::type_info&);
};

#include <render/scene/interchange/detail/inprocess_exchange_storage.inl>

}

}

}

#endif
