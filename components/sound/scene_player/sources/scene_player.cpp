#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/action_queue.h>
#include <common/time.h>

#include <sg/scene.h>
#include <sg/node.h>
#include <sg/entity.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>

#include <sound/scene_player.h>

#include <math/vector.h>

using namespace sound;
using namespace common;
using namespace scene_graph;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const double STOP_EMITTER_DELAY = 0.2; //запаздывание при остановке звука

}

/*
    Эмиттер сцены
*/

class ScenePlayerEmitter: public xtl::trackable
{
  public:
///Конструктор
    ScenePlayerEmitter (SoundEmitter& in_scene_emitter)
      : sound_manager (0)
      , scene_emitter (in_scene_emitter)
      , play_start_time (0)
      , play_start_offset (0.0f)
      , is_playing (false)
      , need_release_scene_emitter (false)
    {
      emitter.SetSource (scene_emitter.SoundDeclarationName ());

      connect_tracker (scene_emitter.RegisterEventHandler (SoundEmitterEvent_OnPlay,   xtl::bind (&ScenePlayerEmitter::OnPlay, this)));
      connect_tracker (scene_emitter.RegisterEventHandler (SoundEmitterEvent_OnStop,   xtl::bind (&ScenePlayerEmitter::OnStop, this)));
      connect_tracker (scene_emitter.RegisterEventHandler (NodeEvent_AfterUpdate,      xtl::bind (&ScenePlayerEmitter::OnUpdate, this)));
      
      on_change_scene = scene_emitter.RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&ScenePlayerEmitter::OnSceneChange, this));
    }
    
///Деструктор
    ~ScenePlayerEmitter ()
    {
      try
      {
        on_change_scene.disconnect ();
        
        OnStop ();
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }  
    
///Изменение менеджера звуков
    void SetSoundManager (SoundManager* in_sound_manager)
    {
      if (!in_sound_manager)
      {
        if (sound_manager)
          sound_manager->StopSound (emitter);

        sound_manager = 0;

        return;
      }

      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
      
      if (!is_playing)
        return;
        
      play_start_offset += (common::milliseconds () - play_start_time) / 1000.0f;
      play_start_time    = common::milliseconds ();

      sound_manager->PlaySound (emitter, play_start_offset);
    }
    
    SoundManager* GetSoundManager () { return sound_manager; }
    
  private:
///Обработчик события обновления эмиттера сцены
    void OnUpdate ()
    {
      emitter.SetPosition  (scene_emitter.WorldPosition ());
      emitter.SetDirection (scene_emitter.WorldOrtZ ());
      emitter.SetVolume    (scene_emitter.Gain ());
    }
    
///Обработчик начала проигрывания звука в эмиттере
    void OnPlay ()
    {
      if (is_playing)
        OnStop ();
            
      emitter.SetSampleIndex (rand ());

      is_playing        = true;
      play_start_time   = common::milliseconds ();
      play_start_offset = 0.f;

      if (!sound_manager)
        return;

      sound_manager->PlaySound (emitter);

      scene_emitter.AddRef ();
      
      need_release_scene_emitter = true;

      if (!sound_manager->IsLooping (emitter))
      {
        auto_stop_action = common::ActionQueue::PushAction (xtl::bind (&ScenePlayerEmitter::OnStop, this),
          common::ActionThread_Current, sound_manager->Duration (emitter) + STOP_EMITTER_DELAY);
      }      
    }

///Обработчик остановки проигрывания звука в эмиттере
    void OnStop ()
    {      
      if (!is_playing)
        return;                
      
      auto_stop_action.Cancel ();
      
      auto_stop_action = Action ();
      is_playing       = false;      

      if (sound_manager)
        sound_manager->StopSound (emitter);

      if (need_release_scene_emitter)
      {
        need_release_scene_emitter = false;

        scene_emitter.Release (); //должна быть последней операцией, поскольку может инициировать удаление текущего объекта
      }      
    }

///Обработчик события изменения сцены
    void OnSceneChange ()
    {     
      delete this;
    }

  private:
    SoundManager*        sound_manager;              //менеджер звуков
    SoundEmitter&        scene_emitter;              //эмиттер в сцене
    sound::Emitter       emitter;                    //эмиттер в менеджере звуков
    size_t               play_start_time;            //время начала проигрывания
    float                play_start_offset;          //смещение начала проигрывания
    bool                 is_playing;                 //проигрывается ли звук
    bool                 need_release_scene_emitter; //нужно освобождать эмиттер в сцене
    Action               auto_stop_action;           //действие автоматической остановки звука
    xtl::auto_connection on_change_scene;            //соединение события изменения сцены
};

