#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>

#include <media/players/stream.h>

using namespace media::players;

namespace
{

const char* TARGET_NAME = "null";

/*
    Пустой проигрыватель
*/

class NullPlayer: public IStreamPlayer
{
  public:
///Конструктор
    NullPlayer (const StreamPlayerManager::StreamEventHandler& in_handler)
      : handler (in_handler)
      {}

///Длительность потока
    float Duration ()
    {
      return 1.f;
    }

///Начать проигрывание
    void Play ()
    {
      OnEvent (StreamEvent_OnPlay);
      OnEvent (StreamEvent_OnAutoStop);
    }

///Приостановить проигрывание
    void Pause ()
    {
    }

///Остановить проигрывание
    void Stop ()
    {
      OnEvent (StreamEvent_OnManualStop);
    }

///Установить позицию проигрывания
    void SetPosition (float position)
    {
    }

///Получить позицию проигрывания
    float Position ()
    {
      return 0.f;
    }

///Установка режима циклического проигрывания
    void SetLooping (bool state)
    {
    }

///Установить громкость
    void SetVolume (float volume)
    {
    }

///Оповещение о событии
    void OnEvent (StreamEvent event)
    {
      handler (this, event);
    }

  private:
    typedef StreamPlayerManager::StreamEventHandler StreamEventHandler;

  private:
    StreamEventHandler handler; //обработчик событий
};


/*
   Компонент проигрывания медиа
*/

class PlayerComponent
{
  public:
    //загрузка компонента
    PlayerComponent ()
    {
      StreamPlayerManager::RegisterPlayer (TARGET_NAME, &PlayerComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "media::players::null:PlayerComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!source_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "source_name");

      if (!xtl::xstrcmp (target_name, TARGET_NAME))
        return new NullPlayer (handler);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

}

extern "C"
{

common::ComponentRegistrator<PlayerComponent> NullPlayer ("media.players.stream.null");

}
