///“ехника рендеринга карты теней
class ShadowMapRenderer: public BasicRenderer
{
  public:
/// онструктор / деструктор
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///ѕолучение теневой карты
    manager::Texture& ShadowMap () { return depth_map; }

///ѕолучение имени локальной текстуры
    const char* LocalTextureName () { return local_texture_name.c_str (); }

///ќбновление карты
    void UpdateShadowMap (RenderingContext& parent_context, Light& light);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  color_map;                //карта теней
    manager::Texture  depth_map;                //карта теней
    RenderTargetArray color_render_targets;     //цели рендеринга
    RenderTargetArray depth_render_targets;     //цели рендеринга
    TraverseResult    traverse_result;          //результат обхода
    stl::string       color_render_target_name; //им€ цели рендеринга
    stl::string       depth_render_target_name; //им€ цели рендеринга
    stl::string       local_texture_name;       //им€ локальной текстуры
};


