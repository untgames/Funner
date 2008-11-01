#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>

#include <input/low_level/driver.h>

#include <input/translation_map.h>
#include <input/translation_map_registry.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;
using namespace input::low_level;

namespace
{

const char* SUBSYSTEM_NAME = "InputManager";
const char* LOG_NAME       = "engine.subsystems.InputManagerSubsystem";
const char* COMPONENT_NAME = LOG_NAME;

/*
   Подсистема менеджера ввода               
*/

typedef xtl::function<void (const char*)> InputHandler;

class InputManagerSubsystem: public ISubsystem, public IAttachmentRegistryListener<const InputHandler>, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    InputManagerSubsystem (const char* registry_name)
      : translation_map_registry (registry_name),
        log (LOG_NAME)
    {
      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

    ~InputManagerSubsystem ()
    {
      AttachmentRegistry::Detach (this);
    }

///Обработчик события добавления точек привязки событий ввода
    void OnRegisterAttachment (const char* attachment_name, const InputHandler& handler)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("InputManagerSubsystem::OnSetInputHandler", "attachment_name");

      AttachmentPtr new_attachment (new Attachment, false);

      for (size_t i = 0; i < DriverManager::DriversCount (); i++)
      {
        IDriver * volatile current_driver = DriverManager::Driver (i);

        for (size_t j = 0; j < current_driver->GetDevicesCount (); j++)
        {
          try
          {
            DevicePtr current_device (current_driver->CreateDevice (current_driver->GetDeviceName (j)), false);

            stl::string profile (attachment_name);

            profile += '.';
            profile += current_device->GetFullName ();

            const char* translation_map_name = translation_map_registry.FindNearest (profile.c_str ());

            if (!translation_map_name)
              continue;

            Attachment::DeviceEntryPtr device_entry (new Attachment::DeviceEntry, false); 

            TranslationMapsMap::iterator translation_map_iter = translation_maps.find (translation_map_name);

            if (translation_map_iter == translation_maps.end ())
              device_entry->translation_map = TranslationMapHolderPtr (new TranslationMapHolder (translation_map_name, translation_maps), false);
            else
              device_entry->translation_map = translation_map_iter->second;

            DevicesMap::iterator device_iter = devices.find (current_device->GetFullName ());

            if (device_iter == devices.end ())
              device_entry->device = DeviceHolderPtr (new DeviceHolder (current_device.get (), devices), false);
            else
              device_entry->device = device_iter->second;

            device_entry->device_connection = current_device->RegisterEventHandler (xtl::bind (&input::TranslationMap::ProcessEvent,
              &device_entry->translation_map->GetTranslationMap (), _1, handler));

            new_attachment->devices.push_back (device_entry);
          }
          catch (xtl::exception& exception)
          {
            log.Printf ("Can't attach to device '%s' with attachment name '%s': exception '%s'", current_driver->GetDeviceName (j), attachment_name,
              exception.what ());
          }
          catch (...)
          {
            log.Printf ("Can't attach to device '%s' with attachment name '%s': unknown exception", current_driver->GetDeviceName (j), attachment_name);
          }
        }
      }

      new_attachment->handler = handler;

      attachments[attachment_name] = new_attachment;
    }

///Обработчик события удаления точек привязки событий ввода
    void OnRemoveInputHandler (const char* attachment_name, const InputHandler&)
    {
      attachments.erase (attachment_name);
    }

/// Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    InputManagerSubsystem (const InputManagerSubsystem&);             //no impl
    InputManagerSubsystem& operator = (const InputManagerSubsystem&); //no impl

  private:    
    class TranslationMapHolder;
    class DeviceHolder;
  
    typedef xtl::com_ptr<IDevice>                                            DevicePtr;
    typedef stl::hash_map<stl::hash_key<const char*>, DeviceHolder*>         DevicesMap;
    typedef stl::hash_map<stl::hash_key<const char*>, TranslationMapHolder*> TranslationMapsMap;    
    typedef xtl::intrusive_ptr<DeviceHolder>                                 DeviceHolderPtr;
    typedef xtl::intrusive_ptr<TranslationMapHolder>                         TranslationMapHolderPtr;

    class TranslationMapHolder : public xtl::reference_counter
    {
      public:
        TranslationMapHolder (const char* translation_map_name, TranslationMapsMap& translation_maps)
          : translation_map (translation_map_name), container (&translation_maps)
        {
          this_iter = translation_maps.insert_pair (translation_map_name, this).first;
        }
        
        ~TranslationMapHolder ()
        {
          container->erase (this_iter);
        }

        input::TranslationMap& GetTranslationMap ()
        {
          return translation_map;
        }

      private:
        input::TranslationMap        translation_map;
        TranslationMapsMap::iterator this_iter;
        TranslationMapsMap*          container;
    };

    class DeviceHolder : public xtl::reference_counter
    {
      public:
        DeviceHolder (IDevice* in_device, DevicesMap& device_map)
          : device (in_device), container (&device_map)
        {
          this_iter = device_map.insert_pair (device->GetFullName (), this).first;
        }
        
        ~DeviceHolder ()
        {
          container->erase (this_iter);
        }

        IDevice* GetDevice ()
        {
          return device.get ();
        }

      private:
        DevicePtr            device;
        DevicesMap::iterator this_iter;
        DevicesMap*          container;
    };

    struct Attachment : public xtl::reference_counter
    {
      public:
        struct DeviceEntry : public xtl::reference_counter
        {
          DeviceHolderPtr         device;
          TranslationMapHolderPtr translation_map;
          xtl::auto_connection    device_connection;
        };

        typedef xtl::intrusive_ptr<DeviceEntry> DeviceEntryPtr;

      private:
        typedef stl::vector<DeviceEntryPtr>     DeviceArray;

      public:
        DeviceArray  devices;
        InputHandler handler;
    };

    typedef xtl::intrusive_ptr<Attachment>                           AttachmentPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, AttachmentPtr> AttachmentMap;

  private:
    input::TranslationMapRegistry translation_map_registry;
    TranslationMapsMap            translation_maps;
    DevicesMap                    devices;
    AttachmentMap                 attachments;
    common::Log                   log;
};

/*
   Компонент менеджера ввода
*/

class InputManagerComponent
{
  public:
    InputManagerComponent () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }
    
  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {        
        const char* registry_name = get<const char*> (node, "TranslationMapRegistry");

        xtl::com_ptr<ISubsystem> subsystem (new InputManagerSubsystem (registry_name), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::InputManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<InputManagerComponent> InputManager (COMPONENT_NAME);

}

}
