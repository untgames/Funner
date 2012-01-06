#ifndef SOCIAL_COLLECTION_HEADER
#define SOCIAL_COLLECTION_HEADER

#include <cstddef>

namespace xtl
{

//forward declaration
template <class T> class iterator;

}

namespace social
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item>
class Collection
{
  public:
    typedef xtl::iterator<Item>       Iterator;
    typedef xtl::iterator<const Item> ConstIterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Collection ();
    Collection (const Collection&);
    ~Collection ();

    Collection& operator = (const Collection&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (size_t index) const;
          Item& operator [] (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Capacity () const;
    void   Reserve  (size_t count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Add (Item&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
