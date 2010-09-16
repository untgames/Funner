#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/string.h>
#include <common/strlib.h>

#include <syslib/window.h>
#include <syslib/platform/iphone.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;
using namespace syslib::iphone;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "IPhoneConfiguration";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.IPhoneConfiguration"; //имя компонента

/*
    Подсистема настроек iOS
*/

class IPhoneConfigurationSubsystem : public ISubsystem, public IAttachmentRegistryListener<syslib::Window>, public xtl::reference_counter
{
  public:
///Конструктор
    IPhoneConfigurationSubsystem (ParseNode& node)
    {
      for (Parser::NamesakeIterator iter=node.First ("Window"); iter; ++iter)
      {
        const char  *attachment_name = common::get<const char*> (*iter, "Id");
        StringArray orientations     = common::split (common::get<const char*> (*iter, "AllowedOrientations"));

        int allowed_orientations = 0;

        for (size_t i = 0, count = orientations.Size (); i < count; i++)
        {
          const char* current_orientation = orientations [i];

          if (!xtl::xstrcmp (current_orientation, "Portrait"))
            allowed_orientations |= InterfaceOrientation_Portrait;
          else if (!xtl::xstrcmp (current_orientation, "PortraitUpsideDown"))
            allowed_orientations |= InterfaceOrientation_PortraitUpsideDown;
          else if (!xtl::xstrcmp (current_orientation, "LandscapeLeft"))
            allowed_orientations |= InterfaceOrientation_LandscapeLeft;
          else if (!xtl::xstrcmp (current_orientation, "LandscapeRight"))
            allowed_orientations |= InterfaceOrientation_LandscapeRight;
          else
            throw xtl::format_operation_exception ("engine::IPhoneConfigurationSubsystem::IPhoneConfigurationSubsystem", "Unknown orientation '%s' at line %u",
                                                   current_orientation, iter->LineNumber ());
        }

        windows_orientations.insert_pair (attachment_name, allowed_orientations);
      }

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///Деструктор
    ~IPhoneConfigurationSubsystem ()
    {
      AttachmentRegistry::Detach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///Обработчик события регистрации окна
    void OnRegisterAttachment (const char* name, syslib::Window& window)
    {
      WindowsOrientations::iterator iter = windows_orientations.find (name);

      if (iter == windows_orientations.end ())
        return;

      WindowManager::SetAllowedOrientations (window, iter->second);
    }

///Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    IPhoneConfigurationSubsystem (const IPhoneConfigurationSubsystem&);             //no impl
    IPhoneConfigurationSubsystem& operator = (const IPhoneConfigurationSubsystem&); //no impl

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, int> WindowsOrientations;

  private:
    WindowsOrientations windows_orientations;
};

/*
    Компонент настроек iOS
*/

class IPhoneConfigurationComponent
{
  public:
    IPhoneConfigurationComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ISubsystem> subsystem (new IPhoneConfigurationSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::IPhoneConfigurationComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<IPhoneConfigurationComponent> IPhoneConfigurationSubsystem (COMPONENT_NAME);

}

}
