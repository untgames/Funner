#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>

#include <common/parser.h>
#include <common/property_map.h>
#include <common/property_binding_map.h>
#include <common/strlib.h>

#include <shared/camera.h>
#include <shared/render_manager.h>
#include <shared/server.h>
#include <shared/technique_manager.h>

#include <shared/sg/visual_model.h>

#include <render/manager.h>

namespace render
{

namespace scene
{

namespace server
{

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
      common::PropertyMap frame_properties;             //свойства кадра
      common::PropertyMap frame_entity_properties;      //свойства кадр-объект
      size_t              view_transaction_id;          //счетчик обновлений матрицы вида
      size_t              proj_transaction_id;          //счетчик обновлений матрицы проекции
      size_t              view_proj_transaction_id;     //счетчик обновлений матрицы вид-проекция
      math::mat4f         view_tm;                      //матрица вида
      math::mat4f         view_proj_tm;                 //матрица вид-проекция
      size_t              mv_matrix_property_index;     //индекс свойства матрицы ModelView
      size_t              mvp_matrix_property_index;    //индекс свойства матрицы ModelViewProjection
      size_t              object_matrix_property_index; //индекс свойства матрицы ObjectMatrix
      size_t              view_matrix_property_index;   //индекс свойства матрицы View (в свойствах кадра)
      size_t              proj_matrix_property_index;   //индекс свойства матрицы Projection (в свойства кадра)
      manager::Frame      frame;                        //фрейм

      PrivateData (BasicTechnique& technique)
        : frame (technique.CreateFrame (*this))
        , view_transaction_id ()
        , proj_transaction_id ()
        , view_proj_transaction_id ()
        , view_tm (1.0f)
        , view_proj_tm (1.0f)
        , mv_matrix_property_index (~0u)
        , mvp_matrix_property_index (~0u)
        , object_matrix_property_index (~0u)
        , view_matrix_property_index (~0u)
        , proj_matrix_property_index (~0u)
      {
      }
    };

///Настройка кадра перед рендерингом
    virtual void ConfigureFrame (RenderingContext& parent_context, PrivateData& private_data);

///Настройка объекта
    virtual void ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, PrivateData& private_data, manager::EntityDrawParams& out_params);

///Отрисовка моделей
    void DrawVisualModels (RenderingContext& parent_context, PrivateData& private_data);

///Обновление кадра
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder);

///Обновление свойств
    void UpdatePropertiesCore ();

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap&);

  private:
/// Создание кадра
    manager::Frame CreateFrame (PrivateData& private_data);

///Обработчик отрисовки объектов
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params);
  
  private:
    RenderManager manager;     //менеджер рендеринга
    stl::string   effect_name; //имя эффекта
};

}

}

}
