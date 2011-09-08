#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    Константы
*/

const size_t SUB_TECHNIQUE_ARRAY_RESERVE = 8; //резервируемое количество техник рендеринга

}

/*
    Описание реализации рендера
*/

typedef stl::vector<TechniquePtr> TechniqueArray;

struct View::Impl
{
  RenderPtr            render;                     //рендер
  RenderManager        manager;                    //менеджер рендеринга
  stl::string          technique_name;             //имя техники рендеринга
  TechniqueArray       sub_techniques;             //вложенные техники
  render::Frame        frame;                      //кадр
  scene_graph::Camera* camera;                     //текущая камера
  common::PropertyMap  properties;                 //свойства рендеринга
  ScenePtr             scene;                      //текущая сцена
  bool                 need_update_properties;     //необходимо обновить свойства рендеринга
  bool                 need_update_camera;         //необходимо обновить камеру
  bool                 need_update_sub_techniques; //необходимо обновить техники рендеринга
  Log                  log;                        //поток протоколирования

///Конструктор
  Impl (RenderManager& in_manager, const char* in_technique)
    : render (Render::GetRender (in_manager))
    , manager (in_manager)
    , technique_name (in_technique)
    , frame (in_manager.CreateFrame ())
    , camera ()
    , need_update_properties (true)
    , need_update_camera (true)
    , need_update_sub_techniques (true)
  {
    sub_techniques.reserve (SUB_TECHNIQUE_ARRAY_RESERVE);
    
    log.Printf ("View created for technique '%s'", technique_name.c_str ());
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("View destroyed for technique '%s'", technique_name.c_str ());
  }
  
///Обновление техник
  void UpdateSubTechniques ()
  {
    try
    {
      if (!need_update_sub_techniques)
        return;
        
        //загрузка конфигурации
        
      sub_techniques.clear ();
      
      for (common::Parser::NamesakeIterator iter=manager.Configuration ().First ("technique"); iter; ++iter)
      {
        const char* technique = common::get<const char*> (*iter, "", "");
        
        if (!xtl::xstrcmp (technique, technique_name.c_str ()))
          continue;
          
        common::ParseNode parent_technique_node = *iter;
          
        for (common::Parser::Iterator iter=parent_technique_node.First (); iter; ++iter)
        {
          try
          {
            TechniquePtr technique = TechniqueManager::CreateTechnique (iter->Name (), manager, *iter);
            
            if (!technique)
              throw xtl::format_operation_exception ("", "Can't create technique '%s'", iter->Name ());
            
            technique->SetName (iter->Name ());

            sub_techniques.push_back (technique);
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at create sub-technique '%s' for technique '%s'", e.what (), iter->Name (), technique_name.c_str ());
          }
          catch (...)
          {
            log.Printf ("unknown exception\n    at create sub-technique '%s' for technique '%s'", iter->Name (), technique_name.c_str ());
          }
        }
      }              
        
      need_update_sub_techniques = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene_render3d::View::Impl::UpdateSubTechniques");
      throw;
    }    
  }
};

/*
    Конструктор / деструктор
*/

View::View (RenderManager& manager, const char* technique)
{
  try
  {
    if (!technique)
      throw xtl::make_null_argument_exception ("", "technique");
    
    impl = new Impl (manager, technique);
    
    impl->render->RegisterView (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::View::View");
    throw;
  }
}

View::~View ()
{
  try
  {
    impl->render->UnregisterView (*this);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Кадр
*/

render::Frame& View::Frame ()
{
  return impl->frame;
}

/*
    Обновление камеры
*/

void View::UpdateCamera (scene_graph::Camera* camera)
{
  impl->camera             = camera;
  impl->need_update_camera = true;
}

/*
    Обновление свойств ренедринга
*/

void View::UpdateProperties (const common::PropertyMap& properties)
{
  impl->properties             = properties;
  impl->need_update_properties = true;
}

/*
    Обновление содержимого кадра
*/

void View::UpdateFrame ()
{
  try
  {
      //проверка необходимости обновления техник рендеринга
      
    if (impl->need_update_sub_techniques)
      impl->UpdateSubTechniques ();
    
      //проверка необходимости обновления свойств рендеринга
      
    if (impl->need_update_properties)
    {
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->UpdateProperties (impl->properties);
          
           ++iter;
           
           continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' properties for technique '%s'", e.what (), (*iter)->Name (), impl->technique_name.c_str ());
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' properties for technique '%s'", (*iter)->Name (), impl->technique_name.c_str ());
        }
        
        impl->sub_techniques.erase (iter);
        
        end = impl->sub_techniques.end ();
      }      

      impl->need_update_properties = false;
    }
    
      //проверка необходимости обновления камеры
      
    if (impl->need_update_camera)
    {
        //обновление сцены
        
      if (impl->camera && impl->camera->Scene ())
      {
        impl->scene = impl->render->GetScene (*impl->camera->Scene ());
      }
      else
      {
        impl->scene = ScenePtr ();
      }
        
        //оповещение техник
      
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->UpdateCamera (impl->camera);
          
          ++iter;
          
          continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' camera for technique '%s'", e.what (), (*iter)->Name (), impl->technique_name.c_str ());          
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' camera for technique '%s'", (*iter)->Name (), impl->technique_name.c_str ());
        }
                
        impl->sub_techniques.erase (iter);
        
        end = impl->sub_techniques.end ();        
      }

      impl->need_update_camera = false;
    }        
    
      //обновление кадра
      
    if (impl->scene)
    {
      for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->UpdateFrame (*impl->scene, impl->frame);
          
          ++iter;
          
          continue;
        }
        catch (std::exception& e)
        {
          impl->log.Printf ("%s\n    at update sub-technique '%s' frame for technique '%s'", e.what (), (*iter)->Name (), impl->technique_name.c_str ());          
        }
        catch (...)
        {
          impl->log.Printf ("unknown exception\n    at update sub-technique '%s' frame for technique '%s'", (*iter)->Name (), impl->technique_name.c_str ());
        }

        impl->sub_techniques.erase (iter);

        end = impl->sub_techniques.end ();
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render:scene_graph3d::View::UpdateFrame");
    throw;
  }
}

/*
    Подсчёт ссылок
*/

void View::AddRef ()
{
  addref (this);
}

void View::Release ()
{
  release (this);
}

/*
    Имя техники рендеринга
*/

const char* View::TechniqueName ()
{
  return impl->technique_name.c_str ();
}

/*
    Управление техниками рендеринга
*/

void View::ResetTechniqueCache ()
{
  impl->sub_techniques.clear ();
  
  impl->need_update_sub_techniques = true;
}
