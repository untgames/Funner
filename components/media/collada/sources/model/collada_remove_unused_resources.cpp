#include "shared.h"

using namespace common;

namespace media
{

namespace collada
{

namespace
{

/*
    Утилита выбора элементов из библиотек
*/

class ItemsSelector
{
  public:  
///Конструктор
    ItemsSelector (Model& model, size_t in_select_flags, const char* in_force_select)
      : source_model (model)
      , select_flags (in_select_flags)
    {
      try
      {
          //выделение строк выборка элементов
        
        force_select = common::split (in_force_select);

          //создание новой модели

        target_model.Rename (source_model.Name ());
        target_model.SetActiveSceneName (source_model.ActiveSceneName ());
        target_model.SetUnitOfMeasure (source_model.UnitOfMeasure ());

          //выбор объектов, входящих в активную сцену

        if (select_flags & ModelSelect_ActiveScene)
          SelectActiveScene ();
        
          //выбор сцен
        
        SelectScenes ();
        
          //выбор библиотек

        SelectLibrary (source_model.Nodes (), ModelSelect_Nodes, "nodes");
        SelectLibrary (source_model.Cameras (), ModelSelect_Cameras, "cameras");
        SelectLibrary (source_model.Lights (), ModelSelect_Lights, "lights");
        SelectLibrary (source_model.Skins (), ModelSelect_Skins, "skins");
        SelectLibrary (source_model.Morphs (), ModelSelect_Morphs, "morphs");
        SelectLibrary (source_model.Meshes (), ModelSelect_Meshes, "meshes");
        SelectLibrary (source_model.Images (), ModelSelect_Images, "images");
        SelectLibrary (source_model.Effects (), ModelSelect_Effects, "effects");
        SelectLibrary (source_model.Materials (), ModelSelect_Materials, "materials");

        //выбор анимаций

        if (select_flags & ModelSelect_Animations)
          for (AnimationList::Iterator iter = const_cast <AnimationList&> (source_model.Animations ()).CreateIterator (); iter; ++iter)
            target_model.Animations ().Add (*iter);

        //обмен моделей

        model.Swap (target_model);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::ItemsSelector() where model_name='%s'", source_model.Name ());
        throw;
      }
    }
    
  private:
///Проверка необходимости выбора
    bool IsNeedSelect (const char* id, size_t flags)
    {
      return (select_flags & flags) == flags || IsForceSelect (id);
    }
    
///Проверка наличия ресурса в списке обязательно выбираемых
    bool IsForceSelect (const char* id)
    {
      const char** items = force_select.Data ();
      
      for (size_t i=0, count=force_select.Size (); i<count; i++)
        if (common::wcmatch (id, items [i]))
          return true;

      return false;
    }
    
///Вставка ресурса
    template <class Item>
    void Insert (ILibrary<Item>& library, const char* id, const Item& item)
    {
      if (library.Find (id))
        return;
        
      library.Insert (id, const_cast<Item&> (item));
    }
    
