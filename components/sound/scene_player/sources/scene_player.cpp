#include <time.h>

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>

#include <common/action_queue.h>

#include <sg/scene.h>
#include <sg/node.h>
#include <sg/entity.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>

#include <sound/scene_player.h>

#include <math/mathlib.h>

#include <syslib/timer.h>

using namespace sound;
using namespace syslib;
using namespace xtl;
using namespace common;
using namespace scene_graph;
using namespace math;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Описание реализации ScenePlayer
*/

struct ScenePlayerEmitter
{
  sound::Emitter  emitter;           //эмиттер
  auto_connection play_connection;   //соединение события запуска проигрывания
  auto_connection stop_connection;   //соединение события остановки проигрывания
  auto_connection update_connection; //соединение события обновления свойств
  clock_t         play_start_time;   //время начала проигрывания
  float           play_start_offset; //смещение начала проигрывания
  bool            is_playing;        //проигрывается ли звук

  ScenePlayerEmitter (const char* source_name, xtl::connection in_play_connection, xtl::connection in_stop_connection, xtl::connection in_update_connection);
};

ScenePlayerEmitter::ScenePlayerEmitter (const char* source_name, xtl::connection in_play_connection, xtl::connection in_stop_connection, xtl::connection in_update_connection)
  : play_connection (in_play_connection),
    stop_connection (in_stop_connection),
    update_connection (in_update_connection),
    play_start_offset (0.f),
    is_playing (false)
  {
    emitter.SetSource (source_name);
  }

typedef xtl::shared_ptr<ScenePlayerEmitter>                              ScenePlayerEmitterPtr;
typedef stl::hash_map<scene_graph::SoundEmitter*, ScenePlayerEmitterPtr> EmitterSet;

namespace
{

const size_t ACTION_QUEUE_PROCESS_MILLISECONDS = 100;

}

struct ScenePlayer::Impl
{
  scene_graph::Listener* listener;                   //слушатель
  sound::SoundManager*   sound_manager;              //менеджер
  EmitterSet             emitters;                   //эмиттеры
  auto_connection        bind_node_connection;       //соединение события появления нового узла в сцене
  auto_connection        unbind_node_connection;     //соединение события удаления узла в сцене
  auto_connection        update_connection;          //соединение события обновления свойств
  auto_connection        listener_unbind_connection; //соединение события удаления слушателя
  auto_connection        manager_destroy_connection; //соединение события удаления менеджера
  ActionQueue            action_queue;               //очередь событий остановки проигрывания эмиттера
  syslib::Timer          action_queue_timer;         //таймер вызова обработки очереди

