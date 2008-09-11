#ifndef RENDER_MID_LEVEL_LOW_LEVEL_RENDERER_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_RENDERER_HEADER

#include <render/mid_level/renderer.h>

namespace render
{

namespace low_level
{

//forward declarations
class IDevice;

}

namespace mid_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������, ���������� ����� render::low_level::IDevice (��� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ILowLevelRenderer: virtual public mid_level::IRenderer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual low_level::IDevice& GetDevice () = 0;
};

}

}

#endif
