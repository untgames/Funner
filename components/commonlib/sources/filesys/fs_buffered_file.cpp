#include "shared.h"

using namespace common;
using namespace stl;

BufferedFileImpl::BufferedFileImpl (FileImplPtr _base_file,size_t buf_size)
  : FileImpl (_base_file->Mode ()), base_file (_base_file)
{
  buffer       = (char*)::operator new (buf_size);
  buffer_size  = buf_size;
  dirty_start  = 0;
  dirty_finish = 0;
  cache_start  = 0;
  cache_finish = 0;
  file_pos     = base_file->Tell ();
  file_size    = base_file->Size ();
}

BufferedFileImpl::~BufferedFileImpl ()
{
  try
  {
    Flush ();
  }
  catch (...)
  {
  }

  ::operator delete (buffer);
}

void BufferedFileImpl::ResetBuffer (filepos_t new_cache_start)
{
  FlushBuffer ();

  if (Mode () & FILE_MODE_READ)
  {
    if (base_file->Tell () != new_cache_start && base_file->Seek (new_cache_start) != new_cache_start)
      BufferedFileException ("Can not seek base file").Raise ("BufferedFileImpl::ResetBuffer");

    cache_finish = new_cache_start + base_file->Read (buffer,buffer_size);
  }
  else cache_finish = new_cache_start + buffer_size;

  cache_start = new_cache_start;  
}

size_t BufferedFileImpl::ReadBuffer (filepos_t position,void* buf,size_t size)
{  
  if (position < cache_start || position >= cache_finish)
    return 0;

  if ((filesize_t)size > (filesize_t)(cache_finish - position))
    size = cache_finish - position;

  memcpy (buf,buffer+position-cache_start,size);

  return size;
}

size_t BufferedFileImpl::WriteBuffer (filepos_t position,const void* buf,size_t size)
{  
  if (position < cache_start || position > cache_finish)
    return 0;

  size_t block_start = position - cache_start;
         
  if (size > buffer_size - block_start)           size          = buffer_size - block_start;
  if ((filesize_t)(cache_finish-position) < size) cache_finish += size;

  size_t block_finish = block_start + size;

  memcpy (buffer+block_start,buf,size);

  if (block_start  < dirty_start)  dirty_start  = block_start;
  if (block_finish > dirty_finish) dirty_finish = block_finish;

  return size;
}

size_t BufferedFileImpl::Read (void* buf,size_t size)
{
  char*  read_buffer = (char*)buf;
  size_t read_size   = 0,
         tail_size   = 0; //размер прочитанного хвоста блока

  if ((filesize_t)(file_size - file_pos) < (filesize_t)size)
    size = file_size - file_pos;

  if (!size)
    return 0;
    
  filepos_t block_finish_file_pos = file_pos + size;

  if (file_pos >= cache_start)
  {
    if (file_pos < cache_finish) //если голова блока находится в кэше
    {     
      read_size    = ReadBuffer (file_pos,read_buffer,size);
      read_buffer += read_size;
      size        -= read_size;
      file_pos    += read_size;
    }
  }
  else if (block_finish_file_pos >= cache_start && block_finish_file_pos <= cache_finish) //если хвост блока находится в кэше
  {
    size_t offset = cache_start - file_pos;
    read_size     = ReadBuffer (cache_start,read_buffer+offset,size-offset);
    size         -= read_size;
    tail_size     = read_size;
  }

  if (!size)
    return read_size;
    
  filepos_t new_cache_start = (file_pos / buffer_size) * buffer_size; //выравниваем по границе страницы
  size_t    block_start     = file_pos - new_cache_start;

  if (block_start + size > buffer_size) //чтение в обход кэша
  {
    if (base_file->Tell () != file_pos && base_file->Seek (file_pos) != file_pos)
      BufferedFileException ("Can not seek base file").Raise ("BufferedFileImpl::Read");

    size      = base_file->Read (read_buffer,size);
    file_pos += size + tail_size;

    return read_size + size;
  }

  ResetBuffer (new_cache_start);

  size       = ReadBuffer (file_pos,read_buffer,size);
  file_pos  += size + tail_size;
  read_size += size;

  return read_size;
}

