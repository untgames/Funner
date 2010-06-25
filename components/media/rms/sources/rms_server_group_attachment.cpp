#include "shared.h"

using namespace media::rms;

/*
    Конструктор / деструктор
*/

ServerGroupAttachment::ServerGroupAttachment (const char* name, ICustomServer& in_server)
  : group (name)
  , server (&in_server)
{
  group.Attach (*server);
}

ServerGroupAttachment::~ServerGroupAttachment ()
{
  group.Detach (*server);
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
