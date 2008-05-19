#include <stl/hash_map>
#include <xtl/signal.h>
#include <xtl/bind.h>
#include <common/exception.h>
#include <sg/scene.h>
#include <sg/node.h>
#include <sg/entity.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sound/sgplayer.h>
#include <math/mathlib.h>

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
    Описание реализации SGPlayer
*/

struct SGPlayerEmitter
{
  sound::Emitter        emitter;     //эмиттер

  SGPlayerEmitter (SoundEmitter* sound_emitter, const char* source_name);
  SGPlayerEmitter (const SGPlayerEmitter& source);
};

SGPlayerEmitter::SGPlayerEmitter (SoundEmitter* sound_emitter, const char* source_name) 
  : emitter (source_name) 
  {}

SGPlayerEmitter::SGPlayerEmitter (const SGPlayerEmitter& source)
  : emitter (source.emitter.Source ())
  {}

typedef stl::hash_map<scene_graph::SoundEmitter*, SGPlayerEmitter> EmitterSet;

struct SGPlayer::Impl
{
  scene_graph::Listener* listener;           //слушатель
  sound::SoundManager&   sound_manager;      //менеджер
  EmitterSet             emitters;           //эмиттеры

  Impl (sound::SoundManager& in_sound_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление свойств слушателя/эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
  void ListenerUpdate (Node& sender, NodeEvent event);
  void EmitterUpdate  (Node& sender, NodeEvent event);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление Node сцены в список, если является эмиттером
///////////////////////////////////////////////////////////////////////////////////////////////////
  void CheckNode (scene_graph::Node& node);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка добавления/удаления нодов в сцене
///////////////////////////////////////////////////////////////////////////////////////////////////
  void ProcessAttachNode (Node& sender, Node& node, NodeSubTreeEvent event);
  void ProcessDetachNode (Node& sender, Node& node, NodeSubTreeEvent event);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проигрывание эмиттеров
///////////////////////////////////////////////////////////////////////////////////////////////////
  void PlayEmitter (SoundEmitter& emitter, SoundEmitterEvent event);
  void StopEmitter (SoundEmitter& emitter, SoundEmitterEvent event);
};

SGPlayer::Impl::Impl (sound::SoundManager& in_sound_manager) 
  : listener (0),
    sound_manager (in_sound_manager)
  {}

/*
   Обновление свойств слушателя/эмиттеров
*/

void SGPlayer::Impl::ListenerUpdate (Node& sender, NodeEvent event)
{
  if (!listener)
    return;

  sound::Listener snd_listener;
    
  snd_listener.position  = listener->WorldPosition ();                              //!!!!!!!!добавить скорость
  snd_listener.direction = listener->WorldOrientation () * vec3f(0.f,0.f,1.f);
  snd_listener.up        = listener->WorldOrientation () * vec3f(0.f,1.f,0.f);

  sound_manager.SetListener (snd_listener);
}

void SGPlayer::Impl::EmitterUpdate (Node& sender, NodeEvent event)
{
  EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&sender));

  if (emitter_iter == emitters.end ())
    return;

  emitter_iter->second.emitter.SetPosition (sender.WorldPosition ());               //!!!!!!!!добавить скорость
  emitter_iter->second.emitter.SetDirection (sender.WorldOrientation () * vec3f(0.f,0.f,1.f));
}

/*
   Добавление Entity сцены в список, если является эмиттером
*/

void SGPlayer::Impl::CheckNode (scene_graph::Node& node)
{
  scene_graph::SoundEmitter* emitter = dynamic_cast<scene_graph::SoundEmitter*> (&node);

  if (emitter)
  {
    SGPlayerEmitter sgplayer_emitter (emitter, emitter->SoundDeclarationName ());

    emitters.insert_pair (emitter, sgplayer_emitter);
    emitter->Event (SoundEmitterEvent_Play).connect (bind (&SGPlayer::Impl::PlayEmitter, this, _1, _2));
    emitter->Event (SoundEmitterEvent_Stop).connect (bind (&SGPlayer::Impl::StopEmitter, this, _1, _2));
    ((scene_graph::Node*)emitter)->Event (NodeEvent_AfterUpdate).connect (bind (&SGPlayer::Impl::EmitterUpdate, this, _1, _2));
  }
}

/*
   Обработка добавления/удаления нодов в сцене
*/

void SGPlayer::Impl::ProcessAttachNode (Node& sender, Node& node, NodeSubTreeEvent event)
{
  CheckNode (node);
}

void SGPlayer::Impl::ProcessDetachNode (Node& sender, Node& node, NodeSubTreeEvent event)
{
  EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&node));

  if (emitter_iter == emitters.end ())
    return;

  emitters.erase (emitter_iter);
}

/*
   Проигрывание эмиттеров
*/

void SGPlayer::Impl::PlayEmitter (SoundEmitter& emitter, SoundEmitterEvent event)
{
  EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&emitter));

  if (emitter_iter == emitters.end ())
    return;

  sound_manager.PlaySound (emitter_iter->second.emitter);
}

void SGPlayer::Impl::StopEmitter (SoundEmitter& emitter, SoundEmitterEvent event)
{
  EmitterSet::iterator emitter_iter = emitters.find ((scene_graph::SoundEmitter*) (&emitter));

  if (emitter_iter == emitters.end ())
    return;

  sound_manager.StopSound (emitter_iter->second.emitter);
}

/*
   Конструктор / деструктор
*/

SGPlayer::SGPlayer (sound::SoundManager& sound_manager)
  : impl (new Impl (sound_manager))
  {}

SGPlayer::~SGPlayer ()
{
}

/*
   Установка слушателя
*/

void SGPlayer::SetListener (scene_graph::Listener& listener)
{
  scene_graph::Scene* scene = listener.Scene ();

  if (!scene)
    Raise <Exception> ("sound::SGPlayer::Impl::Impl", "Listener is not binded to any scene.");

  if (impl->listener)
    if (impl->listener->Scene () == scene)
    {
      impl->listener = &listener;
      return;
    }

  for (EmitterSet::iterator i = impl->emitters.begin (); i != impl->emitters.end (); ++i)
    impl->sound_manager.StopSound (i->second.emitter);

  impl->emitters.erase (impl->emitters.begin (), impl->emitters.end ());

  scene->Traverse (xtl::bind (&SGPlayer::Impl::CheckNode, impl.get (), _1));

  scene->Root ().Event (NodeSubTreeEvent_AfterBind).connect  (bind (&SGPlayer::Impl::ProcessAttachNode, impl.get (), _1, _2, _3));
  scene->Root ().Event (NodeSubTreeEvent_BeforeUnbind).connect (bind (&SGPlayer::Impl::ProcessDetachNode, impl.get (), _1, _2, _3));

  listener.Event (NodeEvent_AfterUpdate).connect (bind (&SGPlayer::Impl::ListenerUpdate, impl.get (), _1, _2));
}
