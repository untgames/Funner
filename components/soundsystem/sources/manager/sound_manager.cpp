#include <sound/manager.h>
#include <sound/device.h>
#include <stl/hash_set>
#include <xtl/intrusive_ptr.h>

using namespace sound;

/*
    Описание реализации SoundManager
*/

typedef stl::hash_set<Emitter*>               EmitterSet;
typedef xtl::com_ptr<low_level::ISoundDevice> DevicePtr;

struct SoundManager::Impl
{
  syslib::Window&             window;          //окно
  DevicePtr                   device;          //устройство воспроизведения
  sound::WindowMinimizeAction minimize_action; //поведение при сворачивании окна
  float                       volume;          //добавочная громкость
  bool                        is_muted;        //флаг блокировки проигрывания звука
  sound::Listener             listener;        //параметры слушателя
  EmitterSet                  emitters;        //излучатели звука
};
