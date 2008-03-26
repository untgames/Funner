#ifndef XTL_ANY_HEADER
#define XTL_ANY_HEADER

#include <exception>
#include <typeinfo>
#include <xtl/dynamic_cast_root.h>
#include <xtl/lexical_cast.h>
#include <xtl/default_cast_type.h>
#include <xtl/type.h>
#include <xtl/type_traits> //for is_polymorphic, remove_reference
#include <xtl/ref.h>

namespace stl
{

//forward declarations
template <class T> class auto_ptr;

}

namespace xtl
{

namespace detail
{

//implementation forwards
struct any_holder;

}

//forward declaration
template <class T> class shared_ptr;
template <class T> class com_ptr;

template <class T, template <class > class Strategy>  class intrusive_ptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: ������ ���������� any-���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class bad_any_cast: public std::bad_cast
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    enum cast_error
    {
      bad_cast,              //����������� ������ ����������
      bad_direct_cast,       //������ ������� ���������� �����
      bad_const_cast,        //������ ��������� �������������� ����
      bad_dynamic_cast,      //������ ������������� ���������� �����
      bad_to_string_cast,    //������ ������������������� ���������� ���� � ������
      bad_to_value_cast,     //������ ������������������� ���������� ���� �� ������
      bad_to_reference_cast  //������ ���������� �������� � ������ (������������� ���������� � ������ � to_value)
    };
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bad_any_cast (cast_error error_id = bad_cast);
    bad_any_cast (cast_error error_id, const std::type_info& source_type, const std::type_info& target_type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� � ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& source_type () const;
    const std::type_info& target_type () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    cast_error error () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* what () const throw ();

  private:
    cast_error           error_id;
    const std::type_info *source, *target;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� �������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class any
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    any  ();
    any  (const any&);
    ~any ();
                       
    template <class T> any (const T& value, bool ref_counted=false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    any& operator = (const any&);
    
    template <class T> any& operator = (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� / �������� �� ��������� 0 (���� castable_type - ������ ��� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;
    bool null  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& type () const;
    const std::type_info& castable_type () const;

    template <class T>       T* content ();
    template <class T> const T* content () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> const T cast () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void to_string   (stl::string& buffer) const;
    void set_content (const stl::string& buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    any& swap (any&);
    
  private:
    template <class T>
    static detail::any_holder* create_holder (const T&, bool ref_counted);
                       
  private: 
    detail::any_holder* content_ptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (any&, any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� any � ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> any make_ref_any (T&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>       T* any_cast (any*);
template <class T> const T* any_cast (const any*);
template <class T>       T  any_cast (any&);
template <class T> const T  any_cast (const any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������� �����:
///   - ������� ���������� �������������� (const, volataile, const volataile)
///   - ������� dynamic_cast ���������� (��� ����� ���������� � dynamic_cast_root)
///   - ������� ������������������ ���������� (lexical_cast)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> const T any_multicast (const any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ��������. ������������ ��� ������� ��� ������ any_multicast
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T& get_castable_value (T&);
template <class T> T& get_castable_value (T*);
template <class T> T& get_castable_value (stl::auto_ptr<T>&);
template <class T> T& get_castable_value (shared_ptr<T>&);
template <class T> T& get_castable_value (com_ptr<T>&);
template <class T> T& get_castable_value (reference_wrapper<T>&);

template <class T, template <class > class Strategy>
T& get_castable_value (intrusive_ptr<T, Strategy>&);

char*    get_castable_value (char*);
wchar_t* get_castable_value (wchar_t*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������������ ���������� ����� any (��� ��������� ���� any)
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_string (stl::string& buffer, const volatile any& value);
void to_value  (const stl::string& buffer, volatile any& value);

#include <xtl/detail/any.inl>

}

#endif