  Impl ()
    : listener (0),
      sound_manager (0),
      action_queue_timer (xtl::bind (&ScenePlayer::Impl::DoActions, this), ACTION_QUEUE_PROCESS_MILLISECONDS)
    {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка слушателя
///////////////////////////////////////////////////////////////////////////////////////////////////
  void SetListener (scene_graph::Listener* in_listener)
  {
    if (listener == in_listener)
      return;

    if (!in_listener)
    {
      listener = in_listener;

      update_connection.disconnect ();
      listener_unbind_connection.disconnect ();

      if (sound_manager)
        sound_manager->SetMute (true);

      return;
    }

    scene_graph::Scene* scene = in_listener->Scene ();

    if (!scene)
      throw xtl::format_operation_exception ("sound::ScenePlayer::SetListener", "Listener is not binded to any scene.");

    if (!listener || (listener->Scene () != scene))
    {
      if (sound_manager)
        for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
          sound_manager->StopSound (i->second->emitter);

      emitters.clear ();

      scene->Traverse (xtl::bind (&ScenePlayer::Impl::CheckNode, this, _1));

      bind_node_connection   = scene->Root ().RegisterEventHandler (NodeSubTreeEvent_AfterBind,    bind (&ScenePlayer::Impl::ProcessAttachNode, this, _1, _2, _3));
      unbind_node_connection = scene->Root ().RegisterEventHandler (NodeSubTreeEvent_BeforeUnbind, bind (&ScenePlayer::Impl::ProcessDetachNode, this, _1, _2, _3));
    }

    listener = in_listener;

    update_connection          = in_listener->RegisterEventHandler (NodeEvent_AfterUpdate,  bind (&ScenePlayer::Impl::ListenerUpdate, this, _1, _2));
    listener_unbind_connection = in_listener->RegisterEventHandler (NodeEvent_BeforeUnbind, bind (&ScenePlayer::Impl::ListenerUnbind, this, _1, _2));

    if (sound_manager)
    {
      sound_manager->SetMute (false);
      ListenerUpdate (*listener, NodeEvent_AfterUpdate);
    }
  }

  scene_graph::Listener* Listener () const
  {
    return listener;
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка менеджера
///////////////////////////////////////////////////////////////////////////////////////////////////
  void SetManager (sound::SoundManager* in_sound_manager)
  {
    if (sound_manager == in_sound_manager)
      return;

    if (!in_sound_manager)
    {
      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        sound_manager->StopSound (i->second->emitter);

      manager_destroy_connection.disconnect ();

      sound_manager = 0;

      return;
    }

    sound_manager = in_sound_manager;

    manager_destroy_connection = in_sound_manager->RegisterDestroyHandler (bind (&ScenePlayer::Impl::ProcessDestroyManager, this));

    if (listener)
      SetListener (listener);

    for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
      if (i->second->is_playing)
      {
        i->second->play_start_offset += (float)(clock () - i->second->play_start_time) / CLOCKS_PER_SEC;

        i->second->play_start_time = clock ();

        sound_manager->PlaySound (i->second->emitter, i->second->play_start_offset);
      }
  }

  sound::SoundManager* Manager () const
  {
    return sound_manager;
  }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика очереди событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoActions ()
    {
      action_queue.DoActions (action_queue_timer.ElapsedMilliseconds ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка удаления менеджера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessDestroyManager ()
    {
      sound_manager = 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление свойств слушателя/эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ListenerUpdate (Node& sender, NodeEvent event)
    {
      if (!listener || !sound_manager)
        return;

      sound::Listener snd_listener;

      mat4f listener_world_tm = listener->WorldTM ();

      snd_listener.position  = listener_world_tm * vec3f (0.f);                              //!!!!!!!!добавить скорость
      snd_listener.direction = listener_world_tm * vec4f (0.f,0.f,-1.f, 0.f);
      snd_listener.up        = listener_world_tm * vec4f (0.f,1.f,0.f, 0.f);

      sound_manager->SetListener (snd_listener);
      sound_manager->SetVolume   (listener->Gain ());
    }

    void EmitterUpdate (Node& sender, NodeEvent event)
    {
      EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&sender));

      if (emitter_iter == emitters.end ())
        return;

      emitter_iter->second->emitter.SetPosition  (sender.WorldPosition ());               //!!!!!!!!добавить скорость
      emitter_iter->second->emitter.SetDirection (sender.WorldOrientation () * vec3f(0.f,0.f,1.f));
      emitter_iter->second->emitter.SetVolume    (((scene_graph::SoundEmitter*)&sender)->Gain ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление Node сцены в список, если является эмиттером
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckNode (scene_graph::Node& node)
    {
      scene_graph::SoundEmitter* emitter = dynamic_cast<scene_graph::SoundEmitter*> (&node);

      if (emitter)
      {
        ScenePlayerEmitterPtr scene_player_emitter (new ScenePlayerEmitter (emitter->SoundDeclarationName (),
                                                                  emitter->RegisterEventHandler (SoundEmitterEvent_Play, bind (&ScenePlayer::Impl::PlayEmitter, this, _1, _2)),
                                                                  emitter->RegisterEventHandler (SoundEmitterEvent_Stop, bind (&ScenePlayer::Impl::StopEmitter, this, _1)),
                                                                  node.RegisterEventHandler     (NodeEvent_AfterUpdate,  bind (&ScenePlayer::Impl::EmitterUpdate, this, _1, _2))));

        emitters.insert_pair (emitter, scene_player_emitter);
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка добавления/удаления нодов в сцене
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ListenerUnbind (Node& sender, NodeEvent event)
    {
      SetListener (0);
    }

    void ProcessAttachNode (Node& sender, Node& node, NodeSubTreeEvent event)
    {
      CheckNode (node);
    }

    void ProcessDetachNode (Node& sender, Node& node, NodeSubTreeEvent event)
    {
      EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&node));

      if (emitter_iter == emitters.end ())
        return;

      emitters.erase (emitter_iter);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрывание эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PlayEmitter (SoundEmitter& emitter, SoundEmitterEvent event)
    {
      EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&emitter));

      if (emitter_iter == emitters.end ())
        return;

      emitter_iter->second->emitter.SetSampleIndex (rand ());

      emitter_iter->second->is_playing        = true;
      emitter_iter->second->play_start_time   = clock ();
      emitter_iter->second->play_start_offset = 0.f;

      if (!sound_manager)
        return;

      sound_manager->PlaySound (emitter_iter->second->emitter);

      emitter.AddRef ();

      if (!sound_manager->IsLooping (emitter_iter->second->emitter))
        action_queue.SetAction ((size_t)&emitter, size_t(sound_manager->Duration (emitter_iter->second->emitter) * 1000.f + ACTION_QUEUE_PROCESS_MILLISECONDS * 2),
                                bind (&ScenePlayer::Impl::StopEmitter, this, xtl::ref (emitter)));
    }

    void StopEmitter (SoundEmitter& emitter)
    {
      if (!sound_manager)
        return;

      EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&emitter));

      if (emitter_iter == emitters.end ())
        return;

      sound_manager->StopSound (emitter_iter->second->emitter);

      emitter.Release ();

      emitter_iter->second->is_playing = false;
    }
};

/*
   Конструктор / деструктор
*/

ScenePlayer::ScenePlayer ()
  : impl (new Impl)
  {}

ScenePlayer::~ScenePlayer ()
{
}

/*
   Установка/получение менеджера
*/

void ScenePlayer::SetManager (sound::SoundManager* sound_manager)
{
  impl->SetManager (sound_manager);
}

sound::SoundManager* ScenePlayer::Manager () const
{
  return impl->Manager ();
}

/*
   Установка/получение слушателя
*/

void ScenePlayer::SetListener (scene_graph::Listener* listener)
{
  impl->SetListener (listener);
}

scene_graph::Listener* ScenePlayer::Listener () const
{
  return impl->Listener ();
}
