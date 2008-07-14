#include "shared.h"
#include <memory.h>

#include <common/hash.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& in_desc, bool in_need_hash)
  : Buffer (context_manager, in_desc), buffer (in_desc.size), need_hash (in_need_hash)
  {}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

/*
    Установка / чтение данных после отсечения
*/

void SystemMemoryBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  memcpy (buffer.data () + offset, data, size);

  need_data_hash_recalc = true;
}

void SystemMemoryBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  memcpy (data, buffer.data () + offset, size);
}

/*
    Установка буфера в контекст OpenGL
*/

void SystemMemoryBuffer::Bind ()
{
    //ошибка! здесь должен быть сброс текущего буфера в 0!!!

  GetContextDataTable (Stage_Input)[InputStageCache_BindedVboBuffer] = 0; //?????
  
    //установка не требуется
}

/*
    Получение указателя на начало буфера
*/

void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();
}

/*
   Получение хеша данных (0 - данные не хешируются)
*/

size_t SystemMemoryBuffer::GetDataHash ()
{
  if (!need_hash)
    return 0;

  if (need_data_hash_recalc)
  {
    data_hash = crc32 (buffer.data (), buffer.size ());
    need_data_hash_recalc = false;
  }

  return data_hash;
}
