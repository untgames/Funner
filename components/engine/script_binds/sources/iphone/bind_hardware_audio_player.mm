#import <Availability.h>

#ifdef __IPHONE_2_2

#import <NSDate.h>
#import <NSError.h>
#import <NSObject.h>
#import <NSString.h>
#import <NSURL.h>

#import <AVAudioPlayer.h>

#include "shared.h"

using namespace script;

namespace
{

class HardwareAudioPlayer;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Делегат проигрывателя
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface HardwareAudioPlayerDelegate : NSObject <AVAudioPlayerDelegate>
{
  @private
    HardwareAudioPlayer *player;  //проигрыватель
    common::Log         *log;     //протокол событий проигрывания
}

-(id) init;
-(id) initWithPlayer:(HardwareAudioPlayer*)in_player;

@end

namespace
{

const char* LOG_NAME                      = "script.binds.HardwareAudioPlayer";
const char* HARDWARE_AUDIO_PLAYER_LIBRARY = "HardwareAudioPlayer";
const char* COMPONENT_NAME                = "script.binds.HardwareAudioPlayer";
const char* BINDER_NAME                   = "HardwareAudioPlayer";

const double EPSILON = 0.01;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аппаратный проигрыватель звука
///////////////////////////////////////////////////////////////////////////////////////////////////
class HardwareAudioPlayer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    HardwareAudioPlayer ()
      : hardware_player (0), volume (1.f), is_paused (false)
    {
      player_delegate = [[HardwareAudioPlayerDelegate alloc] initWithPlayer: this];
    }

    ~HardwareAudioPlayer ()
    {
      ShutdownPlayer ();

      [player_delegate release];
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрывание звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PreloadSound (const char* file_name)
    {
      static const char* METHOD_NAME = "script::binds::HardwareAudioPlayer::PreloadSound";

      if (!file_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

      ShutdownPlayer ();

      NSString *ns_file_name = [[NSString alloc] initWithCString:file_name encoding:NSASCIIStringEncoding];

      NSURL *file_url = [[NSURL alloc] initFileURLWithPath: ns_file_name];

      [ns_file_name release];

      if (!file_url)
        throw xtl::make_argument_exception (METHOD_NAME, "file_name", file_name, "Can't create NSURL from file_name");

      hardware_player = [[AVAudioPlayer alloc] initWithContentsOfURL: file_url error: nil];

      [file_url release];

      if (!hardware_player)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't open file '%s' for playing", file_name);

      if ([hardware_player prepareToPlay] == NO)
      {
        ShutdownPlayer ();
        throw xtl::format_operation_exception (METHOD_NAME, "Can't preload sound for file '%s'", file_name);
      }

      hardware_player.delegate = player_delegate;

      hardware_player.volume = volume;

      sound_file_name = file_name;
    }

    void Play (bool looping)
    {
      static const char* METHOD_NAME = "script::binds::HardwareAudioPlayer::Play";

      if (!hardware_player)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't play sound, player not initialized");

      hardware_player.numberOfLoops = looping ? -1 : 0;

      if ([hardware_player play] == NO)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't play sound, unknown error");

      is_paused = false;
    }

    void PlaySound (const char* file_name, bool looping)
    {
      PreloadSound (file_name);
      Play (looping);
    }

    void Pause ()
    {
      if (!hardware_player)
        throw xtl::format_operation_exception ("script::binds::HardwareAudioPlayer::Pause", "Can't pause playing, player not initialized");

      [hardware_player pause];

      is_paused = true;
    }

    void Stop ()
    {
      [hardware_player stop];
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление громкостью
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetVolume (float new_volume)
    {
      volume = stl::max (0.f, stl::min (1.f, new_volume));

      if (hardware_player)
        hardware_player.volume = volume;
    }

    float GetVolume ()
    {
      return volume;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о проигрывании
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* SoundFileName ()
    {
      return sound_file_name.c_str ();
    }

    bool IsPaused ()
    {
      return is_paused;
    }

  private:
    void ShutdownPlayer ()
    {
      Stop ();
      [hardware_player release];
      hardware_player = 0;
      sound_file_name.clear ();
    }

  private:
    AVAudioPlayer               *hardware_player;  //аппаратный проигрыватель сжатого звука
    HardwareAudioPlayerDelegate *player_delegate;  //делегат проигрывателя
    float                       volume;            //громкость
    bool                        is_paused;         //находится ли в режиме паузы (необходимо для отработки прерывания проигрывания внешними событиями)
    stl::string                 sound_file_name;   //имя загруженного в данный момент файла
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон аппаратного проигрывателя звука
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<HardwareAudioPlayer> HardwareAudioPlayerSingleton;

}

@implementation HardwareAudioPlayerDelegate

-(id) init
{
  return [self initWithPlayer: 0];
}

-(id) initWithPlayer:(HardwareAudioPlayer*)in_player
{
  self = [super init];

  if (self)
  {
    log = 0;

    try
    {
      if (!in_player)
        throw xtl::make_null_argument_exception ("script::binds::HardwareAudioPlayerDelegate::InitWithPlayer", "in_player");

      player = in_player;

      log = new common::Log (LOG_NAME);
    }
    catch (xtl::exception& e)
    {
      printf ("%s\n", e.what ());

      delete log;
      [self release];
      return nil;
    }
    catch (...)
    {
      delete log;
      [self release];
      return nil;
    }
  }

  return self;
}

-(void) dealloc
{
  delete log;

  [super dealloc];
}

-(void) audioPlayerDecodeErrorDidOccur:(AVAudioPlayer*)av_player error:(NSError*)error
{
  log->Printf ("Error while decoding audio file '%s': '%s'\n", player->SoundFileName (), [[error localizedDescription] cStringUsingEncoding: NSASCIIStringEncoding]);
}

-(void) audioPlayerEndInterruption:(AVAudioPlayer*)av_player
{
  if ((av_player.currentTime > EPSILON) && (av_player.currentTime < (av_player.duration - EPSILON)) && !player->IsPaused ())
    [av_player play];
}

@end

namespace
{

void preload_sound (const char* file_name)
{
  HardwareAudioPlayerSingleton::Instance ()->PreloadSound (file_name);
}

void play_sound (const char* file_name, bool looping)
{
  HardwareAudioPlayerSingleton::Instance ()->PlaySound (file_name, looping);
}

void play (bool looping)
{
  HardwareAudioPlayerSingleton::Instance ()->Play (looping);
}

void pause ()
{
  HardwareAudioPlayerSingleton::Instance ()->Pause ();
}

void stop ()
{
  HardwareAudioPlayerSingleton::Instance ()->Stop ();
}

void set_volume (float volume)
{
  HardwareAudioPlayerSingleton::Instance ()->SetVolume (volume);
}

float get_volume ()
{
  return HardwareAudioPlayerSingleton::Instance ()->GetVolume ();
}

void bind_hardware_audio_player_library (script::Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (HARDWARE_AUDIO_PLAYER_LIBRARY);

    //регистрация операций

  lib.Register ("PreloadSound", make_invoker (&preload_sound));
  lib.Register ("PlaySound",    make_invoker (&play_sound));
  lib.Register ("Play",         make_invoker (&play));
  lib.Register ("Pause",        make_invoker (&pause));
  lib.Register ("Stop",         make_invoker (&stop));
  lib.Register ("get_Volume",   make_invoker (&get_volume));
  lib.Register ("set_Volume",   make_invoker (&set_volume));
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_hardware_audio_player_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> HardwareAudioPlayerScriptBind (COMPONENT_NAME);

}

}

#endif