/*
    Слушатель сцены
*/

class ScenePlayerListener: public xtl::trackable
{
  public:
///Конструктор  
    ScenePlayerListener (scene_graph::Listener& in_scene_listener)
      : sound_manager (0)
      , scene_listener (in_scene_listener)
    {
      connect_tracker (scene_listener.RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&ScenePlayerListener::OnUpdate, this)));
      connect_tracker (scene_listener.RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&ScenePlayerListener::OnDestroy, this)));
    }
    
///Деструктор
    ~ScenePlayerListener ()
    {
      try
      {
        SetSoundManager (0);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
///Изменение менеджера звуков
    void SetSoundManager (SoundManager* in_sound_manager)
    {
      if (sound_manager)
      {
        sound_manager->SetMute (true);
      }
      
      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
        
      sound_manager->SetMute (false);
      
      OnUpdate ();
    }
    
    SoundManager* GetSoundManager () { return sound_manager; }
    
///Слушатель сцены
    scene_graph::Listener& GetSceneListener () { return scene_listener; }

  private:
///Обработка события обновления слушателя
    void OnUpdate ()
    {
      if (!sound_manager)
        return;        

      sound::Listener sound_listener;

      math::mat4f listener_world_tm = scene_listener.WorldTM ();

      sound_listener.position  = listener_world_tm * math::vec3f (0.f);
      sound_listener.direction = listener_world_tm * math::vec4f (0.f, 0.f, -1.f, 0.f);
      sound_listener.up        = listener_world_tm * math::vec4f (0.f, 1.f, 0.f, 0.f);

      sound_manager->SetListener (sound_listener);
      sound_manager->SetVolume   (scene_listener.Gain ());
    }
    
///Обработка события удаления слушателя
    void OnDestroy ()
    {
      delete this;
    }

  private:
    SoundManager*          sound_manager;  //менеджер звуков
    scene_graph::Listener& scene_listener; //слушатель в сцене
};

/*
    Описание реализация проигрывателя звуков сцены
*/

struct ScenePlayer::Impl
{
  public:
///Конструктор
    Impl ()
      : sound_manager (0)
      , scene (0)
    {
    }

///Деструктор
    ~Impl ()
    {
      try      
      {
        SetSoundManager (0);
        SetListener (0);
        SetScene (0);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Установка слушателя
    void SetListener (scene_graph::Listener* in_listener, bool need_destroy_listener = true)
    {
      if (!listener && !in_listener || listener && &listener->GetSceneListener () == in_listener)
        return;        
        
      on_destroy_listener.disconnect ();
      on_change_listener_scene.disconnect ();        
      
      if (need_destroy_listener) listener.reset ();
      else                       listener.release ();      

      if (!in_listener)
      {
        SetScene (0);
        return;
      }

      listener = stl::auto_ptr<ScenePlayerListener> (new ScenePlayerListener (*in_listener));

      try
      {
        SetScene (in_listener->Scene ());

        listener->SetSoundManager (sound_manager);
        
        on_destroy_listener      = listener->connect_tracker (xtl::bind (&ScenePlayer::Impl::OnDestroyListener, this));
        on_change_listener_scene = in_listener->RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&ScenePlayer::Impl::OnChangeListenerScene, this));
      }
      catch (...)
      {
        SetListener (0);
        throw;
      }      
    }

    scene_graph::Listener* GetListener () const
    {
      return &listener->GetSceneListener ();
    }

///Установка менеджера
    void SetSoundManager (sound::SoundManager* in_sound_manager)
    {
      if (sound_manager == in_sound_manager)
        return;
        
      on_manager_destroy_connection.disconnect ();
        
      if (listener)
        listener->SetSoundManager (in_sound_manager);

      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        i->second->SetSoundManager (in_sound_manager);        

      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
      
      try
      {
        on_manager_destroy_connection = sound_manager->RegisterDestroyHandler (xtl::bind (&ScenePlayer::Impl::SetSoundManager, this, (SoundManager*)0));
      }
      catch (...)
      {
        SetSoundManager (0);
        throw;
      }
    }

    SoundManager* GetSoundManager () const
    {
      return sound_manager;
    }

  private:
///Смена сцены
    void SetScene (Scene* in_scene)
    {
      if (in_scene == scene)
        return;        
        
      on_bind_node_connection.disconnect ();
        
        //удаление всех эмиттеров
        
      while (!emitters.empty ())      
        delete emitters.begin ()->second;
        
      emitters.clear ();
                        
        //изменение сцены

      scene = in_scene;      
      
      if (!scene)
        return;

      try
      {
          //добавление всех эмиттеров доступных в сцене

        scene->Traverse (xtl::bind (&ScenePlayer::Impl::OnNewNode, this, _1));

          //подписка на появление новых объектов

        on_bind_node_connection = scene->Root ().RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&ScenePlayer::Impl::OnNewNode, this, _2)); 
      }
      catch (...)
      {
        SetScene (0);
        throw;
      }      
    }
    
