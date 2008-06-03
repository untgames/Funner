#include "shared.h"

using namespace common;
using namespace stl;

ClosedFileImpl::ClosedFileImpl ()
  : FileImpl ((size_t)-1)
  { }
  
void ClosedFileImpl::raise (const char* source)
{
  throw FileClosedException (source, "File already closed");
}

size_t ClosedFileImpl::Read (void*,size_t)
{
  raise ("File::Read");
  return 0;
}

size_t ClosedFileImpl::Write (const void*,size_t)
{
  raise ("File::Write");
  return 0;
}

filepos_t ClosedFileImpl::Tell ()
{
  raise ("File::Tell");
  return 0;
}

filepos_t ClosedFileImpl::Seek (filepos_t)
{
  raise ("File::Seek");
  return 0;
}

void ClosedFileImpl::Rewind ()
{
  raise ("File::Rewind");
}

filesize_t ClosedFileImpl::Size ()
{
  raise ("File::Size");
  return 0;
}

void ClosedFileImpl::Resize (filesize_t)
{
  raise ("File::Resize");
}

bool ClosedFileImpl::Eof ()
{
  raise ("File::Eof");
  return false;
}

void ClosedFileImpl::Flush ()
{
  raise ("File::Instance");
}

FileImpl* ClosedFileImpl::Instance ()
{
  return Singleton<ClosedFileImpl>::InstancePtr ();
}
