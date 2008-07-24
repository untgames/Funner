#ifndef RENDER_SCENE_RENDER_CORE_SCENE_RENDER_SHARED_HEADER
#define RENDER_SCENE_RENDER_CORE_SCENE_RENDER_SHARED_HEADER

#include "shared.h"

namespace render
{

//forward declaration
class RenderTargetManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderPathManager
{
  public:
    typedef SceneRender::LogFunction LogFunction;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderPathManager () {}
    RenderPathManager (const char* driver_name_mask, const char* renderer_name_mask, const char* render_path_masks, const LogFunction& log_handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::IRenderer* Renderer () const { return renderer.get (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* RenderPaths () const { return render_paths_string.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ���� ���������� / ��������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool                HasRenderPath (const char* path_name) const;
    ICustomSceneRender& GetRenderPath (const char* path_name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource (const char* tag, const char* file_name, const LogFunction& log_handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderPathManager&);
    
  private:
    RenderPathManager (const RenderPathManager&); //no impl
    RenderPathManager& operator = (const RenderPathManager&); //no impl

  private:
    struct RenderPath
    {
      CustomSceneRenderPtr render;  
      stl::string          name;

      RenderPath (const CustomSceneRenderPtr& in_render, const char* in_name) : render (in_render), name (in_name) {}
    };

    typedef stl::hash_map<stl::hash_key<const char*>, RenderPath> RenderPathMap;
    typedef xtl::com_ptr<mid_level::IRenderer>                    RendererPtr;    

  private:
    RendererPtr   renderer;            //������� ����������
    RenderPathMap render_paths;        //���� ����������
    stl::string   render_paths_string; //������ � ������� ��������� ����� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��� ���������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetBase
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetManager& Manager () const { return *manager; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void FlushResources () = 0;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetBase  (RenderTargetManager&);
    ~RenderTargetBase ();

  private:
    RenderTargetBase (const RenderTargetBase&); //no impl
    RenderTargetBase& operator = (const RenderTargetBase&); //no impl
    
  private:
    typedef xtl::intrusive_ptr<RenderTargetManager> ManagerPtr;

  private:
    ManagerPtr manager; //�������� ����� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetManager: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetManager ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Register   (RenderTargetBase&);
    void Unregister (RenderTargetBase&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetRenderPathManager (render::RenderPathManager*);
    render::RenderPathManager* RenderPathManager    () { return render_path_manager; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushResources ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / ����� ���������� ��������� 
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool BeginDraw (); //��������� ���������� ������������ ��������� (������� ��� �������� �������� ������� ����������� ����������)
    void EndDraw   ();
    bool IsInDraw  () const { return draw_depth != 0; }

  private:
    RenderTargetManager (const RenderTargetManager&); //no impl
    RenderTargetManager& operator = (const RenderTargetManager&); //no impl

  private:
    typedef stl::hash_set<RenderTargetBase*> RenderTargetSet;

  private:
    render::RenderPathManager* render_path_manager; //�������� ����� ����������
    RenderTargetSet            render_targets;      //���� ����������
    size_t                     draw_depth;          //������� ����������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderView: private IViewportListener, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ����� �� RenderView � RenderTarget
///////////////////////////////////////////////////////////////////////////////////////////////////
    class IRenderTargetAPI
    {
      public:
        virtual render::mid_level::IRenderer& GetRenderer          () = 0; //��������� ������� ������������
        virtual const Rect&                   GetRenderableArea    () = 0; //��������� ���������� ������ ������� ����������
        virtual const Rect&                   GetDesktopArea       () = 0; //��������� ���������� ������ ������� ����������
        virtual RenderPathManager*            GetRenderPathManager () = 0; //��������� ��������� ����� ����������
        virtual void                          UpdateOrderNotify    () = 0; //���������� �� ���������� ������� ���������� �������� ������
        
      protected:
        virtual ~IRenderTargetAPI () {}
    };  

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderView  (const render::Viewport& vp, IRenderTargetAPI& render_target_api);
    ~RenderView ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::Viewport& Viewport () { return viewport; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushResources ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� ��������� ������ ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateAreaNotify ();

  private:
    void UpdateRenderView   ();
    void UpdateClearFrame   ();
    void OnChangeCamera     (scene_graph::Camera*);
    void OnChangeArea       (const Rect&);
    void OnChangeRenderPath (const char*);
    void OnChangeZOrder     (int);
    void OnChangeScene      ();
    void OnChangeProperty   (const char* name, const char* value);
    void OnChangeBackground (bool state, const math::vec4f& color);
    void AddRef             ();
    void Release            ();

  private:
    typedef xtl::com_ptr<IRenderView>                    RenderViewPtr;
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;

  private:
    render::Viewport      viewport;                //������� ������
    IRenderTargetAPI&     render_target_api;       //��������� �������� ����� � RenderTarget
    RenderViewPtr         render_view;             //������� ����������
    scene_graph::Scene*   current_scene;           //������� �����
    scene_graph::Camera*  current_camera;          //������� ������
    ClearFramePtr         clear_frame;             //���� �������
    bool                  need_update_view;        //���������� �������� ��������� ������� ������
    bool                  need_update_area;        //���������� �������� ���������� ������� ������
    bool                  need_update_camera;      //���������� �������� ������
    bool                  need_update_path;        //���������� �������� ���� ����������
    bool                  need_update_clear_frame; //���������� �������� ��������� ����
    xtl::auto_connection  on_camera_scene_change;  //���������� � �������� ���������� �� ��������� ����� � ������  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
void log_printf (const SceneRender::LogFunction& log_handler, const char* format, ...);

}

#endif
