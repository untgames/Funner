///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст цели отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderTargetDesc: public xtl::reference_counter
{
  RenderTargetPtr render_target; //цель отрисовки
  ViewportPtr     viewport;      //область вывода
  RectAreaPtr     scissor;       //область отсечения
  
  RenderTargetDesc (const RenderTargetPtr& in_render_target, const ViewportPtr& in_viewport, const RectAreaPtr& in_scissor)
    : render_target (in_render_target)
    , viewport (in_viewport)
    , scissor (in_scissor)
  {
  }  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Карта целей рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetMapImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetMapImpl  ();
    ~RenderTargetMapImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация целевых буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport);
    void SetRenderTarget        (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport, const RectAreaPtr& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение целевых буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr     FindRenderTarget     (const char* name);
    ViewportPtr         FindViewport         (const char* name);
    RectAreaPtr         FindScissor          (const char* name);
    RenderTargetDescPtr FindRenderTargetDesc (const char* name);
    RenderTargetPtr     RenderTarget         (const char* name);
    ViewportPtr         Viewport             (const char* name);
    RectAreaPtr         Scissor              (const char* name);
    RenderTargetDescPtr RenderTargetDesc     (const char* name);    
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
