#include "shared.h"

#import <NSDate.h>
#import <NSError.h>
#import <NSString.h>
#import <NSURL.h>

#import <AVAudioPlayer.h>

using namespace media::players;

namespace
{

class MusicPlayer;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Делегат проигрывателя
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface AudioPlayerDelegate : NSObject <AVAudioPlayerDelegate>
{
  @private
    MusicPlayer *player;  //проигрыватель
    common::Log *log;     //протокол событий проигрывания
}

-(id) init;
-(id) initWithPlayer:(MusicPlayer*)in_player;

@end

namespace
{

const char*  LOG_NAME = "media.players.iphone.MusicPlayer";
const double EPSILON  = 0.01;

/*
    Проигрыватель видео на iPhone
*/

class MusicPlayer: public IStreamPlayer
{
  public:
///Конструктор
    MusicPlayer (const char* in_stream_name, const StreamPlayerManager::StreamEventHandler& in_handler)
      : handler (in_handler), stream_name (in_stream_name), player (0), player_delegate (0), is_paused (false)
    {
      static const char* METHOD_NAME = "media::players::iphone::MusicPlayer::MusicPlayer";

      NSString *ns_file_name = [[NSString alloc] initWithCString:in_stream_name encoding:NSASCIIStringEncoding];

      NSURL *file_url = [[NSURL alloc] initFileURLWithPath: ns_file_name];

      [ns_file_name release];

      if (!file_url)
        throw xtl::make_argument_exception (METHOD_NAME, "stream_name", in_stream_name, "Can't create NSURL from stream_name");

      player = [[AVAudioPlayer alloc] initWithContentsOfURL: file_url error: nil];

      [file_url release];

      if (!player)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't open file '%s' for playing", in_stream_name);

      if ([player prepareToPlay] == NO)
      {
        [player release];
        throw xtl::format_operation_exception (METHOD_NAME, "Can't preload sound for file '%s'", in_stream_name);
      }

      player_delegate = [[AudioPlayerDelegate alloc] initWithPlayer: this];

      player.delegate = player_delegate;
    }
  
///Деструктор
    ~MusicPlayer ()
    {
      [player release];
      [player_delegate release];
    }

///Длительность потока
    float Duration ()
    {
      return player.duration;
    }

///Начать проигрывание
    void Play ()
    {
      static const char* METHOD_NAME = "media::players::iphone::MusicPlayer::Play";

      if (player.playing)
        throw xtl::format_operation_exception (METHOD_NAME, "Sound stream '%s', is already playing", stream_name.c_str ());

      if (![player play])
        throw xtl::format_operation_exception (METHOD_NAME, "Can't play sound stream '%s', unknown error", stream_name.c_str ());

      OnEvent (StreamEvent_OnPlay);

      is_paused = false;
    }
    
///Приостановить проигрывание
    void PauseWithoutNotify ()
    {
      [player pause];
    }

    void Pause ()
    {
      if (!player.playing)
        return;

      PauseWithoutNotify ();

      is_paused = true;

      OnEvent (StreamEvent_OnPause);
    }
    
///Остановить проигрывание
    void Stop ()
    {
      [player stop];
      SetPosition (0.f);

      OnEvent (StreamEvent_OnManualStop);

      is_paused = false;
    }
    
///Установить позицию проигрывания
    void SetPosition (float position)
    {
      player.currentTime = position;
    }

///Получить позицию проигрывания
    float Position ()
    {
      return player.currentTime;
    }

///Установка режима циклического проигрывания
    void SetLooping (bool state)
    {
      player.numberOfLoops = state ? -1 : 0;
    }

///Установить громкость
    void SetVolume (float volume)
    {
      volume = stl::max (0.f, stl::min (1.f, volume));

      player.volume = volume;
    }
    
///Получение имени потока
    const char* StreamName ()
    {
      return stream_name.c_str ();
    }

///Получение состояния проигрывателя
    bool IsPaused ()
    {
      return is_paused;
    }

///Оповещение о событии
    void OnEvent (StreamEvent event)
    {
      handler (this, event);
    }

  private:
    typedef StreamPlayerManager::StreamEventHandler StreamEventHandler;

  private:
    StreamEventHandler  handler;           //обработчик событий
    stl::string         stream_name;       //имя проигрываемого потока
    AVAudioPlayer       *player;           //проигрыватель звука
    AudioPlayerDelegate *player_delegate;  //делегат проигрывателя
    bool                is_paused;         //находится ли в режиме паузы (необходимо для отработки прерывания проигрывания внешними событиями)
};

}

@implementation AudioPlayerDelegate

-(id) init
{
  return [self initWithPlayer: 0];
}

-(id) initWithPlayer:(MusicPlayer*)in_player
{
  self = [super init];

  if (!self)
    return nil;

  log = 0;

  try
  {
    if (!in_player)
      throw xtl::make_null_argument_exception ("media::players::iphone::AudioPlayerDelegate::InitWithPlayer", "in_player");

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

  return self;
}

-(void) dealloc
{
  delete log;

  [super dealloc];
}

-(void) audioPlayerDidFinishPlaying:(AVAudioPlayer*)av_player successfully:(BOOL)flag
{
  if (!flag)
    log->Printf ("Error while decoding audio file '%s': undecodable data\n", player->StreamName ());

  player->OnEvent (StreamEvent_OnAutoStop);
}

-(void) audioPlayerDecodeErrorDidOccur:(AVAudioPlayer*)av_player error:(NSError*)error
{
  log->Printf ("Error while decoding audio file '%s': '%s'\n", player->StreamName (), [[error localizedDescription] cStringUsingEncoding: NSASCIIStringEncoding]);
}

-(bool) isInTheMiddleOfPlayback:(AVAudioPlayer*)av_player
{
  return (av_player.currentTime > EPSILON) && (av_player.currentTime < (av_player.duration - EPSILON)) && !player->IsPaused ();
}

-(void) audioPlayerBeginInterruption:(AVAudioPlayer*)av_player
{
  if ([self isInTheMiddleOfPlayback:av_player])
    player->OnEvent (StreamEvent_OnPause);
}

-(void) audioPlayerEndInterruption:(AVAudioPlayer*)av_player
{
  if ([self isInTheMiddleOfPlayback:av_player])
    player->Play ();
}

@end

namespace media
{

namespace players
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание проигрывателя
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_music_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler* handler)
{
  if (!stream_name)
    throw xtl::make_null_argument_exception ("media::players::iphone::create_music_player", "stream_name");

  return new MusicPlayer (stream_name, *handler);
}

}

}

}
