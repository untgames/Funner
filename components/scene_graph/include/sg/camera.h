#ifndef SCENE_GRAPH_CAMERA_HEADER
#define SCENE_GRAPH_CAMERA_HEADER

#include <math/matrix.h>
#include <sg/entity.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Camera : public Entity
{
  public:  
    typedef xtl::com_ptr<Camera>       Pointer;
    typedef xtl::com_ptr<const Camera> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& ProjectionMatrix () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Camera  ();
    ~Camera ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� ������ �������� ������� ��� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateNotify ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ComputeProjectionMatrix (math::mat4f&) = 0;

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PerspectiveCamera : public Camera
{
  public:
    typedef xtl::com_ptr <PerspectiveCamera> Pointer;
    typedef xtl::com_ptr <const PerspectiveCamera> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetFovX  (const math::anglef& fov_x);
    void                SetFovY  (const math::anglef& fov_y);
    void                SetZNear (float z_near);
    void                SetZFar  (float z_far);
    const math::anglef& FovX     () const;
    const math::anglef& FovY     () const;
    float               ZNear    () const;
    float               ZFar     () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PerspectiveCamera  ();
    ~PerspectiveCamera ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
    void ComputeProjectionMatrix (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������ (������� ������� �������������, ������������ gluOrtho2D)
///////////////////////////////////////////////////////////////////////////////////////////////////
class OrthoCamera: public Camera
{
  public:
    typedef xtl::com_ptr <OrthoCamera> Pointer;
    typedef xtl::com_ptr <const OrthoCamera> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetLeft   (float left);
    void  SetRight  (float right);
    void  SetTop    (float top);
    void  SetBottom (float bottom);
    void  SetZNear  (float z_near);
    void  SetZFar   (float z_far);
    float Left      () const;
    float Right     () const;
    float Top       () const;
    float Bottom    () const;
    float ZNear     () const;
    float ZFar      () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OrthoCamera  ();
    ~OrthoCamera ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
    void ComputeProjectionMatrix (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

  private:
    struct Impl;
    Impl* impl;
};

}

#endif