size_t BufferedFileImpl::Write (const void* buf,size_t size)
{
  const char* write_buffer = (char*)buf;
  size_t      write_size   = 0;
  size_t      tail_size    = 0; //размер записанного хвоста блока
  
  if ((filesize_t)(file_size - file_pos) < (filesize_t)size)
  {
    if (Mode () & FILE_MODE_RESIZE) Resize (file_pos + size);
    else                            size = file_size - file_pos;
  }

  if (!size)
    return 0;

  filepos_t block_finish_file_pos = file_pos + size;

  if (file_pos >= cache_start)
  {
    if (file_pos <= cache_finish) //если голова блока находится в кэше
    {
      write_size    = WriteBuffer (file_pos,write_buffer,size);
      write_buffer += write_size;
      size         -= write_size;
      file_pos     += write_size;
    }
  }
  else if (block_finish_file_pos > cache_start)
  {      
    if (block_finish_file_pos <= cache_finish) //если хвост блока находится в кэше
    {
      size_t offset = cache_start - file_pos;
      write_size    = WriteBuffer (cache_start,write_buffer+offset,size-offset);
      size         -= write_size;
      tail_size     = write_size;
    }      
    else //если блок накрывает кэш
    {
      if (base_file->Tell () != file_pos && base_file->Seek (file_pos) != file_pos)
        BufferedFileException ("Can not seek base file").Raise ("BufferedFileImpl::Write");

      write_size  = base_file->Write (write_buffer,size);
      file_pos   += write_size;

      if (file_pos > cache_start)
      {
        size_t block_size = min ((filesize_t)(file_pos-cache_start),(filesize_t)buffer_size);

        if (dirty_start  < block_size) dirty_start  = block_size;
        if (dirty_finish < block_size) dirty_finish = block_size;

        if (Mode () & FILE_MODE_READ)
          memcpy (buffer,write_buffer+write_size-block_size,block_size);
      }

      return write_size;
    }
  }
  
  if (!size)
    return write_size;

  filepos_t new_cache_start = (file_pos / buffer_size) * buffer_size; //выравниваем по границе страницы
  size_t    block_start     = file_pos - new_cache_start;

  if (block_start + size > buffer_size) //запись в обход кэша
  {
    if (base_file->Tell () != file_pos && base_file->Seek (file_pos) != file_pos)
      BufferedFileException ("Can not seek base file").Raise ("BufferedFileImpl::Write");

    size      = base_file->Write (write_buffer,size);
    file_pos += size + tail_size;

    return write_size + size;
  }

  ResetBuffer (new_cache_start);

  size        = WriteBuffer (file_pos,write_buffer,size);
  file_pos   += size + tail_size;
  write_size += size;

  return write_size;
}

filepos_t BufferedFileImpl::Tell ()
{
  return (filepos_t)file_pos;
}

filepos_t BufferedFileImpl::Seek (filepos_t new_pos)
{
  return (filepos_t)(file_pos = ((filesize_t)new_pos <= file_size ? new_pos : file_size));
}

void BufferedFileImpl::Rewind ()
{
  file_pos = 0;
}

filesize_t BufferedFileImpl::Size ()
{
  return file_size;
}

void BufferedFileImpl::Resize (filesize_t new_size)
{
  file_size = new_size;
}

bool BufferedFileImpl::Eof ()
{
  return (filesize_t)file_pos == file_size;
}

void BufferedFileImpl::FlushBuffer ()
{
  if (base_file->Size () != file_size)
    base_file->Resize (file_size);

  if (dirty_start == dirty_finish)
    return;

  filepos_t position = cache_start + dirty_start;
  size_t    size     = dirty_finish - dirty_start;

  if (base_file->Tell () != position && base_file->Seek (position) != position)
    BufferedFileException ("Can not seek base file").Raise ("BufferedFileImpl::FlushBuffer");

  if (base_file->Write (buffer+dirty_start,size) != size)
    BufferedFileException ("Can not flush file buffer").Raise ("BufferedFileImpl::FlushBuffer");

  dirty_start = dirty_finish = 0;
}

void BufferedFileImpl::Flush ()
{
  FlushBuffer ();
  base_file->Flush  ();
}
