///Базовая техника рендеринга объектов
class BasicTechnique: public Technique
{
  public:
///Конструктор / деструктор
    BasicTechnique  (RenderManager& in_manager, const common::ParseNode& node);
    ~BasicTechnique ();

  protected:
///Данные для рендеринга техники в каждом кадре
    struct PrivateData
    {
      BasicRendererPtr renderer;

      PrivateData (BasicTechnique& technique) : renderer (new BasicRenderer (technique.manager, technique.effect_name.c_str ()), false) {}
    };

///Обновление кадра
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder);

///Обновление свойств
    void UpdatePropertiesCore ();

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap&);

  private:
    RenderManager manager;     //менеджер рендеринга
    stl::string   effect_name; //имя эффекта
};
