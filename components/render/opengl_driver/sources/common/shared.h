#ifndef RENDER_GL_DRIVER_COMMON_SHARED_HEADER
#define RENDER_GL_DRIVER_COMMON_SHARED_HEADER

#include <cstring>
#include <cstdarg>

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
    ����������� ����� ��������� OpenGL
*/

void detect_opengl_bugs (stl::string& extensions_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Version
{
  size_t major_version;  //������� ����� ������
  size_t minor_version;  //������� ����� ������
  size_t build_number;   //����� ������

///////////////////////////////////////////////////////////////////////////////////////////////////  
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
  Version ();
  Version (const char* string);

///////////////////////////////////////////////////////////////////////////////////////////////////  
///�������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool IsValid () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool operator <  (const Version&) const;
  bool operator == (const Version&) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextSettings
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextSettings  (const char* init_string);
    ~ContextSettings ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsNeedCheckErrors () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ExtensionSet& RequiredExtensions () const; //������������ ����������
    const ExtensionSet& EnabledExtensions  () const; //����������� ����������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Version& MinVersion () const; //����������� ��������� ������ OpenGL
    const Version& MaxVersion () const; //������������ ������ (��� ������� ����������� ����������������)

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
