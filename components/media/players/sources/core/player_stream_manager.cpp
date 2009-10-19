#include "shared.h"

using namespace media::players;

namespace
{

/*
    Реализация менеджера проигрывателей медиа-потоков
*/

class StreamPlayerManagerImpl
{
  public:
    typedef StreamPlayerManager::PlayerCreator PlayerCreator;

///Регистрация проигрывателя медиа-потоков
    void RegisterPlayer (const char* target, const PlayerCreator& creator)
    {
      static const char* METHOD_NAME = "media::players::StreamPlayerManagerImpl::RegisterPlayer";
      
      if (!target)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target");
        
      CreatorMap::iterator iter = creators.find (target);
      
      if (iter == creators.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "target", target, "Target with this name has already registered");
        
      creators.insert_pair (target, creator);
    }

///Отмена регистрации проигрывателя медиа-потоков
    void UnregisterPlayer (const char* target)
    {
      if (!target)
        return;
        
      creators.erase (target);
    }

///Отмена регистрации всех проигрывателей медиа-потоков
    void UnregisterAllPlayers ()
    {
      creators.clear ();
    }
    
///Создание проигрывателя медиа-потока
    IStreamPlayer* CreatePlayer (const char* target, const char* stream_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      try
      {
        if (!target)
          throw xtl::make_null_argument_exception ("", "target");

        if (!stream_name)
          throw xtl::make_null_argument_exception ("", "stream_name");

        CreatorMap::iterator iter = creators.find (target);

        if (iter == creators.end ())
          throw xtl::make_argument_exception ("", "target", target, "Unknown media player target");

        return iter->second (target, stream_name, handler);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::players::StreamPlayerManagerImpl::CreatePlayer");
        throw;
      }
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, PlayerCreator> CreatorMap;

  private:
    CreatorMap creators; //карта функторов, создающих проигрыватели медиа-поток
};

typedef common::Singleton<StreamPlayerManagerImpl> StreamPlayerSingleton;

}

/*
    Обёртки над вызовами к менеджеру потоков
*/

void StreamPlayerManager::RegisterPlayer (const char* target, const PlayerCreator& creator)
{
  StreamPlayerSingleton::Instance ()->RegisterPlayer (target, creator);
}

void StreamPlayerManager::UnregisterPlayer (const char* target)
{
  StreamPlayerSingleton::Instance ()->UnregisterPlayer (target);
}

void StreamPlayerManager::UnregisterAllPlayers ()
{
  StreamPlayerSingleton::Instance ()->UnregisterAllPlayers ();
}

namespace media
{

namespace player
{

IStreamPlayer* create_stream_player (const char* target, const char* stream_name, const StreamPlayerManager::StreamEventHandler& handler)
{
  return StreamPlayerSingleton::Instance ()->CreatePlayer (target, stream_name, handler);
}

}

}
