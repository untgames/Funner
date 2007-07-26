#ifndef MEDIALIB_COLLADA_SCENE_HEADER
#define MEDIALIB_COLLADA_SCENE_HEADER

#include <media/collada/geometry.h>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightType
{
  LightType_Ambient, //���������� ���������
  LightType_Point,   //�������� �������� �����
  LightType_Spot,    //������������ �������������� �������� �����
  LightType_Direct,  //������������ �������������� �������� �����
  
  LightType_Default = LightType_Point
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightParam
{
  LightParam_AttenuationConstant,  //���������� ����������� ��������� �� ����������
  LightParam_AttenuationLinear,    //�������� ����������� ��������� �� ����������
  LightParam_AttenuationQuadratic, //������������ ����������� ��������� �� ����������
  LightParam_FalloffAngle,         //���� ���������
  LightParam_FalloffExponent,      //���������������� ����������� ��������� �� ����

  LightParam_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CameraType
{
  CameraType_Perspective, //������������� ������
  CameraType_Orthographic //��������������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CameraParam
{
  CameraParam_ZNear,       //����������� �� ������� ��������� ���������
  CameraParam_ZFar,        //����������� �� ������� ��������� ���������
  CameraParam_AspectRatio, //����������� ��������������� �� ����
  CameraParam_XFov,        //���� ������ �� X
  CameraParam_YFov,        //���� ������ �� Y
  CameraParam_XMagnitude,  //??
  CameraParam_YMagnitude,  //??  
  
  CameraParam_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Light: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LightType Type    () const;
    void      SetType (LightType type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& Color    () const;
    void               SetColor (const math::vec3f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetParam (LightParam param, float value);
    float Param    (LightParam param) const;

  protected:
    Light  (ModelImpl*, const char* id);
    ~Light ();
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Camera: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CameraType Type    () const;
    void       SetType (CameraType type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetParam (CameraParam param, float value);
    float Param    (CameraParam param) const;

   protected:
     Camera  (ModelImpl*, const char* id);
     ~Camera ();

   private:
     struct Impl;
     Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialBinds
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const collada::Material* FindMaterial (const char* symbol) const; //no throw
          collada::Material* FindMaterial (const char* symbol);       //no throw
    const collada::Material* FindMaterial (const Surface&) const;     //no throw
          collada::Material* FindMaterial (const Surface&);           //no throw

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� / �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetMaterial    (const char* symbol, Material&);
    void RemoveMaterial (const char* symbol);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������ ���������� ��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* TexcoordChannelName       (const char* material, const char* texchannel) const; //no throw
    void        SetTexcoordChannelName    (const char* material, const char* texchannel, const char* surface_channel);
    void        RemoveTexcoordChannelName (const char* material, const char* texchannel);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������ ���������� ��������� �����������. -1 � ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    int FindTexcoordChannel (const Surface&, const Texture&) const; //nothrow

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

  protected:
    MaterialBinds  (Entity&);
    ~MaterialBinds ();
  
  private:
    MaterialBinds (const MaterialBinds&); //no impl
    MaterialBinds& operator = (const MaterialBinds&); //no impl
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
class InstanceMesh
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
          collada::Mesh& Mesh ();
    const collada::Mesh& Mesh () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
          collada::MaterialBinds& MaterialBinds ();
    const collada::MaterialBinds& MaterialBinds () const;

  protected:
    InstanceMesh  (collada::Mesh&);
    ~InstanceMesh ();
    
  private:
    InstanceMesh (const InstanceMesh&); //no impl
    InstanceMesh& operator = (const InstanceMesh&); //no impl
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ��������� ���������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <> class ICollection<InstanceMesh>: public IItemCollection<InstanceMesh>
{
  public:
    virtual InstanceMesh& Create (Mesh&) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Node: public Entity
{
  public:
    typedef ICollection<Node>         NodeList;
    typedef ICollection<Camera>       CameraList;
    typedef ICollection<Light>        LightList;
    typedef ICollection<InstanceMesh> MeshList;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���� � �������� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* SubId    () const;
    void        SetSubId (const char*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& Transform    () const;
    void               SetTransform (const math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
          NodeList&   Nodes   ();       //����-�������
    const NodeList&   Nodes   () const;
          LightList&  Lights  ();       //��������� �����
    const LightList&  Lights  () const;
          CameraList& Cameras ();       //������
    const CameraList& Cameras () const;
          MeshList&   Meshes  ();       //����
    const MeshList&   Meshes  () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
//    size_t      LayersCount () const;
//    const char* Layer       (size_t layer_index) const;

  protected:
    Node  (ModelImpl*, const char* id);
    ~Node ();

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene: public Node
{
  protected:
    Scene (ModelImpl* owner, const char* id) : Node (owner, id) {}
};

}

}

#endif
