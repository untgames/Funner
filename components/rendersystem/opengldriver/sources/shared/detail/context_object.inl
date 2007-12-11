/*
    Приведение типов объектов с проверкой совместимости
*/

template <class DstT, class SrcT>
inline DstT* cast_object (const ContextManager& owner, SrcT* ptr, const char* source, const char* argument_name)
{
  DstT* object = cast_object<DstT> (ptr, source, argument_name);

  if (!object->GetContextManager ().IsCompatible (owner))
    common::Raise<common::ArgumentException> (source, "Argument %s is incompatible with target IDevice", argument_name);
    
  return object;
}

template <class DstT, class SrcT>
inline DstT* cast_object (const ContextObject& owner, SrcT* ptr, const char* source, const char* argument_name)
{
  return cast_object<DstT> (owner.GetContextManager (), ptr, source, argument_name);
}
