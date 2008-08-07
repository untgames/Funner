#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_BASIC_RENDERER_SHARED_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_BASIC_RENDERER_SHARED_HEADER

#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/shared_ptr.h>

#include <common/singleton.h>

#include <media/image.h>

#include <render/low_level/utils.h>

#include <render/mid_level/low_level_driver.h>

#include <shared/basic_renderer.h>
#include <shared/renderer2d.h>

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClearFrame: virtual public IClearFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClearFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFlags (size_t clear_flags);
    size_t GetFlags () { return clear_flags; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetColor (const math::vec4f& color);
    void GetColor (math::vec4f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetDepthValue   (float depth_value);
    void          SetStencilIndex (unsigned char stencil_index);
    float         GetDepthValue   () { return clear_depth_value; }
    unsigned char GetStencilIndex () { return clear_stencil_index; }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (render::low_level::IDevice*);

  private:
    size_t                     clear_flags;
    render::low_level::Color4f clear_color;
    float                      clear_depth_value;
    unsigned char              clear_stencil_index;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetRenderersCount ();    
    const char* GetRendererName   (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderer* CreateRenderer (const char* name);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRenderer       (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);
    void UnregisterRenderer     (const char* name);
    void UnregisterAllRenderers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}

  private:
    void UnregisterDriver ();

  private:
    struct RendererEntry;

    typedef xtl::shared_ptr<RendererEntry> RendererEntryPtr;
    typedef stl::vector<RendererEntryPtr>  RendererEntries;

  private:
    RendererEntries renderer_entries;
};

}

}

}

#endif

