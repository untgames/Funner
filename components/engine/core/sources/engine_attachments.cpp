#include "shared.h"

using namespace engine;

namespace
{

/*
    Ключ к реестре
*/

struct AttachmentKey
{
  size_t                name_hash; //хэш имени
  const std::type_info* type;      //тип точки привязки
  
  AttachmentKey (const char* name, const std::type_info& in_type) :
    name_hash (common::strhash (name)), type (&in_type) {}
    
  bool operator == (const AttachmentKey& key) const { return name_hash == key.name_hash && type == key.type; }
};

size_t hash (const AttachmentKey& key)
{
  return stl::hash (key.name_hash, stl::hash (key.type));
}

}

namespace engine
{

/*
    Реализация менеджера точек привязки
*/

class AttachmentRegistryImpl: public xtl::reference_counter
{
  public:
///Деструктор
    ~AttachmentRegistryImpl ()
    {
      UnregisterAll ();
    }
  
///Регистрация точки привязки
    void Register (const char* name, detail::IBasicAttachment* attachment)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistry::Register";
      
        //проверка корректности аргументов, захват ресурсов
      
      if (!attachment)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment");
      
      AttachmentPtr         attachment_holder (attachment);
      const std::type_info& type = attachment->GetType ();

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        //поиск вхождения в карту точек привязки

      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);
      
      if (iter == attachments.end ())
      {
          //добавление новой точки привязки

        attachments.insert_pair (key, AttachmentEntryPtr (new AttachmentEntry (name, type, attachment_holder), false));
      }
      else
      {
        iter->second->attachment = attachment_holder;
      }

        //оповещение об установке новой точки привязки

      RegisterAttachmentNotify (name, type, attachment);      
    }
    
///Отмена регистрации точки привязки
    void Unregister (const char* name, const std::type_info& type)
    {
      if (!name)
        return;

        //поиск точки привязки

      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);

      if (iter == attachments.end ())
        return;

        //оповещение об удалении точки привязки

      AttachmentEntry& entry = *iter->second;

      UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());

        //удаление точки привязки

      attachments.erase (iter);
    }
    
///Отмена регистрации всех точек привязки определённого типа
    void UnregisterAll (const std::type_info& type)
    {
      for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end ();)
        if (iter->second->type == &type)
        {
          AttachmentMap::iterator next = iter;

          ++next;

            //оповещение об удалении точки привязки

          AttachmentEntry& entry = *iter->second;

          UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());

            //удаление точки привязки

          attachments.erase (iter);

          iter = next;
        }
        else ++iter;
    }
    
///Отмена регистрации всех точек привязки
    void UnregisterAll ()
    {
      for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end ();)
      {
          //оповещение об удалении точки привязки

        AttachmentEntry& entry = *iter->second;

        UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());
      }

      attachments.clear ();
    }

///Поиск точки привязки
    detail::IBasicAttachment* Find (const char* name, const std::type_info& type, bool raise_exception)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistryImpl::Find";

        //проверка корректности аргументов

      if (!name)
      {
        if (raise_exception)
          throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        return 0;
      }
      
        //поиск точки привязки по имени
      
      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);

      if (iter == attachments.end ())
      {
        if (raise_exception)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment point not found");

        return 0;
      }

      return iter->second->attachment.get ();
    }

///Добавление слушателя событий
    void Attach (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistryImpl::Attach";
      
        //проверка корректности аргументов

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      switch (mode)
      {
        case AttachmentRegistryAttachMode_Quiet:
        case AttachmentRegistryAttachMode_ForceNotify:
          break;
        default:
          throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);
      }

        //добавление слушателя

      listeners.push_back (ListenerEntry (type, listener));      
      
        //принудительное оповещение о регистрации точек привязки
        
      if (mode == AttachmentRegistryAttachMode_ForceNotify)
      {        
        for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end (); ++iter)
        {
          AttachmentEntry& entry = *iter->second;
          
          if (entry.type == &type)
          {
            try
            {
              listener->OnRegisterAttachmentCore (entry.name.c_str (), entry.attachment.get ());
            }
            catch (...)
            {
              //подавление всех исключений
            }
          }
        }
      }
    }
    