    template <class Item>
    void Insert (ILibrary<Item>& library, const Item& item)
    {
      Insert (library, item.Id (), item);
    }        

///Выбор изображения
    void Select (const Image& image)
    {
      Insert (target_model.Images (), image);
    }

///Выбор текстуры
    void Select (const Texture& texture)
    {
      if (!*texture.Image ())
        return;
      
      const Image* image = source_model.Images ().Find (texture.Image ());
        
      if (!image)
        throw xtl::format_operation_exception ("media::collada::ItemsSelector::Select(const Texture&)", "Image '%s' not found", texture.Image ());
          
      Select (*image);
    }

///Выбор эффекта
    void Select (const Effect& effect)
    {
      for (int i=0; i<TextureMap_Num; i++)
      {
        TextureMap map = (TextureMap)i;
        
        if (!effect.HasTexture (map))
          continue;      

        try
        {
          Select (effect.Texture (map));
        }
        catch (xtl::exception& e)
        {
          e.touch ("media::collada::ItemsSelector::Select(const Effect&) where effect_id='%s' texture_map=%d", effect.Id (), map);
          throw;
        }
      }

      Insert (target_model.Effects (), effect);
    }

///Выбор материала
    void Select (const Material& material)
    {
      try
      {
        const Effect* effect = source_model.Effects ().Find (material.Effect ());

        if (!effect)
          throw xtl::format_operation_exception ("", "Effect '%s' for material '%s' not found", material.Effect (), material.Id ());
          
        Select (*effect);

        Insert (target_model.Materials (), material);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const Material&) where material_id='%s'", material.Id ());
        throw;
      }
    }

///Выбор источника света
    void Select (const Light& light)
    {
      Insert (target_model.Lights (), light);
    }

///Выбор камеры
    void Select (const Camera& camera)
    {
      Insert (target_model.Cameras (), camera);
    }
   
///Выбор меша
    void Select (const Mesh& mesh)
    {
      Insert (target_model.Meshes (), mesh);
    }
    
///Выбор меша по имени
    void SelectMeshByName (const char* mesh_name, const MaterialBinds* material_binds = 0)
    {
      try
      {
        const Mesh* mesh = source_model.Meshes ().Find (mesh_name);
        
        if (!mesh)
          throw xtl::format_operation_exception ("", "Mesh not '%s' found", mesh_name);
          
        if (material_binds) Select (*mesh, *material_binds);
        else                Select (*mesh);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::SelectMeshByName(const char*,const MaterialBinds*) where mesh_id='%s'", mesh_name);
        throw;
      }
    }
    
///Выбор морфа
    void Select (const Morph& morph, const MaterialBinds* material_binds = 0)
    {
      try
      {
        SelectMeshByName (morph.BaseMesh (), material_binds);

        for (Morph::TargetList::ConstIterator iter=morph.Targets ().CreateIterator (); iter; ++iter)
          SelectMeshByName (iter->Mesh (), material_binds);

        Insert (target_model.Morphs (), morph);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada:ItemsSelector::Select(const Morph&,const MaterialBinds*) where morph_id='%s'", morph.Id ());
        throw;
      }
    }

///Выбор скина
    void Select (const Skin& skin, const MaterialBinds* material_binds = 0, const InstanceController* controller = 0)
    {
      try
      {
        SelectMeshByName (skin.BaseMesh (), material_binds);

          //выбор узлов для соединений
          
        if (controller)
        {        
          for (size_t joint_index=0, joints_count=skin.JointsCount (); joint_index<joints_count; joint_index++)
          {
            const char* joint_name = skin.JointName (joint_index);
            const Node* joint      = 0;
            
            for (size_t search_root_index=0, search_roots_count=controller->JointSearchRootsCount (); !joint && search_root_index<search_roots_count; search_root_index++)
            {
              const char* search_root_node_name = controller->JointSearchRoot (search_root_index);              
              const Node* search_root           = source_model.Nodes ().Find (search_root_node_name);
              
              if (!search_root)
                throw xtl::format_operation_exception ("", "Search root '%s' not found", search_root_node_name);

              joint = search_root->FindChild (joint_name);
            }

            if (!joint)
              throw xtl::format_operation_exception ("", "Joint node '%s' not found", joint_name);

            Select (*joint);
          }
        }

        Insert (target_model.Skins (), skin);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const Skin&,const MaterialBinds*,const InstanceController*) where skin_id='%s'", skin.Id ());
        throw;
      }
    }

///Выбор инстанцированных материалов
    void Select (const Surface& surface, const MaterialBinds& binds)
    {
      const char* material_name = binds.FindMaterial (surface);
      
      const Material* material = source_model.Materials ().Find (material_name);

      if (!material_name || !material)
        throw xtl::format_operation_exception ("media::collada::ItemsSelector::Select(const Surface&,const MaterialBinds&)", "Instance material for surface with material '%s' not found", surface.Material ());
        
      Select (*material);
    }    
    
///Выбор инстанцированного меша
    void Select (const Mesh& mesh, const MaterialBinds& material_binds)
    {
      try
      {
        Select (mesh);
        
        for (Mesh::SurfaceList::ConstIterator iter=mesh.Surfaces ().CreateIterator (); iter; ++iter)  
          Select (*iter, material_binds);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const Mesh&,const MaterialBinds&) where mesh_id='%s'", mesh.Id ());
        throw;
      }
    }    

///Выбор инстанцированного меша
    void Select (const InstanceMesh& instance_mesh)
    {
      try
      {
        SelectMeshByName (instance_mesh.Mesh (), &instance_mesh.MaterialBinds ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const InstanceMesh&)");
        throw;
      }
    }    
    
///Выбор контроллера
    void Select (const InstanceController& instance_controller)
    {
      try
      {
          //выбор контроллера
        
        const Skin* skin = source_model.Skins ().Find (instance_controller.Controller ());
        
        if (skin)
        {
          Select (*skin, &instance_controller.MaterialBinds (), &instance_controller);
        }
        
        const Morph* morph = source_model.Morphs ().Find (instance_controller.Controller ());
        
        if (morph)
        {
          Select (*morph, &instance_controller.MaterialBinds ());
        }
        
        if (!skin && !morph)
          throw xtl::format_operation_exception ("", "Controller '%s' not found", instance_controller.Controller ());
          
          //выбор меша
          
        const char* base_mesh_name = instance_controller.FindBaseMesh (source_model);
          
        if (base_mesh_name)
        {          
          SelectMeshByName (base_mesh_name, &instance_controller.MaterialBinds ());
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada:ItemsSelector::Select(const InstanceController&)");
        throw;
      }      
    }
    
///Выбор коллекции
    template <class Item>
    void Select (const ICollection<Item>& collection, const char* collection_name)
    {
      try
      {
        for (typename ICollection<Item>::ConstIterator iter=collection.CreateIterator (); iter; ++iter)
          Select (*iter);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const ICollection<Item>&,const char*) where collection_name='%s'", collection_name);
        throw;
      }
    }    

///Выбор объектов, входящих в узел
    void Select (const Node& node)
    {
      try
      {
        Select (node.Lights (), "lights");
        Select (node.Cameras (), "cameras");
        Select (node.Meshes (), "instance_meshes");
        Select (node.Controllers (), "instance_controllers");
        Select (node.Nodes (), "nodes");

        Insert (target_model.Nodes (), node);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const Node&) where node_id='%s'", node.Id ());
        throw;
      }
    }

