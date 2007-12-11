#ifndef RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER
#define RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER

#include <render/low_level/driver.h>
#include <gl/glew.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� GLEW (��� ������� ������� ������ �� ���������� GLEW)
///////////////////////////////////////////////////////////////////////////////////////////////////
const GLEWContext* glewGetContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������� ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ContextDataTable
{
  ContextDataTable_InputStage,
  ContextDataTable_ShaderStage,
  ContextDataTable_RasterizerStage,
  ContextDataTable_OutputStage,
  
  ContextDataTable_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ������ �� ������ ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t CONTEXT_DATA_TABLE_SIZE = 64;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� OpenGL (�������� ����������� - ������� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextManager
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextManager  (const LogHandler& log_handler);
    ContextManager  (const ContextManager&);
    ~ContextManager ();

    ContextManager& operator = (const ContextManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetSwapChains    (ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain);
    ISwapChain* GetDrawSwapChain () const;
    ISwapChain* GetReadSwapChain () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeContextCurrent () const;
    bool IsContextCurrent   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetContextData   (ContextDataTable table_id, size_t element_id, size_t value);
    size_t GetContextData   (ContextDataTable table_id, size_t element_id) const;
    void   ClearContextData (ContextDataTable table_id);
    void   ClearContextData ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �� ������� ����������, ��������� �� �������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetSwapChainsExtensionString () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (const ContextManager&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogPrintf  (const char* format, ...) const;
    void LogVPrintf (const char* format, va_list args) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckErrors (const char* source) const;
    void RaiseError  (const char* source) const;

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
