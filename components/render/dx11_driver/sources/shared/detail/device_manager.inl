/*
    Приведение типов объектов с проверкой совместимости
*/

template <class DstT, class SrcT>
inline DstT* cast_object (const DeviceManager& owner, SrcT* ptr, const char* source, const char* argument_name)
{
  if (!ptr)
    return 0;

  ObjectFamily family;
  void*        root = 0;

  memset (&family, 0, sizeof (family));

  ptr->GetFamily (family, root);

  if (family.id != Object::FAMILY_ID || family.sub_id != owner.GetFamilySubId () || !root)
    throw xtl::format_exception<xtl::bad_argument> (source, "Argument '%s' is incompatible with target IDevice", argument_name);

  return static_cast<DstT*> (static_cast<Object*> (root));
}

template <class DstT, class SrcT>
inline DstT* cast_object (const DeviceObject& owner, SrcT* ptr, const char* source, const char* argument_name)
{
  return cast_object<DstT> (owner.GetDeviceManager (), ptr, source, argument_name);
}
