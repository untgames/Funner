///////////////////////////////////////////////////////////////////////////////////////////////////
///Цель рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetImpl  (const DeviceManagerPtr& device_manager, low_level::ITexture* texture, unsigned int layer = 0, unsigned int mip_level = 0);
    ~RenderTargetImpl ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение отображения (в случае отсутствия - исключение)
///////////////////////////////////////////////////////////////////////////////////////////////////
    low_level::IView& View ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры цели рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  ();
    unsigned int Height ();
    void         Resize (unsigned int width, unsigned int height);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Смещение области вывода от начала координат цели рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetViewportOffset (const math::vec2ui& offset);
    const math::vec2ui& ViewportOffset    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetTarget (low_level::ITexture* texture, unsigned int layer = 0, unsigned int mip_level = 0);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Захват изображения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture (media::Image&);
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
