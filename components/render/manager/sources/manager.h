///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManagerImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderManagerImpl  ();
    ~RenderManagerImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowPtr CreateWindow (syslib::Window& window, common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    WindowsCount ();
    WindowPtr Window       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr CreateRenderBuffer       (size_t width, size_t height, PixelFormat format);
    RenderTargetPtr CreateDepthStencilBuffer (size_t width, size_t height);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::TextureManager&   TextureManager   ();
    render::PrimitiveManager& PrimitiveManager ();
    render::MaterialManager&  MaterialManager  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EntityPtr CreateEntity ();
    FramePtr  CreateFrame  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* resource_name);
    void LoadResource   (const media::rfx::MaterialLibrary&);
    void LoadResource   (const media::geometry::MeshLibrary&);
    void UnloadResource (const char* resource_name);
    void UnloadResource (const media::rfx::MaterialLibrary&);
    void UnloadResource (const media::geometry::MeshLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef RenderManager::WindowEventHandler WindowEventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