///Выбор объектов, входящих в сцену
    void SelectScene (const char* id, const Node& scene)
    {
      try
      {
        Select (scene);

        Insert (target_model.Scenes (), id, scene);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::Select(const char*,const Node&) where scene_id='%s'", id);
        throw;
      }
    }

///Выбор объектов, входящих в активную сцену
    void SelectActiveScene ()
    {
      try
      {
        const Node* root = source_model.Scenes ().Find (source_model.ActiveSceneName ());
      
        if (!root)
          throw xtl::format_operation_exception ("", "Scene '%s' not found", source_model.ActiveSceneName ());

        SelectScene (source_model.ActiveSceneName (), *root);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::SelectActiveScene");
        throw;
      }
    }

///Выбор используемых сцен
    void SelectScenes ()
    {
      try
      {
        for (NodeLibrary::ConstIterator iter=source_model.Scenes ().CreateIterator (); iter; ++iter)
        {
          const char* id = source_model.Scenes ().ItemId (iter);
          
          if (!IsNeedSelect (id, ModelSelect_Scenes))
            continue;

          SelectScene (id, *iter);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::ItemsSelector::SelectScenes");
        throw;
      }
    }

///Выбор используемых элементов библиотеки
    template <class Item>
    void SelectLibrary (const ILibrary<Item>& library, size_t flags, const char* library_type)
    {
      try
      {
        for (typename ILibrary<Item>::ConstIterator iter=library.CreateIterator (); iter; ++iter)
        {
          if (!IsNeedSelect (library.ItemId (iter), flags))
            continue;

          Select (*iter);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::collada::SelectLibrary(ILibrary<Item>&,size_t,const char*) where library_type='%s'", library_type);
        throw;
      }
    }    
  
  private:
    const Model& source_model; //исходная модель
    Model        target_model; //результирующая модель
    size_t       select_flags; //флаги выбора
    StringArray  force_select; //список масок идентификаторов ресурсов, обязательно выбираемых
};

}

/*
    Выбор только используемых ресурсов
*/

void remove_unused_resources (Model& model, size_t select_flags, const char* force_select)
{
  try
  {
    if (!force_select)
      throw xtl::make_null_argument_exception ("", "force_select");      
      
      //селектор
      
    ItemsSelector (model, select_flags, force_select);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::collada::remove_unused_resources");
    throw;
  }
}

}

}
