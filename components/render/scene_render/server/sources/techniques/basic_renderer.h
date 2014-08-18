///Базовый рендер
class BasicRenderer: public xtl::reference_counter, public xtl::noncopyable
{
  public:
///Конструктор / деструктор
    BasicRenderer  (server::RenderManager& manager, const char* effect_name);
    ~BasicRenderer ();

///Доступ к полям
    server::RenderManager&  RenderManager   () { return manager; } 
    manager::Frame&         Frame           () { return frame; }
    common::PropertyMap&    FrameProperties () { return frame_properties; }

///Отрисовка
    void Draw (RenderingContext& parent_context);

  protected:
///Настройка кадра перед рендерингом
    virtual void ConfigureFrame (RenderingContext& parent_context);

///Настройка объекта
    virtual void ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, manager::EntityDrawParams& out_params);

///Отрисовка моделей
    void DrawVisualModels (RenderingContext& parent_context);

  private:
///Отрисовка массива отображаемых моделей
    void Draw (RenderingContext& context, size_t visual_models_count, VisualModel* const* models, void* user_data = 0) const;
    void Draw (RenderingContext& context, const stl::vector<VisualModel*>& models, void* user_data = 0) const;

  private:
///Обработчик отрисовки объектов
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params);
  
  private:
    server::RenderManager manager;                      //менеджер рендеринга
    manager::Frame        frame;                        //фрейм
    common::PropertyMap   frame_properties;             //свойства кадра
    common::PropertyMap   frame_entity_properties;      //свойства кадр-объект
    size_t                view_transaction_id;          //счетчик обновлений матрицы вида
    size_t                proj_transaction_id;          //счетчик обновлений матрицы проекции
    size_t                view_proj_transaction_id;     //счетчик обновлений матрицы вид-проекция
    math::mat4f           view_tm;                      //матрица вида
    math::mat4f           view_proj_tm;                 //матрица вид-проекция
    size_t                mv_matrix_property_index;     //индекс свойства матрицы ModelView
    size_t                mvp_matrix_property_index;    //индекс свойства матрицы ModelViewProjection
    size_t                object_matrix_property_index; //индекс свойства матрицы ObjectMatrix
    size_t                view_matrix_property_index;   //индекс свойства матрицы View (в свойствах кадра)
    size_t                proj_matrix_property_index;   //индекс свойства матрицы Projection (в свойства кадра)
};
