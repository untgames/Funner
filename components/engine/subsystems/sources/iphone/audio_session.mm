#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <engine/subsystem_manager.h>

#import <AudioToolbox/AudioServices.h>
#import <MediaPlayer/MPMoviePlayerController.h>

using namespace engine;

//—лушатель событи€ окончани€ проигрывани€ видео
@interface VideoPlayerFinishPlaybackListener : NSObject
{
}

@end

namespace
{

/*
     онстанты
*/

const char* SUBSYSTEM_NAME = "IPhoneAudioSession";                   //им€ подсистемы
const char* COMPONENT_NAME = "engine.subsystems.IPhoneAudioSession"; //им€ компонента
const char* LOG_NAME       = COMPONENT_NAME;

const common::ActionQueue::time_t REACTIVATE_DELAY = 0.25;

/*
   ѕодсистема
*/

class IPhoneAudioSessionSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
    /// онструктор/деструктор
    IPhoneAudioSessionSubsystem (common::ParseNode& node, SubsystemManager& in_subsystem_manager);
    ~IPhoneAudioSessionSubsystem ();

    ///ќстановка/запуск необходимых подсистем
    void StopAffectedSubsystems ();
    void StartAffectedSubsystems ();

    ///ѕодсчЄт ссылок
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


void audio_session_interruption_listener (void*, UInt32 interruption_state);

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
    case kAudioSessionNotActiveError:           return "The operation failed because your application's audio session was not active.";
    case kAudioServicesNoHardwareError:         return "The audio operation failed because the device has no audio input available.";
    case kAudioSessionNoCategorySet:            return "The audio operation failed because it requires the audio session to have an explicitly-set category, but none was set. To use a hardware codec you must explicitly initialize the audio session and explicitly set an audio session category.";
    case kAudioSessionIncompatibleCategory:     return "The specified audio session category cannot be used for the attempted audio operation. For example, you attempted to play or record audio with the audio session category set to kAudioSessionCategory_AudioProcessing.";
    case kAudioSessionUnspecifiedError:         return "An unspecified audio session error has occurred. This typically results from the audio system being in an inconsistent state.";
    default:                                    return "Unknown error";
  }
}

//проверка ошибок Audio Services
void check_audio_session_error (OSStatus error_code, const char* source)
{
  if (error_code == kAudioSessionNoError)
    return;

  throw xtl::format_operation_exception (source, "Audio session error. %s. Code %d.", get_audio_session_error_name (error_code), error_code);
}

// ласс инициализирующий аудиосессию и хран€щий указатель на подсистему
class AudioSessionManager
{
  public:
    /// онструктор
    AudioSessionManager ()
      : log (LOG_NAME)
      , active_subsystem (0)
    {
      static const char* METHOD_NAME = "engine::AudioSessionManager::AudioSessionManager";

      check_audio_session_error (AudioSessionInitialize (0, 0, audio_session_interruption_listener, 0), METHOD_NAME);
      check_audio_session_error (AudioSessionSetActive (true), METHOD_NAME);

      video_player_listener = [[VideoPlayerFinishPlaybackListener alloc] init];
    }

    ~AudioSessionManager ()
    {
      [video_player_listener release];
    }

    ///Change audio category
    void SetAudioCategory (const char* category_name)
    {
      static const char* METHOD_NAME = "engine::AudioSessionManager::SetAudioCategory";

      if (!category_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "category_name");

      int category;

      if (!xtl::xstrcmp (category_name, "AmbientSound"))
        category = kAudioSessionCategory_AmbientSound;
      else if (!xtl::xstrcmp (category_name, "SoloAmbientSound"))
        category = kAudioSessionCategory_SoloAmbientSound;
      else if (!xtl::xstrcmp (category_name, "MediaPlayback"))
        category = kAudioSessionCategory_MediaPlayback;
      else if (!xtl::xstrcmp (category_name, "RecordAudio"))
        category = kAudioSessionCategory_RecordAudio;
      else if (!xtl::xstrcmp (category_name, "PlayAndRecord"))
        category = kAudioSessionCategory_PlayAndRecord;
      else if (!xtl::xstrcmp (category_name, "AudioProcessing"))
        category = kAudioSessionCategory_AudioProcessing;
      else
        throw xtl::format_operation_exception (METHOD_NAME, "Unknown audio category '%s'", category_name);

      check_audio_session_error (AudioSessionSetProperty (kAudioSessionProperty_AudioCategory,
                                 sizeof (category), &category), METHOD_NAME);
    }

