#include "shared.h"

using namespace media::players::iphone;

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
    }

///Установка режима циклического проигрывания    
    void SetLooping  (bool state)
    {
    }

///Установить громкость
    void SetVolume (float volume)
    {
    }
    
  private:
    
};
