#include "shared.h"

#include <shared/sg/collection_visitor.h>

using namespace render;
using namespace render::scene::server;

//TODO: корректирующая матрица для отрисовки плоских зеркал
//TODO: хранение фрастума и использование внутреннего фрастума области вывода в случае наличия корректирующей матрицы

/*
===================================================================================================
    Common structures
===================================================================================================
*/

namespace
{

struct ViewportDrawListNode;

/// ViewportDrawList head
struct ViewportDrawListHead: public xtl::reference_counter
{
  ViewportDrawListNode* first;
  ViewportDrawListNode* last;

  ViewportDrawListHead () : first (), last () {}
};

typedef xtl::intrusive_ptr<ViewportDrawListHead> ViewportDrawListHeadPtr;

/// ViewportDrawList visible part
struct ViewportDrawListNode
{
  ViewportDrawListHeadPtr head;
  ViewportDrawListNode*   prev;
  ViewportDrawListNode*   next;
  bool                    is_added_to_list;

  ViewportDrawListNode (const ViewportDrawListHeadPtr& in_head)
    : head (in_head)
    , prev ()
    , next ()
    , is_added_to_list ()
  {
  }

  ~ViewportDrawListNode ()
  {
    RemoveFromDrawingList ();
  }

  void AddToDrawingList ()
  {
    if (is_added_to_list)
      return;

    next = 0;
    prev = head->last;

    head->last = this;

    if (prev) prev->next  = this;
    else      head->first = this;

    is_added_to_list = true;
  }

  void RemoveFromDrawingList ()
  {
    if (!is_added_to_list)
      return;

    if (prev) prev->next  = next;
    else      head->first = next;

    if (next) next->prev = prev;
    else      head->last = prev;

    next = prev = 0;

    is_added_to_list = false;
  }

  virtual void Cleanup () = 0;
};

/// Кэш результата обхода сцены 
struct CameraTraverseResult: public ITraverseResultCache
{
  const bound_volumes::plane_listf& frustum;  //пирамида отсечения
  Scene&                            scene;    //сцена
  TraverseResult&                   result;   //результат обхода
  bool                              computed; //результат рассчитан
  
///Конструктор
  CameraTraverseResult (const bound_volumes::plane_listf& in_frustum, Scene& in_scene, TraverseResult& in_result)
    : frustum (in_frustum)
    , scene (in_scene)
    , result (in_result)
    , computed (false) { }
  
///Получение результата
  TraverseResult& Result ()
  {
    try
    {
      if (computed)
        return result;
        
      scene.Traverse (frustum, result, Collect_All);
       
      computed = true;
      
      return result;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::CameraTraverseResult::Result");
      throw;
    }
  }
};

}

/*
===================================================================================================
    ViewportDrawList
===================================================================================================
*/

struct ViewportDrawList::Impl: public ViewportDrawListHead
{
};

/*
===================================================================================================
    Viewport
===================================================================================================
*/

/*
    Константы
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE                = 4;    //резервируемое количество слушателей
const size_t TECNIQUE_ARRAY_RESERVE_SIZE                = 16;   //резервируемое количество техник
const size_t FRAME_ARRAY_RESERVE_SIZE                   = 4;    //резервируемое количество кадров
const size_t TRAVERSE_RESULT_LIGHTS_RESERVE_SIZE        = 64;   //резервируемое количество источников света
const size_t TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE = 1024; //резервируемое количество визуализируемых моделей

/*
    Описание реализации области вывода
*/

namespace
{

struct RenderTargetEntry
{
  RenderTargetDesc desc;
  bool             need_update;

  RenderTargetEntry (const RenderTargetDesc& in_desc) : desc (in_desc), need_update (true) {}
};

typedef stl::hash_map<const RenderTargetDesc*, RenderTargetEntry> RenderTargetEntryMap;
typedef stl::vector<IViewportListener*>                           ListenerArray;
typedef stl::vector<TechniquePtr>                                 TechniqueArray;
typedef stl::vector<TechniquePrivateData>                         TechniquePrivateDataArray;

struct Frame: public xtl::reference_counter
{
  manager::Frame            frame;
  TechniquePrivateDataArray private_data;

  Frame (const manager::Frame& in_frame, size_t techniques_count)
    : frame (in_frame)
  {
    private_data.resize (techniques_count);
  }
};

typedef xtl::intrusive_ptr<Frame> FramePtr;
typedef stl::vector<FramePtr>     FrameArray;

}

