#ifndef COMMON_STRLIB_SHARED_HEADER
#define COMMON_STRLIB_SHARED_HEADER

#include <common/strconv.h>
#include <common/singleton.h>
#include <common/utf_converter.h>
#include <stl/hash_map>
#include <xtl/function.h>

namespace common
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон менеджера конвертеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverterManager
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор синглтона
///////////////////////////////////////////////////////////////////////////////////////////////////
  StringConverterManager();
///////////////////////////////////////////////////////////////////////////////////////////////////
///Методы
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool RegisterConverter (const char*, const char*, const StringConverterSystem::ConverterFn&);
  void UnregisterConverter (const char*, const char*);
  void UnregisterAllConverters ();
  bool IsConverterRegistered (const char*, const char*);
  IStringConverter* GetConverter(const char*, const char*);

private:
  typedef stl::hash_map<size_t, StringConverterSystem::ConverterFn> StringConverterMap;
  StringConverterMap string_converter_map;
};

typedef common::Singleton<StringConverterManager> StringConverterManagerSingleton;

} // namespace common

#endif