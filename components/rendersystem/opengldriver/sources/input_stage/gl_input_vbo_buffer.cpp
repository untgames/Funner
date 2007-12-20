#include "shared.h"

/*
    Замечания
      1) перед каждым блоком команд GL нужно делать MakeContextCurrent ()
         в деструкторе тоже нужно, но страховать этот вызов в try { MakeContextCurrent (); gl-commands; } catch (...) { nothing }
      2) лучше дескриптор сделать в базовом классе const и proteced
      3) упрощение enum-ов: BufferAccessFlag -> AccessFlag, BufferUsageMode -> UsageMode - они общие в буферах и текстурах
      4) не понял зачем Map/Unmap. там есть методы Set/Get. хотя это нужно посмотреть детальнее
*/

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///Конструктор / деструктор
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer (desc), ContextObject (context_manager), target (in_target)
{ 
  glGenBuffers(1, &buffer_id);            // зарезервируем номер буфера
  
  glBindBuffer(target, buffer_id);     // сделаем его активным
  
  BufferDesc bd;                          // опять дескриптор! надоел!
  GetDesc(bd);
  
  size_t buffermode;                      // переменная под режим буфера
  
  switch (bd.usage_mode)                  // что поделаешь, под каждую пару доступ-режим - отдельный флаг
  {
    case UsageMode_Default:         // по дефолту делаем динамик, это перестраховка, а вообще 
      switch (bd.access_flags)            // надо дефолт нафик убрать! потому как в реализации его нет
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Static:          // статический буфер
      switch (bd.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STATIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STATIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STATIC_COPY;
          break;
      }
      break;
    case UsageMode_Dynamic:         // динамический буфер
      switch (bd.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Stream:          // поточный буфер
      switch (bd.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STREAM_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STREAM_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STREAM_COPY;
          break;
      }
      break;  
  }
  
  glBufferData(target, bd.size, NULL, buffermode); // инициализируем буфер
}

VboBuffer::~VboBuffer ()
{
  glDeleteBuffers(1, &buffer_id);                     // уничтожаем буфер
}

///Работа с данными буфера
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  Bind();
  
  glBufferSubData(target, offset, size, data);
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{

  BufferDesc bd;                                // и снова дескриптор
  GetDesc(bd);

  if (offset < 0 || offset >= bd.size)          // смещение
    return;

  Bind();

  void* buffer;
  buffer = glMapBuffer(target, GL_READ_ONLY);   // мапим буфер
  
  // указатели
  char* begin = (char*)buffer + offset;
  char* end   = (char*)buffer + offset + size;
  char* ptr   = (char*)data;
  // копирование
  for (; begin < end; begin++, ptr++)
    *ptr = *begin;
  
  glUnmapBuffer(target);                        // анмапим буфер
}

///Установка буфера в контекст OpenGL
void VboBuffer::Bind ()
{
  glBindBuffer(target, buffer_id);
}

///Cмещение от начала буфера (для аппаратных буферов)
void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
