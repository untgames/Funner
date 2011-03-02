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

  ShapeType Type () { return (ShapeType)ShapeTypeId<T>::Type; }
  
  T data;
};

template <class T> T* Shape::IShapeData::Cast (IShapeData* data, detail::ShapeTypeSelector<T*>)
{
  if (!data)
    return 0;

  if (data->Type () == (ShapeType)ShapeTypeId<T>::Type)
    return &static_cast<ShapeData<T>*> (data)->data;
    
  return 0;
}

template <class T> T& Shape::IShapeData::Cast (IShapeData* data, detail::ShapeTypeSelector<T>)
{
  if (!data)
    Shape::RaiseNullArgument ("media::physics::Shape::IShapeData::Cast", "data");

  T* casted_data = Cast (data, detail::ShapeTypeSelector<T*> ());

  if (!casted_data)
    Shape::RaiseWrongType (data->Type (), (ShapeType)ShapeTypeId<T>::Type);

  return *casted_data;
}

template <class T> const typename ShapeDataType<T>::Type Shape::Data () const
{
  return const_cast<Shape&> (*this).Data<T> ();
}

template <class T> typename ShapeDataType<T>::Type Shape::Data ()
{
  return IShapeData::Cast (DataCore (), detail::ShapeTypeSelector<T> ());
}

template <class T> void Shape::SetData (const T& data)
{
  SetDataCore (new ShapeData<T> (data));
}