struct Viewport::Impl: public xtl::reference_counter, public ViewportDrawListNode, public IRenderTargetMapListener, public IRenderManagerListener
{
  RenderManager                render_manager;             //менеджер рендеринга
  RenderTargetMap              render_targets;             //цели рендеринга
  manager::RenderTargetMap     frame_render_targets;       //цели рендеринга фреймов
  RenderTargetEntryMap         render_target_entries;      //дескрипторы целей рендеринга
  stl::string                  name;                       //имя области вывода
  stl::string                  technique_name;             //имя техники
  TechniqueArray               sub_techniques;             //техники
  Rect                         area;                       //область вывода
  float                        min_depth;                  //минимальная глубина
  float                        max_depth;                  //максимальная глубина
  int                          zorder;                     //порядок вывода
  bool                         is_active;                  //активна ли область отрисовки
  FrameArray                   frames;                     //присоединенные кадры
  size_t                       current_frame_index;        //индекс текущего кадра
  math::vec4f                  background_color;           //цвет фона
  bool                         background_state;           //состояние фона
  common::PropertyMap          properties;                 //свойства области вывода
  server::Camera               camera;                     //камера
  stl::auto_ptr<server::Scene> scene;                      //текущая сцена
  size_t                       max_draw_depth;             //максимальная глубина вложенности рендеринга
  ListenerArray                listeners;                  //слушатели событий области вывода
  bool                         need_reconfiguration;       //конфигурация изменена
  bool                         need_update_technique;      //обновление техники
  bool                         need_update_area;           //обновилась область
  bool                         need_update_render_targets; //требуется обновить буферы отрисовки
  bool                         need_update_background;     //требуется обновить параметры очистки
  bool                         need_update_camera;         //требуется обновить камеру
  bool                         need_update_properties;     //требуется обновление свойств

/// Конструктор
  Impl (const RenderManager& in_render_manager, const RenderTargetMap& in_render_targets, const ViewportDrawList& viewports, size_t in_max_draw_depth)
    : ViewportDrawListNode (viewports.impl)
    , render_manager (in_render_manager)
    , render_targets (in_render_targets)
    , area (0, 0, 100, 100)
    , min_depth (0.0f)
    , max_depth (1.0f)
    , zorder (0)
    , is_active (true)
    , current_frame_index (0)
    , background_state (false)
    , max_draw_depth (in_max_draw_depth)
    , need_reconfiguration (true)
    , need_update_area (true)
    , need_update_render_targets (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
  {
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
    sub_techniques.reserve (TECNIQUE_ARRAY_RESERVE_SIZE);
    frames.reserve (FRAME_ARRAY_RESERVE_SIZE);

    render_targets.AttachListener (this);

    try
    {
      render_manager.AttachListener (this);
    }
    catch (...)
    {
      render_targets.DetachListener (this);
      throw;
    }
  }

/// Деструктор
  ~Impl ()
  {
    sub_techniques.clear ();

    render_manager.DetachListener (this);
    render_targets.DetachListener (this);
  }

/// Получение текущего кадра
  Frame* CurrentFrame ()
  {
    try
    {
      if (current_frame_index >= max_draw_depth)
        return 0;

      if (current_frame_index < frames.size ())
        return &*frames [current_frame_index];

      FramePtr frame_desc (new Frame (render_manager.Manager ().CreateFrame (), sub_techniques.size ()), false);

      frame_desc->frame.SetRenderTargets (frame_render_targets);

      ConfigureBackground (frame_desc->frame);

      frames.push_back (frame_desc);

      return &*frame_desc;      
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::CurrentFrame");
      throw;
    }
  }

/// Оповещение о добавлении новой области вывода
  void OnRenderTargetAdded (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter != render_target_entries.end ())
      return;

    render_target_entries.insert_pair (&desc, RenderTargetEntry (desc));

    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// Оповещение об изменении области вывода
  void OnRenderTargetUpdated (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter == render_target_entries.end ())
      return;

    iter->second.need_update   = true;    
    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// Оповещение об удалении области вывода
  void OnRenderTargetRemoved (const RenderTargetDesc& desc)
  {
    render_target_entries.erase (&desc);

    need_update_render_targets = true;
    need_reconfiguration       = true;    
  }

/// Оповещение об изменении конфигурации
  void OnRenderManagerConfigurationChanged (const common::ParseNode&)
  {
    sub_techniques.clear ();

    need_update_technique = true;
    need_reconfiguration  = true;
  }

/// Создание техник
  void CreateSubTechniques ()
  {
    try
    {
      common::Log&      log           = render_manager.Log ();
      common::ParseNode configuration = render_manager.Manager ().Configuration ();

      for (common::Parser::NamesakeIterator iter=configuration.First ("technique"); iter; ++iter)
      {
        const char* technique = common::get<const char*> (*iter, "", "");        
        
        if (xtl::xstrcmp (technique, technique_name.c_str ()))
          continue;

        common::ParseNode parent_technique_node = *iter;

        for (common::Parser::Iterator iter=parent_technique_node.First (); iter; ++iter)
        {
          try
          {
            TechniquePtr technique = TechniqueManager::CreateTechnique (iter->Name (), render_manager, *iter);
            
            if (!technique)
              throw xtl::format_operation_exception ("", "Can't create technique '%s'", iter->Name ());
            
            technique->SetName (iter->Name ());

            sub_techniques.push_back (technique);
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at create sub-technique '%s' for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::CreateSubTechniques",
              e.what (), iter->Name (), technique_name.c_str (), name.c_str ());
          }
          catch (...)
          {
            log.Printf ("unknown exception\n    at create sub-technique '%s' for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::CreateSubTechniques",
              iter->Name (), technique_name.c_str (), name.c_str ());
          }
        }

        frames.clear ();

        return;
      }

      throw xtl::format_operation_exception ("", "Technique '%s' has not been found in the render manager configuration file '%s'", technique_name.c_str (), common::get<const char*> (configuration, "", ""));
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::CreateSubTechniques(technique='%s')", technique_name.c_str ());
      throw;
    }
  }

/// Переконфигурация техники
  void ReconfigureTechnique ()
  {
    try
    {
      render_manager.Log ().Printf ("Initializing technique '%s' for viewport '%s'", technique_name.c_str (), name.c_str ());

      CreateSubTechniques ();

      need_update_properties = true;
      need_update_technique  = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureTechnique");
      throw;
    }
  }

/// Конфигурация параметров очистки
  void ConfigureBackground (manager::Frame& frame)
  {
    if (background_state)
    {
      frame.SetClearColor (background_color);
      frame.SetClearFlags (manager::ClearFlag_All | manager::ClearFlag_ViewportOnly);
    }
    else
    {
      frame.SetClearFlags (0u);
    }
  }

/// Переконфигурация параметров очистки
  void ReconfigureBackground ()
  {
    try
    {
      for (FrameArray::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
        ConfigureBackground ((*iter)->frame);

      need_update_background = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureBackground");
      throw;
    }
  }

/// Переконфигурация целевых буферов отрисовки
  void ReconfigureRenderTargets ()
  {
    try
    {
      const Rect& viewport_area = area;
                              
      for (RenderTargetEntryMap::iterator iter=render_target_entries.begin (), end=render_target_entries.end (); iter!=end; ++iter)
      {
        RenderTargetEntry& entry = iter->second;

        if (!need_update_area && !entry.need_update)
          continue;

        const Rect& screen_area = entry.desc.Area ();

        const manager::RenderTarget& target = entry.desc.Target ();

        const size_t target_width = target.Width (), target_height = target.Height ();

        const double x_scale = screen_area.width ? double (target_width) / screen_area.width : 0.0,
                     y_scale = screen_area.height ? double (target_height) / screen_area.height : 0.0;

        Rect target_rect (int ((viewport_area.x - screen_area.x) * x_scale),
                          int ((viewport_area.y - screen_area.y) * y_scale),
                          size_t (ceil (viewport_area.width * x_scale)),
                          size_t (ceil (viewport_area.height * y_scale)));

        frame_render_targets.SetRenderTarget (entry.desc.Name (), target, manager::Viewport (target_rect, min_depth, max_depth));

        entry.need_update = false;
      }
      
      need_update_render_targets = false;
      need_update_area           = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureRenderTargets");
      throw;
    }
  }

/// Переконфигурация свойств
  void ReconfigureProperties ()
  {
    try
    {
      for (TechniqueArray::iterator iter=sub_techniques.begin (), end=sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->SetProperties (properties);
          
           ++iter;
           
           continue;
        }
        catch (std::exception& e)
        {
          render_manager.Log ().Printf ("%s\n    at update sub-technique '%s' properties for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::ReconfigureProperties", e.what (), (*iter)->Name (), technique_name.c_str (), name.c_str ());
        }
        catch (...)
        {
          render_manager.Log ().Printf ("unknown exception\n    at update sub-technique '%s' properties for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::ReconfigureProperties", (*iter)->Name (), technique_name.c_str (), name.c_str ());
        }
        
        sub_techniques.erase (iter);
        
        end = sub_techniques.end ();
      }            

      need_update_properties = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureProperties");
      throw;
    }
  }

/// Переконфигурация
  void Reconfigure ()
  {
    try
    {
      if (!need_reconfiguration)
        return;

        //переконфигурация техники

      if (need_update_technique)
        ReconfigureTechnique ();

        //переконфигурация параметров очистки
        
      if (need_update_background)
        ReconfigureBackground ();

        //переконфигурация целевых буферов отрисовки
        
      if (need_update_render_targets || need_update_area)
        ReconfigureRenderTargets ();

        //переконфигурация свойств

      if (need_update_properties)
        ReconfigureProperties ();

        //обновление флагов

      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::Reconfigure");
      throw;
    }
  }

/// Очистка после отрисовки кадра
  void Cleanup ()
  {
    current_frame_index = 0;
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Viewport::Viewport (const RenderManager& render_manager, const RenderTargetMap& render_target_map, const ViewportDrawList& viewports, size_t max_draw_depth)
{
  try
  {
    impl = new Impl (render_manager, render_target_map, viewports, max_draw_depth);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Viewport::Viewport");
    throw;
  }
}

Viewport::Viewport (const Viewport& vp)
  : impl (vp.impl)
{
  addref (impl);
}

Viewport::~Viewport ()
{
  release (impl);
}

Viewport& Viewport::operator = (const Viewport& vp)
{
  Viewport tmp (vp);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Имя области вывода
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::SetName", "name");

  impl->name = name;
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    Область
*/

void Viewport::SetArea (const Rect& rect)
{
  if (rect == impl->area)
    return;

  impl->area                 = rect;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

const Rect& Viewport::Area () const
{
  return impl->area;
}

/*
    Диапазон глубины для области вывода
*/

void Viewport::SetMinDepth (float value)
{
  if (value == impl->min_depth)
    return;

  impl->min_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

void Viewport::SetMaxDepth (float value)
{
  if (value == impl->max_depth)
    return;

  impl->max_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

float Viewport::MinDepth () const
{
  return impl->min_depth;
}

float Viewport::MaxDepth () const
{
  return impl->max_depth;
}

/*
    Активность области вывода
*/

void Viewport::SetActive (bool active)
{
  impl->is_active = active;
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    Порядок вывода
*/

void Viewport::SetZOrder (int order)
{
  if (order == impl->zorder)
    return;

  impl->zorder = order;

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnViewportZOrderUpdated (order);
    }
    catch (...)
    {
    }
  }
}

int Viewport::ZOrder () const
{
  return impl->zorder;
}

/*
    Задний фон
*/

void Viewport::SetBackground (bool state, const math::vec4f& color)
{
  if (state == impl->background_state && color == impl->background_color)
    return;

  impl->background_state       = state;
  impl->background_color       = color;
  impl->need_update_background = true;
  impl->need_reconfiguration   = true;
}

bool Viewport::BackgroundState () const
{
  return impl->background_state;
}

const math::vec4f& Viewport::BackgroundColor () const
{
  return impl->background_color;
}

/*
    Техника отрисовки
*/

void Viewport::SetTechnique (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::SetTechnique", "name");

  if (name == impl->technique_name)
    return;

  impl->sub_techniques.clear ();

  impl->technique_name        = name;
  impl->need_update_technique = true;
  impl->need_reconfiguration  = true;
}

const char* Viewport::Technique () const
{
  return impl->technique_name.c_str ();
}

/*
    Свойства отрисовки
*/

void Viewport::SetProperties (const common::PropertyMap& properties)
{
  impl->properties             = properties;
  impl->need_update_properties = true;
  impl->need_reconfiguration   = true;
}

const common::PropertyMap& Viewport::Properties () const
{
  return impl->properties;
}

/*
    Камера
*/

Camera* Viewport::Camera ()
{
  return &impl->camera;
}

const Camera* Viewport::Camera () const
{
  return &impl->camera;
}

/*
    Сцена
*/

void Viewport::SetScene (render::scene::server::Scene* scene)
{
  if (scene) impl->scene.reset (new server::Scene (*scene));
  else       impl->scene.reset ();
}

render::scene::server::Scene* Viewport::Scene ()
{
  return impl->scene.get ();
}

const render::scene::server::Scene* Viewport::Scene () const
{
  return impl->scene.get ();
}

/*
    Максимальный уровень вложенности рендеринга
*/

void Viewport::SetMaxDrawDepth (size_t level)
{
  if (level == impl->max_draw_depth)
    return;

  level++;

  impl->max_draw_depth = level;

  if (level < impl->frames.size ())
    impl->frames.erase (impl->frames.begin () + level);

  impl->frames.reserve (level);

//?????????????

  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Viewport::MaxDrawDepth () const
{
  return impl->max_draw_depth;
}

/*
    Обновление
*/

void Viewport::Update (manager::Frame* parent_frame)
{
  try
  {
      //отрисовка только в случае активности области вывода
    
    if (!impl->is_active)
      return;

      //отрисовка только в случае наличия присоединенной сцены

    if (!impl->scene)
      return;

      //добавление области вывода в список отрисовки

    impl->AddToDrawingList ();

      //переконфигурация в случае изменения существенных параметров

    if (impl->need_reconfiguration)
      impl->Reconfigure ();

      //получение текущего кадра

    Frame* frame_desc = impl->CurrentFrame ();

    if (!frame_desc)
      return; //достигнута максимальная глубина вложенности кадров

    impl->current_frame_index++;

      //подготовка кэша результатов обхода сцены

    TraverseResult& traverse_result = impl->render_manager.TraverseResultStorage ();

    traverse_result.Clear ();

    traverse_result.visual_models.reserve (TRAVERSE_RESULT_VISUAL_MODELS_RESERVE_SIZE);
    traverse_result.lights.reserve (TRAVERSE_RESULT_LIGHTS_RESERVE_SIZE);

    CameraTraverseResult camera_traverse_result (impl->camera.Frustum (), *impl->scene, traverse_result);

      //создание контекста рендеринга

    RenderingContext context (frame_desc->frame, impl->render_manager, camera_traverse_result);

      //обновление кадра

    for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
    {
      try
      {
        (*iter)->UpdateFrame (context, frame_desc->private_data [iter - impl->sub_techniques.begin ()]);
        
        ++iter;
        
        continue;
      }
      catch (std::exception& e)
      {
        impl->render_manager.Log ().Printf ("%s\n    at update sub-technique '%s' frame for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Update", e.what (), (*iter)->Name (), impl->technique_name.c_str (), impl->name.c_str ());
      }
      catch (...)
      {
        impl->render_manager.Log ().Printf ("unknown exception\n    at update sub-technique '%s' frame for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Update", (*iter)->Name (), impl->technique_name.c_str (), impl->name.c_str ());
      }

      impl->sub_techniques.erase (iter);

      end = impl->sub_techniques.end ();
    }

      //отрисовка
      
    manager::Frame& frame = frame_desc->frame;
    
    if (frame.EntitiesCount () || frame.FramesCount ())
    {
      if (parent_frame) parent_frame->AddFrame (frame);
      else              frame.Draw ();    
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Viewport::Update(viewport='%s')", impl->name.c_str ());
    throw;
  }
}

/*
    Добавление слушателей
*/

void Viewport::AttachListener (IViewportListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
===================================================================================================
    ViewportDrawList
===================================================================================================
*/

ViewportDrawList::ViewportDrawList ()
  : impl (new Impl)
{
}

ViewportDrawList::ViewportDrawList (const ViewportDrawList& list)
  : impl (list.impl)
{
  addref (impl);
}

ViewportDrawList::~ViewportDrawList ()
{
  release (impl);
}

ViewportDrawList& ViewportDrawList::operator = (const ViewportDrawList& list)
{
  ViewportDrawList tmp (list);

  stl::swap (impl, tmp.impl);

  return *this;
}

void ViewportDrawList::CleanupViewports ()
{
  for (ViewportDrawListNode* node=impl->first; node;)
  {
    ViewportDrawListNode* next = node->next;

    node->Cleanup ();
    node->RemoveFromDrawingList ();

    node = next;
  }
}