///Обработка события изменения сцены у слушателя
    void OnChangeListenerScene ()
    {
      if (!listener)
        return;
      
      SetScene (listener->GetSceneListener ().Scene ());
    }
    
///Обработка события удаления слушателя
    void OnDestroyListener ()
    {
      SetListener (0, false);
    }
  
///Обработка события появления нового узла
    void OnNewNode (Node& node)
    {
      scene_graph::SoundEmitter* emitter = dynamic_cast<scene_graph::SoundEmitter*> (&node);

      if (!emitter)
        return;

      stl::auto_ptr<ScenePlayerEmitter> scene_player_emitter (new ScenePlayerEmitter (*emitter));

      scene_player_emitter->SetSoundManager (sound_manager);

      scene_player_emitter->connect_tracker (xtl::bind (&ScenePlayer::Impl::OnDestroyEmitter, this, emitter));

      emitters.insert_pair (emitter, scene_player_emitter.get ());

      scene_player_emitter.release ();
    }

///Обработка события удаления эмиттера
    void OnDestroyEmitter (SoundEmitter* node)
    {
      emitters.erase (node);
    }

  private:
    typedef stl::hash_map<SoundEmitter*, ScenePlayerEmitter*> EmitterSet;      

  private:
    stl::auto_ptr<ScenePlayerListener> listener;                      //слушатель
    sound::SoundManager*               sound_manager;                 //менеджер звуков
    EmitterSet                         emitters;                      //эмиттеры
    Scene*                             scene;                         //текущая сцена
    xtl::auto_connection               on_manager_destroy_connection; //соединение события удаления менеджера    
    xtl::auto_connection               on_bind_node_connection;       //соединение события появления нового узла в сцене
    xtl::auto_connection               on_destroy_listener;           //соединение события удаления слушателя
    xtl::auto_connection               on_change_listener_scene;      //соединение события изменения сцены слушателя
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
  impl->SetSoundManager (sound_manager);
}

sound::SoundManager* ScenePlayer::Manager () const
{
  return impl->GetSoundManager ();
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
  return impl->GetListener ();
}
