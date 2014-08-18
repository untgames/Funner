///Техника рендеринга карты теней
class ShadowMapRenderer: public BasicRenderer
{
  public:
///Конструктор / деструктор
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///Получение теневой карты
    manager::Texture& ShadowMap () { return shadow_map; }

///Получение имени локальной текстуры
    const char* LocalTextureName () { return local_texture_name.c_str (); }

///Обновление карты
    void UpdateShadowMap (RenderingContext& parent_context, Light& light);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  shadow_map;         //карта теней
    RenderTargetArray render_targets;     //цели рендеринга
    TraverseResult    traverse_result;    //результат обхода
    stl::string       render_target_name; //имя цели рендеринга
    stl::string       local_texture_name; //имя локальной текстуры
};


