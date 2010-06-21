#include "shared.h"

using namespace common;
using namespace stl;

ClosedFileImpl::ClosedFileImpl ()
  : FileImpl (0u)
  { }
  
void ClosedFileImpl::Raise (const char* source)
{
  throw xtl::format_operation_exception (source, "File already closed");
}

const char* ClosedFileImpl::GetPath ()
{
  Raise ("File::GetPath");
}

size_t ClosedFileImpl::Read (void*,size_t)
{
  Raise ("File::Read");
  return 0;
}

size_t ClosedFileImpl::Write (const void*,size_t)
{
  Raise ("File::Write");
  return 0;
}

filepos_t ClosedFileImpl::Tell ()
{
  Raise ("File::Tell");
  return 0;
}

filepos_t ClosedFileImpl::Seek (filepos_t)
{
  Raise ("File::Seek");
  return 0;
}

void ClosedFileImpl::Rewind ()
{
  Raise ("File::Rewind");
}

filesize_t ClosedFileImpl::Size ()
{
  Raise ("File::Size");
  return 0;
}

void ClosedFileImpl::Resize (filesize_t)
{
  Raise ("File::Resize");
}

bool ClosedFileImpl::Eof ()
{
  Raise ("File::Eof");
  return false;
}

void ClosedFileImpl::Flush ()
{
  Raise ("File::Instance");
}

FileImpl* ClosedFileImpl::Instance ()
{
  return &*Singleton<ClosedFileImpl>::Instance (); //???
}
