#ifndef XTL_INSTANCE_COUNTER_HEADER
#define XTL_INSTANCE_COUNTER_HEADER

#include <typeinfo>

#include <stl/iterator_traits.h>

#include <xtl/reference_counter.h>
#include <xtl/singleton_default.h>

namespace xtl
{

namespace detail
{

//implementation forwards
template <class T>
class typed_instance_counter_group;

}

class instance_counter_group_iterator;

/////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� �����������
/////////////////////////////////////////////////////////////////////////////////////////////
class instance_counter_group: private reference_counter
{
  template <class T> friend class instance_counter;
  template <class T> friend class detail::typed_instance_counter_group;
  friend class instance_counter_group_iterator;
  public:      
/////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� � ������
/////////////////////////////////////////////////////////////////////////////////////////////  
    size_t size () const;    

/////////////////////////////////////////////////////////////////////////////////////////////
///���, ��������������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& type () const;

  private:
    instance_counter_group (const std::type_info&);  
  
    instance_counter_group (const instance_counter_group&); //no implementation
    instance_counter_group& operator = (const instance_counter_group&); //no implementation
    
  private:
    const std::type_info&   std_type;
    instance_counter_group* next;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �� ����
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T> instance_counter_group& get_instance_counter_group ();

/////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ��������� �����������
/////////////////////////////////////////////////////////////////////////////////////////////
class instance_counter_group_iterator 
{ 
  typedef instance_counter_group* (instance_counter_group_iterator::*unspecified_bool_type)() const;
  public:
    typedef instance_counter_group    value_type;
    typedef size_t                    size_type;
    typedef ptrdiff_t                 difference_type;
    typedef instance_counter_group*   pointer;
    typedef instance_counter_group&   reference;
    typedef stl::forward_iterator_tag iterator_category;

    instance_counter_group_iterator (bool is_last = false);
    instance_counter_group_iterator (const instance_counter_group_iterator&);    
    
/////////////////////////////////////////////////////////////////////////////////////////////
///������������
/////////////////////////////////////////////////////////////////////////////////////////////
    instance_counter_group_iterator& operator = (const instance_counter_group_iterator&);
    
/////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������������
/////////////////////////////////////////////////////////////////////////////////////////////
    operator unspecified_bool_type () const;
    bool     operator !            () const;

/////////////////////////////////////////////////////////////////////////////////////////////
///���������
/////////////////////////////////////////////////////////////////////////////////////////////
    reference operator  * () const;
    pointer   operator -> () const;

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ���������
/////////////////////////////////////////////////////////////////////////////////////////////
    instance_counter_group_iterator& operator ++ ();
    instance_counter_group_iterator  operator ++ (int);

/////////////////////////////////////////////////////////////////////////////////////////////
///���������
/////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const instance_counter_group_iterator&) const;
    bool operator != (const instance_counter_group_iterator&) const;

  private:  
    instance_counter_group* group;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� �� ������ � ����� ������ �����
/////////////////////////////////////////////////////////////////////////////////////////////
instance_counter_group_iterator get_first_instance_counter_group ();
instance_counter_group_iterator get_last_instance_counter_group  ();

/////////////////////////////////////////////////////////////////////////////////////////////
///������� �����������
/////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class instance_counter
{
  public:
    instance_counter  ();
    instance_counter  (const instance_counter&);
    ~instance_counter ();

    instance_counter& operator = (const instance_counter&);   
};

#include <xtl/detail/instance_counter.inl>

}

#endif
