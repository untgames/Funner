///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManagerImpl: public Object, public xtl::trackable
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
    WindowPtr CreateWindow (INativeWindow* window, common::PropertyMap& properties);

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
    render::DeviceManager&    DeviceManager    ();
    render::CacheManager&     CacheManager     ();
    render::TextureManager&   TextureManager   ();
    render::PrimitiveManager& PrimitiveManager ();
    render::ProgramManager&   ProgramManager   ();
    render::MaterialManager&  MaterialManager  ();
    render::EffectManager&    EffectManager    ();

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
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef RenderManager::WindowEventHandler WindowEventHandler;
    typedef RenderManager::EventHandler EventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;
    xtl::connection RegisterEventHandler       (RenderManagerEvent event, const EventHandler& handler) const;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::Settings& Settings ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
