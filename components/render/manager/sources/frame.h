///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderTargetDesc: public xtl::reference_counter
{
  RenderTargetPtr render_target; //���� ���������
  RectAreaPtr     viewport;      //������� ������
  RectAreaPtr     scissor;       //������� ���������
  
  RenderTargetDesc (const RenderTargetPtr& in_render_target, const RectAreaPtr& in_viewport, const RectAreaPtr& in_scissor)
    : render_target (in_render_target)
    , viewport (in_viewport)
    , scissor (in_scissor)
  {
  }  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameImpl  (const DeviceManagerPtr&, const EffectManagerPtr&);
    ~FrameImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr     FindRenderTarget     (const char* name);
    RectAreaPtr         FindViewport         (const char* name);
    RectAreaPtr         FindScissor          (const char* name);
    RenderTargetDescPtr FindRenderTargetDesc (const char* name);
    RenderTargetPtr     RenderTarget         (const char* name);
    RectAreaPtr         Viewport             (const char* name);
    RectAreaPtr         Scissor              (const char* name);
    RenderTargetDescPtr RenderTargetDesc     (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetScissorState (bool state);
    bool ScissorState    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetClearFlags (size_t clear_flags);
    size_t ClearFlags    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetClearColor (const math::vec4f& color);
    const math::vec4f& ClearColor    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetClearDepthValue   (float depth_value);
    void          SetClearStencilIndex (unsigned char stencil_index);
    float         ClearDepthValue      ();
    unsigned char ClearStencilIndex    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLocalTexture        (const char* name, const TexturePtr& texture);
    void RemoveLocalTexture     (const char* name);
    void RemoveAllLocalTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TexturePtr FindLocalTexture (const char* name);
    TexturePtr LocalTexture     (const char* name);
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetEffect (const char* name);
    const char* Effect    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectRendererPtr EffectRenderer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����-����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetShaderDefines (const common::PropertyMap&);
    const common::PropertyMap& ShaderDefines    () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Frame::EntityDrawHandler EntityDrawHandler;

    size_t EntitiesCount     ();
    void   AddEntity         (const EntityPtr& entity);
    void   AddEntity         (const EntityPtr& entity, const common::PropertyMap& properties, const math::mat4f& mvp_matrix);
    void   AddEntity         (const EntityPtr& entity, const EntityDrawHandler& handler);
    void   RemoveAllEntities ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���-���������� � ����-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FramesCount     ();
    void   AddFrame        (const FramePtr& frame);
    void   RemoveAllFrames ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw (RenderingContext* previous = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
