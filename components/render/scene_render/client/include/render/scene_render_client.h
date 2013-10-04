#ifndef RENDER_SCENE_RENDER_HEADER
#define RENDER_SCENE_RENDER_HEADER

#include <stl/auto_ptr.h>
#include <xtl/functional_fwd>

#include <sg/camera.h>
#include <sg/screen.h>

namespace media
{

//forward declaration
class Image;

}

namespace render
{

namespace scene
{

namespace client
{

///forward declarations
class RenderTargetImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTarget
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget  ();
    RenderTarget  (const RenderTarget&);
    ~RenderTarget ();

    RenderTarget& operator = (const RenderTarget&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� (�������� �������� - weak-ref)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetScreen (scene_graph::Screen* screen);
    scene_graph::Screen* Screen    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ���� ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBindedToRenderer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������� (screen-shot)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);
    void CaptureImage (const char* image_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderTarget&);

  protected:
    RenderTarget (RenderTargetImpl*);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RenderTarget&, RenderTarget&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Client
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Client  (const char* connection_name, const char* init_string = "", size_t logon_timeout_ms = size_t (-1));
    Client  (const Client&);
    ~Client ();
    
    Client& operator = (const Client&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget CreateRenderTarget (const char* target_name, const char* init_string = "");

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetMaxDrawDepth (size_t level);
    size_t MaxDrawDepth    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* file_name);
    void UnloadResource (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Client&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Client&, Client&);

}

}

using scene::client::RenderTarget;
using scene::client::Client;

}

#endif
