#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

///ƒескриптор анимационной библиотеки
struct AnimationLibraryDesc: public xtl::reference_counter
{
  stl::string                        name;    //им€ библиотеки
  media::animation::AnimationLibrary library; //библиотека
};

}

/*
    ќписание реализации менеджера анимаций
*/

typedef xtl::intrusive_ptr<AnimationLibraryDesc>                                 AnimationLibraryPtr;
typedef stl::list<AnimationLibraryPtr>                                           AnimationLibraryList;
typedef stl::hash_map<stl::hash_key<const char*>, media::animation::Animation*>  AnimationMap;

struct AnimationManager::Impl: public xtl::reference_counter
{
  AnimationLibraryList animation_libraries;        //библиотеки анимации
  AnimationMap         animations;                 //анимации
  bool                 need_rebuild_animation_map; //карта анимаций требует перестроени€
  
/// онструктор
  Impl ()
    : need_rebuild_animation_map (false)
  {
  }
  
///ѕостроение карты анимаций
  void RebuildAnimationMap ()
  {
    if (!need_rebuild_animation_map)
      return;
      
    animations.clear ();
    
    for (AnimationLibraryList::iterator iter=animation_libraries.begin (), end=animation_libraries.end (); iter!=end; ++iter)
    {
      media::animation::AnimationLibrary& library = (*iter)->library;
      
      for (media::animation::AnimationLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
      {
        media::animation::Animation& animation = *iter;
        
        animations [library.ItemId (iter)] = &animation;
      }
    }
    
    need_rebuild_animation_map = false;
  }
};

/*
     онструкторы / деструктор / присваивание
*/

AnimationManager::AnimationManager ()
  : impl (new Impl)
{
}

AnimationManager::AnimationManager (const AnimationManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

AnimationManager::~AnimationManager ()
{
  release (impl);
}

AnimationManager& AnimationManager::operator = (const AnimationManager& manager)
{
  AnimationManager (manager).Swap (*this);

  return *this;
}

/*
    —оздание анимации
*/

Animation AnimationManager::CreateAnimation (const char* name, Node& root)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
      //перестроение карты анимаций

    impl->RebuildAnimationMap ();

      //поиск анимации

    AnimationMap::iterator iter = impl->animations.find (name);

    if (iter == impl->animations.end ())
      throw xtl::make_argument_exception ("", "name", name, "Animation not found");
      
    media::animation::Animation& animation = *iter->second;

      //поиск или создание анимационного контроллера

    AnimationController::Pointer controller = root.FirstController<AnimationController> ();
    
    if (!controller)
    {
      controller = AnimationController::Create (root, *this);

      if (!controller)
        throw xtl::format_operation_exception ("", "Controller is null");
    }    

      //создание анимации

    return controller->CreateAnimation (animation);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationManager::CreateAnimation");
    throw;
  }
}

Animation AnimationManager::PlayAnimation (const char* name, Node& root)
{
  return PlayAnimation (name, root, Animation::EventHandler ());
}

Animation AnimationManager::PlayAnimation (const char* name, Node& root, const Animation::EventHandler& on_finish)
{
  try
  {
    Animation animation = CreateAnimation (name, root);

    if (on_finish) animation.RegisterEventHandler (AnimationEvent_OnFinish, on_finish);

    animation.Play ();

    return animation;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationManager::PlayAnimation");
    throw;
  }
}

/*
    ”правление ресурсами
*/

void AnimationManager::LoadResource (const char* name)
{
  try
  {
      //проверка корректности аргументов    

    if (!name)
      throw xtl::make_null_argument_exception ("", "name");      

    for (AnimationLibraryList::iterator iter=impl->animation_libraries.begin (), end=impl->animation_libraries.end (); iter!=end; ++iter)
      if ((*iter)->name == name)
        throw xtl::make_argument_exception ("", "name", name, "Animation library has already loaded");

      //создание библиотеки

    AnimationLibraryPtr desc (new AnimationLibraryDesc, false);

    desc->name = name;

    desc->library.Load (name);

      //регистраци€ библиотеки

    impl->animation_libraries.push_back (desc);

    impl->need_rebuild_animation_map = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationManager::LoadResource");
    throw;
  }
}

void AnimationManager::UnloadResource (const char* name)
{
  if (!name)
    return;
    
  for (AnimationLibraryList::iterator iter=impl->animation_libraries.begin (), end=impl->animation_libraries.end (); iter!=end; ++iter)
    if ((*iter)->name == name)
    {
      impl->animation_libraries.erase (iter);

      impl->need_rebuild_animation_map = true;

      return;
    }    
}
    
/*
    ќбмен
*/

void AnimationManager::Swap (AnimationManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace scene_graph
{

namespace controllers
{
    
void swap (AnimationManager& manager1, AnimationManager& manager2)
{
  manager1.Swap (manager2);
}

}

}
