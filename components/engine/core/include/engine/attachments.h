#ifndef ENGINE_ATTACHMENTS_HEADER
#define ENGINE_ATTACHMENTS_HEADER

#include <typeinfo>

namespace xtl
{

//forward declarations
template <class T, template <class > class Strategy> class intrusive_ptr;
template <class T>                                   class com_ptr;
template <class T>                                   class shared_ptr;
template <class T>                                   class reference_wrapper;

}

namespace engine
{

//forward declaration
class AttachmentRegistryImpl;
class IBasicAttachmentRegistryListener;
template <class T> class IAttachmentRegistryListener;

namespace detail
{

class IBasicAttachment;
template <class T> class IAttachment;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим присоединения слушателей реестра точек привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AttachmentRegistryAttachMode
{
  AttachmentRegistryAttachMode_Quiet,       //без приндительного оповещения
  AttachmentRegistryAttachMode_ForceNotify, //с принудительным оповещением о регистрации/отмене регистрации

  AttachmentRegistryAttachMode_Num,

  AttachmentRegistryAttachMode_Default = AttachmentRegistryAttachMode_Quiet
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реестр точек привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
class AttachmentRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация точек привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static void Register      (const char* name, T& object);
    template <class T> static void Unregister    (const char* name, T& object); //only if equal
    template <class T> static void Unregister    (const char* name);
    template <class T> static void UnregisterAll ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск точки привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static T& Get  (const char* name);
    template <class T> static T* Find (const char* name); //nothrow

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со слушателями событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static void Attach (IAttachmentRegistryListener<T>* listener, AttachmentRegistryAttachMode = AttachmentRegistryAttachMode_Default);
    template <class T> static void Detach (IAttachmentRegistryListener<T>* listener, AttachmentRegistryAttachMode = AttachmentRegistryAttachMode_Default);

  private:
    static void                      RegisterCore      (const char* name, detail::IBasicAttachment* attachment);
    static void                      UnregisterCore    (const char* name, const std::type_info& type);
    static void                      UnregisterAllCore (const std::type_info& type);
    static void                      AttachCore        (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode);
    static void                      DetachCore        (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode);
    static detail::IBasicAttachment* FindCore          (const char* name, const std::type_info& type, bool raise_exception);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый интерфейс слушателя реестра точек привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
class IBasicAttachmentRegistryListener
{
  friend class AttachmentRegistryImpl;
  protected:
    virtual ~IBasicAttachmentRegistryListener () {}

  private:
    virtual void OnRegisterAttachmentCore   (const char* name, detail::IBasicAttachment* attachment) = 0;
    virtual void OnUnregisterAttachmentCore (const char* name, detail::IBasicAttachment* attachment) = 0; 
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий реестра точек привязки
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class IAttachmentRegistryListener: public IBasicAttachmentRegistryListener
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///События регистрации / отмены регистрации объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnRegisterAttachment   (const char* name, T& object) {}
    virtual void OnUnregisterAttachment (const char* name, T& object) {}

  private:
    virtual void OnRegisterAttachmentCore   (const char* name, detail::IBasicAttachment* attachment);
    virtual void OnUnregisterAttachmentCore (const char* name, detail::IBasicAttachment* attachment);
};

#include <engine/detail/attachments.inl>

}

#endif
