namespace detail
{

/*
    Реализация точки привязки
*/

class IBasicAttachment
{
  public:
    virtual ~IBasicAttachment () {}

///Тип точки привязки
    virtual const std::type_info& GetType () = 0;
};

template <class T> class IAttachment: public IBasicAttachment
{
  public:
///Получение точки привязки
    virtual T& GetValue () = 0;
};

template <class T> class BasicAttachmentImpl: public IAttachment<T>
{
  public:
    const std::type_info& GetType () { return typeid (T); }
};

//pointer by default
template <class T, class Ptr> class PtrAttachment: public BasicAttachmentImpl<T>
{
  public:
    PtrAttachment (const Ptr& in_ptr) : ptr (in_ptr) {}

    T& GetValue () { return *ptr; }

  private:
    Ptr ptr;
};

template <class T> class ValueAttachment: public BasicAttachmentImpl<T>
{
  public:
    ValueAttachment (const T& in_value) : value (in_value) {}

    T& GetValue () { return value; }

  private:
    T value;
};

template <class T> class RefAttachment: public BasicAttachmentImpl<T>
{
  public:
    RefAttachment (const xtl::reference_wrapper<T>& in_ref) : ref (in_ref) {}

    T& GetValue () { return ref.get (); }

  private:
    xtl::reference_wrapper<T> ref;
};

/*
    Создание точек привязки
*/

template <class T>
inline IAttachment<T>* make_attachment (const T& value)
{
  return new ValueAttachment<T> (value);
}

template <class T>
inline IAttachment<T>* make_attachment (const xtl::reference_wrapper<T>& ref)
{
  return new RefAttachment<T> (ref);
}

template <class T>
inline IAttachment<T>* make_attachment (const xtl::shared_ptr<T>& ptr)
{
  return new PtrAttachment<T, xtl::shared_ptr<T> > (ptr);
}

template <class T, template <class> class Strategy>
inline IAttachment<T>* make_attachment (const xtl::intrusive_ptr<T, Strategy>& ptr)
{
  return new PtrAttachment<T, xtl::intrusive_ptr<T, Strategy> > (ptr);
}

template <class T>
inline IAttachment<T>* make_attachment (const xtl::com_ptr<T>& ptr)
{
  return new PtrAttachment<T, xtl::com_ptr<T> > (ptr);
}

}

/*
    Слушатель событий реестра точек привязки
*/

template <class T>
void IAttachmentRegistryListener<T>::OnRegisterAttachmentCore (const char* name, detail::IBasicAttachment* attachment)
{
  OnRegisterAttachment (name, static_cast<detail::IAttachment<T>*> (attachment)->GetValue ());
}

template <class T>
void IAttachmentRegistryListener<T>::OnUnregisterAttachmentCore (const char* name, detail::IBasicAttachment* attachment)
{
  OnUnregisterAttachment (name, static_cast<detail::IAttachment<T>*> (attachment)->GetValue ());
}

/*
    Реестр точек привязки
*/

template <class T>
inline void AttachmentRegistry::Register (const char* name, T& object)
{
  RegisterCore (name, detail::make_attachment (object));
}

template <class T>
inline void AttachmentRegistry::Unregister (const char* name)
{
  UnregisterCore (name, typeid (T));
}

template <class T>
inline void AttachmentRegistry::Unregister (const char* name, T& object)
{
  detail::IAttachment<T>* attachment = static_cast<detail::IAttachment<T>*> (FindCore (name, typeid (T), false));
  
  if (!attachment || &attachment->GetValue () != &object)
    return;

  UnregisterCore (name, attachment->GetType ());
}

template <class T>
inline void AttachmentRegistry::UnregisterAll ()
{
  UnregisterAllCore (typeid (T));
}
    
template <class T>
inline T& AttachmentRegistry::Get (const char* name)
{
  return static_cast<detail::IAttachment<T>*> (FindCore (name, typeid (T), true))->GetValue ();
}

template <class T>
inline T* AttachmentRegistry::Find (const char* name)
{
  detail::IAttachment<T>* attachment = static_cast<detail::IAttachment<T>*> (FindCore (name, typeid (T), false));

  return attachment ? &attachment->GetValue () : 0;
}

template <class T>
inline void AttachmentRegistry::Attach (IAttachmentRegistryListener<T>* listener, AttachmentRegistryAttachMode mode)
{
  AttachCore (typeid (T), listener, mode);
}

template <class T>
inline void AttachmentRegistry::Detach (IAttachmentRegistryListener<T>* listener, AttachmentRegistryAttachMode mode)
{
  DetachCore (typeid (T), listener, mode);
}
