///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализации цели отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetImpl: public xtl::noncopyable, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetImpl  (const ConnectionPtr& connection, const char* render_target_name, const char* init_string);
    ~RenderTargetImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Экран (политика владения - weak-ref)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetScreen (scene_graph::Screen* screen);
    scene_graph::Screen* Screen    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Захват изображения (screen-shot)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);
    void CaptureImage (const char* image_name);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

typedef xtl::intrusive_ptr<RenderTargetImpl> RenderTargetPtr;
