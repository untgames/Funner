///Техника рендеринга карты теней
class ShadowMapBuilder: public BasicTechnique
{
  public:
///Конструктор / деструктор
    ShadowMapBuilder  (RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapBuilder ();

///Получение теневой карты
    manager::Texture ShadowMap () const;

  private:
    manager::Texture shadow_map; //карта теней
};


