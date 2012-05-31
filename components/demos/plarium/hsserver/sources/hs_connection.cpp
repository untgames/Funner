#include "shared.h"

using namespace plarium::hsserver;
using namespace plarium::system;
using namespace plarium::utility;

/*
   HS Server Connection implementation
*/

struct HsConnection::Impl
{
  TcpClient tcp_client;
};

/*
   Constructor / destructor
*/

HsConnection::HsConnection ()
  : impl (new Impl)
  {}

HsConnection::~HsConnection ()
{
  delete impl;
}

/*
   Connection
*/

void HsConnection::Connect (const unsigned char (&address)[4], unsigned short port)
{
  impl->tcp_client.Connect (address, port);
}

void HsConnection::Close ()
{
  impl->tcp_client.Close ();
}

/*
   Sending events
*/

void HsConnection::SendMessage (unsigned short plugin_id, const char* message)
{

}

/*
   Event listening
*/

void HsConnection::SetListener (IHsConnectionListener* listener)
{

}

IHsConnectionListener* HsConnection::Listener () const
{

}

/*
   Keep alive options (0 - disabled, default - 10000)
*/

void HsConnection::SetKeepAliveInterval (size_t milliseconds)
{

}

size_t HsConnection::KeepAliveInterval () const
{

}

/*
   Sending options
*/

void HsConnection::SetCompressionEnabled (bool state)
{

}

void HsConnection::SetCompressionThreshold (size_t threshold)
{

}

void HsConnection::SetEncryptionEnabled (bool state)
{

}

void HsConnection::SetEncryptionKey (const unsigned char* key, size_t key_bits)
{

}

bool HsConnection::IsCompressionEnabled () const
{

}

size_t HsConnection::CompressionThreshold () const
{

}

bool HsConnection::IsEncrypionEnabled () const
{

}

const unsigned char* HsConnection::EncryptionKey () const
{

}

size_t HsConnection::EncryptionKeySize () const
{

}

/*
   Connection state
*/

bool HsConnection::IsConnected () const
{

}
