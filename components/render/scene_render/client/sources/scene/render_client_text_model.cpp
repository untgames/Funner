#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Текстовая модель
class TextModel: public VisualModel
{
  public:
///Конструктор
    TextModel (scene_graph::TextModel& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_SpriteList)
      , on_update_fonts_connection (model.RegisterEventHandler (scene_graph::TextModelEvent_AfterFontsUpdate, xtl::bind (&TextModel::UpdateFontsNotify, this)))
      , on_update_descs_connection (model.RegisterEventHandler (scene_graph::TextModelEvent_AfterCharDescsUpdate, xtl::bind (&TextModel::UpdateDescsNotify, this)))
      , need_update_fonts (true)
      , need_update_descs (true)
      , material_name_hash ()
    {
    }

///Исходный узел
    scene_graph::TextModel& SourceNode () const { return static_cast<scene_graph::TextModel&> (Node::SourceNode ()); }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::TextModel::UpdateCore");
        throw;
      }
    }

  private:
    void UpdateDescsNotify () { need_update_descs = true; }

    void UpdateFontsNotify ()
    {
      need_update_fonts = true;
    }

  private:
    xtl::auto_connection on_update_fonts_connection;
    xtl::auto_connection on_update_descs_connection;
    bool                 need_update_fonts;
    bool                 need_update_descs;
    size_t               material_name_hash;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::TextModel& model, SceneManager& scene_manager)
{
  return new TextModel (model, scene_manager);
}

}}}
