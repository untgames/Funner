#ifndef RENDER_LOW_LEVEL_HELPERS_PROGRAM_PARAMETERS_LAYOUT_HEADER
#define RENDER_LOW_LEVEL_HELPERS_PROGRAM_PARAMETERS_LAYOUT_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/state.h>

namespace render
{

namespace low_level
{

namespace helpers
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������� � ������ (����������� ����������� �� �������� ����������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameterGroup
{
  size_t                  slot;       //������ ������������ ������
  size_t                  count;      //���������� ���������� � ������
  const ProgramParameter* parameters; //��������� �� ������ ������� � ����������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ProgramParametersLayout: virtual public IProgramParametersLayout
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramParametersLayout  (const ProgramParametersLayoutDesc&);
    ~ProgramParametersLayout ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  GetParametersCount () const;
    const ProgramParameter* GetParameters      () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t                       GetGroupsCount () const;
    const ProgramParameterGroup* GetGroups      () const;
    const ProgramParameterGroup& GetGroup       (size_t index);

  private:  
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif