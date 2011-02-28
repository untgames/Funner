namespace detail
{

template <class T> struct ShapeTypeSelector {};

}

struct Shape::IShapeData
{
  virtual ~IShapeData () {}

  virtual ShapeType Type () = 0;   
  
  template <class T> static T* Cast (IShapeData*, detail::ShapeTypeSelector<T*>);
  template <class T> static T& Cast (IShapeData*, detail::ShapeTypeSelector<T>);
};

template <class T> struct Shape::ShapeData: public IShapeData
{
  ShapeData (const T& in_data) : data (in_data) {}

  ShapeType Type () { return ShapeTypeId<T>::Type; }
  
  T data;
};

template <class T> T* Shape::IShapeData::Cast (IShapeData* data, detail::ShapeTypeSelector<T*>)
{
  if (!data)
    return 0;

  if (data->Type () == ShapeTypeId<T>::Type)
    return static_cast<ShapeData<T>*> (data)->data;
    
  return 0;
}

template <class T> T& Shape::IShapeData::Cast (IShapeData* data, detail::ShapeTypeSelector<T>)
{
  T* data = Cast (data, detail::ShapeTypeSelector<T*> ());

  if (!data)
    Shape::RaiseWrongType (Type (), ShapeTypeId<T>::Type);

  return *data;
}

template <class T> const typename ShapeDataType<T>::Type Shape::Data () const
{
  return const_cast<Shape&> (*this).Data ();
}

template <class T> typename ShapeDataType<T>::Type Shape::Data ()
{
  return Cast (DataCore (), detail::ShapeTypeSelector<T> ());
}

template <class T> void Shape::SetData (const T& data)
{
  SetDataCore (new ShapeData<T> (data));
}