///Удаление слушателя событий
    void Detach (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode)
    {
        //проверка корректности аргументов
      
      if (!listener)
        return;
        
      switch (mode)
      {
        case AttachmentRegistryAttachMode_Quiet:
        case AttachmentRegistryAttachMode_ForceNotify:
          break;
        default:
          return;
      }        
        
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end ();)
        if (iter->type == &type && iter->listener == listener)
        {
          ListenerList::iterator next = iter;

          ++next;
          
          IBasicAttachmentRegistryListener* listener = iter->listener;
          
            //принудительное оповещение об отмене регистрации точек привязки

          if (mode == AttachmentRegistryAttachMode_ForceNotify)
          {        
            for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end (); ++iter)
            {
              AttachmentEntry& entry = *iter->second;

              if (entry.type == &type)
              {
                try
                {
                  listener->OnRegisterAttachmentCore (entry.name.c_str (), entry.attachment.get ());
                }
                catch (...)
                {
                  //подавление всех исключений
                }
              }
            }
          }          

            //удаление слушателя

          listeners.erase (iter);

          iter = next;
        }
        else ++iter;
    }
    
  private:
///Оповещение о регистрации точки привязки
    void RegisterAttachmentNotify (const char* name, const std::type_info& type, detail::IBasicAttachment* attachment)
    {
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end (); ++iter)
        if (iter->type == &type)
        {
          try
          {
            iter->listener->OnRegisterAttachmentCore (name, attachment);
          }
          catch (...)
          {
            //подавление всех исключений
          }
        }
    }

///Оповещение об удалении точки привязки
    void UnregisterAttachmentNotify (const char* name, const std::type_info& type, detail::IBasicAttachment* attachment)
    {
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end (); ++iter)
        if (iter->type == &type)
        {
          try
          {
            iter->listener->OnUnregisterAttachmentCore (name, attachment);
          }
          catch (...)
          {
            //подавление всех исключений
          }
        }
    }

  private:
    typedef stl::auto_ptr<detail::IBasicAttachment> AttachmentPtr;

    struct AttachmentEntry: public xtl::reference_counter
    {
      stl::string           name;
      AttachmentPtr         attachment;
      const std::type_info* type;

      AttachmentEntry (const char* in_name, const std::type_info& in_type, AttachmentPtr& in_attachment) :
        name (in_name), attachment (in_attachment), type (&in_type) {}
    };  

    struct ListenerEntry
    {
      IBasicAttachmentRegistryListener* listener;
      const std::type_info*             type;

      ListenerEntry (const std::type_info& in_type, IBasicAttachmentRegistryListener* in_listener) :
        listener (in_listener), type (&in_type) {}
    };

    typedef xtl::intrusive_ptr<AttachmentEntry>              AttachmentEntryPtr;
    typedef stl::hash_map<AttachmentKey, AttachmentEntryPtr> AttachmentMap;
    typedef stl::list<ListenerEntry>                         ListenerList;

  private:
    AttachmentMap attachments; //карта точек привязки
    ListenerList  listeners;   //слушатели
};

typedef common::Singleton<AttachmentRegistryImpl> AttachmentRegistrySingleton;

}

/*
    Врапперы над вызовами к AttachmentRegistry
*/

void AttachmentRegistry::RegisterCore (const char* name, detail::IBasicAttachment* attachment)
{
  AttachmentRegistrySingleton::Instance ().Register (name, attachment);
}

void AttachmentRegistry::UnregisterCore (const char* name, const std::type_info& type)
{
  AttachmentRegistrySingleton::Instance ().Unregister (name, type);
}

void AttachmentRegistry::UnregisterAllCore (const std::type_info& type)
{
  AttachmentRegistrySingleton::Instance ().UnregisterAll (type);
}

detail::IBasicAttachment* AttachmentRegistry::FindCore (const char* name, const std::type_info& type, bool raise_exception)
{
  return AttachmentRegistrySingleton::Instance ().Find (name, type, raise_exception);
}

void AttachmentRegistry::AttachCore
 (const std::type_info&             type,
  IBasicAttachmentRegistryListener* listener,
  AttachmentRegistryAttachMode      mode)
{
  AttachmentRegistrySingleton::Instance ().Attach (type, listener, mode);
}

void AttachmentRegistry::DetachCore
 (const std::type_info&             type,
  IBasicAttachmentRegistryListener* listener,
  AttachmentRegistryAttachMode      mode)
{
  AttachmentRegistrySingleton::Instance ().Detach (type, listener, mode);
}
