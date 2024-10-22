#ifndef RENDER_GL_DRIVER_COMMON_SHARED_HEADER
#define RENDER_GL_DRIVER_COMMON_SHARED_HEADER

#include <cstring>
#include <cstdarg>
#include <cmath>

#include <render/low_level/device.h>
#include <render/low_level/debug.h>

#include <shared/command_list.h>
#include <shared/context_object.h>
#include <shared/log.h>
#include <shared/platform.h>
#include <shared/property_list.h>

#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>
#include <stl/bitset>

#include <xtl/array>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    Определение багов драйверов OpenGL
*/

void detect_opengl_bugs (stl::string& extensions_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Версия OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Version
{
  size_t major_version;  //старший номер версии
  size_t minor_version;  //младший номер версии
  size_t build_number;   //номер сборки

///////////////////////////////////////////////////////////////////////////////////////////////////  
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
  Version ();
  Version (const char* string);

///////////////////////////////////////////////////////////////////////////////////////////////////  
///Проверка корректности версии
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool IsValid () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение версий
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool operator <  (const Version&) const;
  bool operator == (const Version&) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Целочисленные настройки контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ContextSettingsInteger
{
  ContextSettingsInteger_MaxTextureSize,    //максимальный размер текстур
  ContextSettingsInteger_MaxAnisotropy,     //максимально возможная степень анизотропии
  ContextSettingsInteger_TextureUnitsCount, //количество слотов мультитекстурирования

  ContextSettingsInteger_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextSettings
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextSettings  (const char* init_string);
    ~ContextSettings ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Нужно ли проверять ошибки / необходимо ли валидировать программы
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsNeedCheckErrors      () const;
    bool IsNeedValidatePrograms () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Множества расширений
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ExtensionSet& RequiredExtensions () const; //обязательные расширения
    const ExtensionSet& EnabledExtensions  () const; //разрешенные расширения

///////////////////////////////////////////////////////////////////////////////////////////////////
///Версии OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Version& MinVersion () const; //минимальная требуемая версия OpenGL
    const Version& MaxVersion () const; //максимальная версия (для ручного ограничения функциональности)

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение целочисленных настроек контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int GetInteger (ContextSettingsInteger) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разрешено ли ffp
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsFfpAllowed () const;

  private:
    ContextSettings (const ContextSettings&); //no impl
    ContextSettings& operator = (const ContextSettings&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
