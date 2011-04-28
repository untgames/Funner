///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderingContext: public xtl::noncopyable
{
  public: 
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderingContext (FrameImpl& frame, RenderingContext* previous = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TexturePtr          FindLocalTexture (const char* name);
    RenderTargetDescPtr FindRenderTarget (const char* name);

  private:
    FrameImpl&        frame;    //����
    RenderingContext* previous; //���������� ��������
};
