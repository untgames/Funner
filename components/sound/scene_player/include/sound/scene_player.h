#ifndef SGPLAYER_HEADER
#define SGPLAYER_HEADER

#include <stl/auto_ptr.h>
#include <sg/listener.h>
#include <sound/manager.h>

namespace sound
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрыватель звуков сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScenePlayer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScenePlayer  ();
    ~ScenePlayer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение менеджера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetManager (sound::SoundManager* sound_manager);
    sound::SoundManager* Manager    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение слушателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                   SetListener (scene_graph::Listener* listener);
    scene_graph::Listener* Listener    () const;

  private:
    ScenePlayer (const ScenePlayer&); //no impl
    ScenePlayer& operator = (const ScenePlayer&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
