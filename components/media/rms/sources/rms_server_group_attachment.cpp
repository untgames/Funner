#include "shared.h"

using namespace media::rms;

/*
    Конструктор / деструктор
*/

ServerGroupAttachment::ServerGroupAttachment (const char* name, ICustomServer& in_server)
  : group (name)
  , server (&in_server)
  , notifications_state (true)
{
  group.Attach (*this);
}

ServerGroupAttachment::~ServerGroupAttachment ()
{
  group.Detach (*this);
}

/*
    Получение группы и сервера
*/

ICustomServer& ServerGroupAttachment::Server () const
{
  return *server;
}

media::rms::ServerGroup ServerGroupAttachment::ServerGroup () const
{
  return group;
}

/*
    Управление оповещениями
*/

void ServerGroupAttachment::SetNotificationsState (bool state)
{
  if (state == notifications_state)
    return;

  if (state)
    event_batcher.Flush (*server);

  notifications_state = state;
}

bool ServerGroupAttachment::NotificationsState () const
{
  return notifications_state;
}

/*
    Управление ресурсами
*/

void ServerGroupAttachment::PrefetchResource (const char* resource_name)
{
  if (notifications_state)
  {
    server->PrefetchResource (resource_name);
    return;
  }

  event_batcher.PrefetchResource (resource_name);
}

void ServerGroupAttachment::LoadResource (const char* resource_name)
{
  if (notifications_state)
  {
    server->LoadResource (resource_name);
    return;
  }

  event_batcher.LoadResource (resource_name);
}

void ServerGroupAttachment::UnloadResource (const char* resource_name)
{
  if (notifications_state)
  {
    server->UnloadResource (resource_name);
    return;
  }

  event_batcher.UnloadResource (resource_name);
}
