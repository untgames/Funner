#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <engine/subsystem_manager.h>

#import <AudioServices.h>
#import <MPMoviePlayerController.h>

using namespace engine;

//Слушатель события окончания проигрывания видео
@interface VideoPlayerFinishPlaybackListener : NSObject
{
}

@end

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "IPhoneAudioSession";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.IPhoneAudioSession"; //имя компонента

class IPhoneAudioSessionSubsystem;

void audio_session_interruption_listener (void* iphone_audio_session_subsystem, UInt32 interruption_state);

//получение имени ошибки Audio Services
const char* get_audio_session_error_name (OSStatus error)
{
  switch (error)
  {
    case kAudioSessionNoError:                  return "No error has occurred.";
    case kAudioSessionNotInitialized:           return "An Audio Session Services function was called without first initializing the session.";
    case kAudioSessionAlreadyInitialized:       return "The AudioSessionInitialize function was called more than once during the lifetime of your application.";
    case kAudioSessionInitializationError:      return "There was an error during audio session initialization.";
    case kAudioSessionUnsupportedPropertyError: return "The audio session property is not supported.";
    case kAudioSessionBadPropertySizeError:     return "The size of the audio session property data was not correct.";
    case kAudioSessionNotActiveError:           return "The operation failed because your application’s audio session was not active.";
    default:                                    return "Unknown error";
  }
}

//проверка ошибок Audio Services
void check_audio_session_error (OSStatus error_code, const char* source)
{
  if (error_code == kAudioSessionNoError)
    return;

  throw xtl::format_operation_exception (source, "Audio session error. %s", get_audio_session_error_name (error_code));
}

//Класс инициализирующий аудиосессию и хранящий указатель на подсистему
class AudioSessionManager
{
  public:
    ///Конструктор
    AudioSessionManager ()
      : active_subsystem (0)
    {
      static const char* METHOD_NAME = "engine::AudioSessionManage::AudioSessionManager";

      check_audio_session_error (AudioSessionInitialize (0, 0, audio_session_interruption_listener, 0), METHOD_NAME);

      int audio_session_category = kAudioSessionCategory_SoloAmbientSound;

      check_audio_session_error (AudioSessionSetProperty (kAudioSessionProperty_AudioCategory,
                                 sizeof (audio_session_category), &audio_session_category), METHOD_NAME);

      check_audio_session_error (AudioSessionSetActive (true), METHOD_NAME);

      video_player_listener = [[VideoPlayerFinishPlaybackListener alloc] init];
    }

    ~AudioSessionManager ()
    {
      [video_player_listener release];
    }

    ///Установка/получение подсистемы
    IPhoneAudioSessionSubsystem* ActiveSubsystem ()
    {
      return active_subsystem;
    }

    void SetActiveSubsystem (IPhoneAudioSessionSubsystem* new_active_subsystem)
    {
      active_subsystem = new_active_subsystem;
    }

  private:
    IPhoneAudioSessionSubsystem*       active_subsystem;
    VideoPlayerFinishPlaybackListener* video_player_listener;
};

typedef common::Singleton <AudioSessionManager> AudioSessionManagerSingleton;

/*
   Подсистема рендера сцены
*/

class IPhoneAudioSessionSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    IPhoneAudioSessionSubsystem (common::ParseNode& node, SubsystemManager& in_subsystem_manager)
      : subsystem_manager (in_subsystem_manager)
    {
      try
      {
        for (common::ParseNamesakeIterator iter = node.First ("InterruptionAffectedSubsystems"); iter; ++iter)
        {
          common::StringArray wildcards        = common::split (common::get<const char*> (*iter, "Wildcards"));
          const char*         config_file_name = common::get<const char*> (*iter, "ConfigFile");

          for (size_t i = 0, count = wildcards.Size (); i < count; i++)
            affected_subsystems.push_back (AffectedSubsystemDescPtr (new AffectedSubsystemDesc (wildcards [i], config_file_name)));
        }

        AudioSessionManagerSingleton::Instance ()->SetActiveSubsystem (this);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::IPhoneAudioSessionSubsystem::IPhoneAudioSessionSubsystem");
        throw;
      }
    }

    ~IPhoneAudioSessionSubsystem ()
    {
      AudioSessionManagerSingleton::Instance ()->SetActiveSubsystem (0);
    }

    //Остановка/запуск необходимых подсистем
    void StopAffectedSubsystems ()
    {
      for (AffectedSubsystemsDescsArray::iterator iter = affected_subsystems.begin (), end = affected_subsystems.end (); iter != end; ++iter)
        subsystem_manager.Remove ((*iter)->name_wildcard.c_str ());
    }

    void StartAffectedSubsystems ()
    {
      StopAffectedSubsystems ();  //для обработки ситуации, когда сообщение о восстановлении аудиосессии приходит два раза (воспроизведение видео)

      for (AffectedSubsystemsDescsArray::reverse_iterator iter = affected_subsystems.rbegin (), end = affected_subsystems.rend (); iter != end; ++iter)
        subsystem_manager.Start ((*iter)->config_file.c_str (), (*iter)->name_wildcard.c_str ());
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    IPhoneAudioSessionSubsystem (const IPhoneAudioSessionSubsystem&);             //no impl
    IPhoneAudioSessionSubsystem& operator = (const IPhoneAudioSessionSubsystem&); //no impl

  private:
    struct AffectedSubsystemDesc
    {
      stl::string name_wildcard;
      stl::string config_file;

      AffectedSubsystemDesc (const char* in_name_wildcard, const char* in_config_file)
        : name_wildcard (in_name_wildcard), config_file (in_config_file)
        {}
    };

    typedef xtl::shared_ptr <AffectedSubsystemDesc> AffectedSubsystemDescPtr;
    typedef stl::vector <AffectedSubsystemDescPtr>  AffectedSubsystemsDescsArray;

  private:
    SubsystemManager&            subsystem_manager;
    AffectedSubsystemsDescsArray affected_subsystems;
};

//обработчик событий прерывания/восстановления аудиосессии
void audio_session_interruption_listener (void* iphone_audio_session_subsystem, UInt32 interruption_state)
{
  try
  {
    IPhoneAudioSessionSubsystem* subsystem = AudioSessionManagerSingleton::Instance ()->ActiveSubsystem ();

    if (!subsystem)
      return;

    if (interruption_state == kAudioSessionBeginInterruption)
    {
      subsystem->StopAffectedSubsystems ();
      check_audio_session_error (AudioSessionSetActive (false), "engine::audio_session_interruption_listener");
    }
    else if (interruption_state == kAudioSessionEndInterruption)
    {
      check_audio_session_error (AudioSessionSetActive (true), "engine::audio_session_interruption_listener");
      subsystem->StartAffectedSubsystems ();
    }
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at processing audio session state change: '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Unknown exception at processing audio session state change\n");
  }
}

}

@implementation VideoPlayerFinishPlaybackListener

- (id)init
{
  self = [super init];

  if (!self)
    return nil;

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (handlePlaybackDidFinishNotification)
                                        name:MPMoviePlayerPlaybackDidFinishNotification object:nil];

  return self;
}

- (void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void) handlePlaybackDidFinishNotification
{
  try
  {
    IPhoneAudioSessionSubsystem* subsystem = AudioSessionManagerSingleton::Instance ()->ActiveSubsystem ();

    if (!subsystem)
      return;

    check_audio_session_error (AudioSessionSetActive (true), "engine::VideoPlayerFinishPlaybackListener::handlePlaybackDidFinishNotification");
    subsystem->StartAffectedSubsystems ();
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at processing audio session state change: '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Unknown exception at processing audio session state change\n");
  }
}

@end

namespace
{

/*
   Компонент регистрации слушателя прерываний аудио
*/

class IPhoneAudioSessionComponent
{
  public:
    //загрузка компонента
    IPhoneAudioSessionComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (common::ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<IPhoneAudioSessionSubsystem> subsystem (new IPhoneAudioSessionSubsystem (node, manager), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::IPhoneAudioSessionComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

common::ComponentRegistrator<IPhoneAudioSessionComponent> IPhoneAudioSessionSubsystem (COMPONENT_NAME);

}

}
