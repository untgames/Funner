#ifndef COMMONLIB_VAR_REGISTRY_CONTAINER_HEADER
#define COMMONLIB_VAR_REGISTRY_CONTAINER_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/any.h>
#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/var_registry.h>

namespace common
{

namespace detail
{

//forward declaration
class VarRegistryContainerImpl;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class VarRegistryContainer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VarRegistryContainer ();
    VarRegistryContainer (const VarRegistryContainer& source);
    ~VarRegistryContainer ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VarRegistryContainer& operator = (const VarRegistryContainer& source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const T& GetValue (const char* var_name) const;
    void     SetValue (const char* var_name, const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveVariable (const char* var_name);
    void RemoveAllVariables ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasVariable (const char* var_name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������/������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Mount      (const char* branch_name);
    void Unmount    (const char* branch_name);
    void UnmountAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (VarRegistryContainer& source);

  private:
    detail::VarRegistryContainerImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void swap (VarRegistryContainer<T>& source1, VarRegistryContainer<T>& source2);

#include <common/detail/var_registry_container.inl>

}

#endif
