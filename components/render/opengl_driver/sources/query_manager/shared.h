#ifndef RENDER_GL_DRIVER_QUERY_MANAGER_SHARED_HEADER
#define RENDER_GL_DRIVER_QUERY_MANAGER_SHARED_HEADER

#include <xtl/common_exceptions.h>
#include <xtl/trackable_ptr.h>

#include <shared/context_object.h>
#include <shared/query_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum QueryManagerCacheEntry
{
  CacheEntry_IsInQueryRanges = CacheEntry_QueryManagerPrivateFirst  //��������� �� � ������ begin-end
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IOpenGlPredicate: virtual public IPredicate
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///  (�������� ����� �������� � ��������� ���������� ���� �� ���������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool GetResult () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsResultAvailable () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    QueryType GetType () { return QueryType_OcclusionPredicate; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    void GetData (size_t size, void* data, IDeviceContext*)
    {      
      if (size < sizeof (OcclusionPredicateQueryDesc))
        throw xtl::make_argument_exception ("render::low_level::opengl::IOpenGlPredicate::GetData", "size", size, "Size is too small");

      *reinterpret_cast<OcclusionPredicateQueryDesc*> (data) = GetResult ();
    }

    bool TryGetData (size_t size, void* data, IDeviceContext*)
    {
      if (size < sizeof (OcclusionPredicateQueryDesc))
        return false;

      if (!IsResultAvailable ())
        return false;

      *reinterpret_cast<OcclusionPredicateQueryDesc*> (data) = GetResult ();

      return true;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������, ����������� ��� ���������� ���������� ��������� occlusion query
///////////////////////////////////////////////////////////////////////////////////////////////////
class NullPredicate : virtual public IOpenGlPredicate, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Begin (IDeviceContext*) {}
    void End   (IDeviceContext*) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///  (�������� ����� �������� � ��������� ���������� ���� �� ���������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetResult () { return true; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsResultAvailable () { return true; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� � ����������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AsyncPredicate : virtual public IOpenGlPredicate, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AsyncPredicate  (const ContextManager&);
    ~AsyncPredicate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Begin (IDeviceContext*);
    void End   (IDeviceContext*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///  (�������� ����� �������� � ��������� ���������� ���� �� ���������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetResult ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsResultAvailable ();

  private:
    GLuint query; //OpenGL id �������
};

}

}

}

#endif
