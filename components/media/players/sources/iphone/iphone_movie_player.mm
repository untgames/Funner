#include "shared.h"

using namespace media::players;

/*
    Проигрыватель видео на iPhone
*/

class MoviePlayer: public IStreamPlayer
{
  public:
///Конструктор
  
  
///Деструктор
    ~MoviePlayer ()
    {
    }

///Длительность потока
    float Duration ()
    {
      throw xtl::make_not_implemented_exception("media::players::iphone::MoviePlayer::Duration");
    }

///Начать проигрывание
    void  Play ()
    {
    }
    
///Приостановить проигрывание
    void Pause ()
    {
    }
    
///Остановить проигрывание
    void  Stop        ()
    {
    }
    
///Установить позицию проигрывания
    void  SetPosition (float position)
    {
    }

///Получить позицию проигрывания
    float Position ()
    {
      throw xtl::make_not_implemented_exception("media::players::iphone::MoviePlayer::Position");
    }

///Установка режима циклического проигрывания
    void SetLooping  (bool state)
    {
    }

///Установить громкость
    void SetVolume (float volume)
    {
    }
};

namespace media
{

namespace players
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание проигрывателя
///////////////////////////////////////////////////////////////////////////////////////////////////
IStreamPlayer* create_movie_player (const char* stream_name, const StreamPlayerManager::StreamEventHandler& handler)
{
  throw xtl::make_not_implemented_exception("media::players::iphone::create_movie_player");
}

}

}

}
