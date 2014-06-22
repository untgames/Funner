/*
    Базовый тип для присоединенных данных
*/

namespace detail
{

struct BasicAttachment
{
  object_id_t           owner_id;
  const std::type_info* type;

  BasicAttachment (object_id_t in_owner_id, const std::type_info& in_type) : owner_id (in_owner_id), type (&in_type) {}

  virtual ~BasicAttachment () {}
};

template <class T> struct Attachment: public BasicAttachment
{
  T object;

  Attachment (object_id_t owner_id, const T& in_object) : BasicAttachment (owner_id, typeid (T)), object (in_object) {}
};

}

/*
    Регистрация и поиск объекта
*/

template <class T>
inline object_id_t InprocessExchangeStorage::Attach (object_id_t owner_id, const T& object)
{
  return Attach (new detail::Attachment<T> (owner_id, object));
}

template <class T>
inline T* InprocessExchangeStorage::FindAttachment (object_id_t id)
{
  detail::BasicAttachment* attachment = FindAttachment (id, typeid (T));

  if (!attachment)
    return 0;

  return &static_cast<detail::Attachment<T>*> (attachment)->object;
}

template <class T>
inline T& InprocessExchangeStorage::Attachment (object_id_t id)
{
  T* result = FindAttachment<T> (id);

  if (result)
    return *result;

  RaiseAttachmentError ("render::scene::interchange::InprocessExchangeStorage::Attachment", id, typeid (T));

  return *result; //will be never called
}
