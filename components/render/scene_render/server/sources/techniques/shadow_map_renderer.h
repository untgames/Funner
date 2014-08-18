///Техника рендеринга карты теней
class ShadowMapRenderer: public BasicRenderer
{
  public:
///Конструктор / деструктор
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///Получение теневой карты
    manager::Texture& ShadowMap () { return shadow_map; }

///Обновление карты
    void UpdateShadowMap (RenderingContext& parent_context, Light& light, ITraverseResultCache& traverse_result_cache);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  shadow_map;         //карта теней
    RenderTargetArray render_targets;     //цели рендеринга
    stl::string       render_target_name; //имя цели рендеринга
};


