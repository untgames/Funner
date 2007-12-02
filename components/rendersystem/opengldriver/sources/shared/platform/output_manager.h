#ifndef RENDER_GL_DRIVER_OUTPUT_MANAGER_HEADER
#define RENDER_GL_DRIVER_OUTPUT_MANAGER_HEADER

#include <render/low_level/driver.h>
#include <stl/auto_ptr.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ��������������� ������ � ��������������� ������������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputManager  ();
    ~OutputManager ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount () const;
    IOutput* GetOutput       (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������, ���������� �������� � ����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IOutput* FindContainingOutput (void* window_handle) const;

  private:
    OutputManager (const OutputManager&); //no impl
    OutputManager& operator = (const OutputManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