    ///”становка/получение подсистемы
    IPhoneAudioSessionSubsystem* ActiveSubsystem ()
    {
      return active_subsystem;
    }

    void SetActiveSubsystem (IPhoneAudioSessionSubsystem* new_active_subsystem)
    {
      active_subsystem = new_active_subsystem;
    }

    ///ѕытатьс€ реактивировать аудио-сессию несколько раз, поскольку с первого раза может не получитьс€. јудио-сесси€ может быть еще заблокирована другим приложением, несмотр€ на событие окончани€ прерывани€
    void ReactivateAudioSessionWithDelay ()
    {
      log.Printf ("Init reactivation");
      reactivate_action = common::ActionQueue::PushAction (xtl::bind (&AudioSessionManager::ProcessAudioSessionInterruption, this, (size_t)kAudioSessionEndInterruption), common::ActionThread_Current, REACTIVATE_DELAY);
    }

    void ProcessAudioSessionInterruption (size_t interruption_state)
    {
      if (interruption_state == kAudioSessionBeginInterruption)
      {
        log.Printf ("Interruption begin");

        reactivate_action.Cancel ();

        if (active_subsystem)
          active_subsystem->StopAffectedSubsystems ();

        check_audio_session_error (AudioSessionSetActive (false), "engine::audio_session_interruption_listener");
      }
      else if (interruption_state == kAudioSessionEndInterruption)
      {
        log.Printf ("Interruption end");

        try
        {
          check_audio_session_error (AudioSessionSetActive (true), "engine::audio_session_interruption_listener");
        }
        catch (xtl::exception& e)
        {
          log.Printf ("Exception at processing audio session state change: '%s'\n", e.what ());
          ReactivateAudioSessionWithDelay ();
          return;
        }
        catch (...)
        {
          log.Printf ("Unknown exception at processing audio session state change\n");
          ReactivateAudioSessionWithDelay ();
          return;
        }

        if (active_subsystem)
          active_subsystem->StartAffectedSubsystems ();
      }
    }

  private:
    common::Log                        log;
    IPhoneAudioSessionSubsystem*       active_subsystem;
    VideoPlayerFinishPlaybackListener* video_player_listener;
    common::Action                     reactivate_action;
};

typedef common::Singleton <AudioSessionManager> AudioSessionManagerSingleton;

///  онструктор/деструктор
IPhoneAudioSessionSubsystem::IPhoneAudioSessionSubsystem (common::ParseNode& node, SubsystemManager& in_subsystem_manager)
  : subsystem_manager (in_subsystem_manager)
{
  try
  {
    AudioSessionManagerSingleton::Instance ()->SetAudioCategory (common::get<const char*> (node, "AudioCategory", "SoloAmbientSound"));

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

IPhoneAudioSessionSubsystem::~IPhoneAudioSessionSubsystem ()
{
  AudioSessionManagerSingleton::Instance ()->SetActiveSubsystem (0);
}

//ќстановка/запуск необходимых подсистем
void IPhoneAudioSessionSubsystem::StopAffectedSubsystems ()
{
  for (AffectedSubsystemsDescsArray::iterator iter = affected_subsystems.begin (), end = affected_subsystems.end (); iter != end; ++iter)
    subsystem_manager.Remove ((*iter)->name_wildcard.c_str ());
}

void IPhoneAudioSessionSubsystem::StartAffectedSubsystems ()
{
  StopAffectedSubsystems ();  //дл€ обработки ситуации, когда сообщение о восстановлении аудиосессии приходит два раза (воспроизведение видео)

  for (AffectedSubsystemsDescsArray::reverse_iterator iter = affected_subsystems.rbegin (), end = affected_subsystems.rend (); iter != end; ++iter)
    subsystem_manager.Start ((*iter)->config_file.c_str (), (*iter)->name_wildcard.c_str ());
}

//обработчик событий прерывани€/восстановлени€ аудиосессии
void audio_session_interruption_listener (void*, UInt32 interruption_state)
{
  try
  {
    AudioSessionManagerSingleton::Instance ()->ProcessAudioSessionInterruption (interruption_state);
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
    омпонент регистрации слушател€ прерываний аудио
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
