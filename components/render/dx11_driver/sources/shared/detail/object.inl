/*
    Приведение типов объектов
*/

template <class DstT, class SrcT>
inline DstT* cast_object (SrcT* ptr, const char* source, const char* argument_name)
{
  if (!ptr)
    return 0;

  ObjectFamily family;
  void*        root = 0;

  memset (&family, 0, sizeof (family));

  ptr->GetFamily (family, root);

  if (family.id != Object::FAMILY_ID || !root)
    throw xtl::format_exception<xtl::bad_argument> (source, "Invalid argument <%s>. No conversion from %s to %s", argument_name, typeid (SrcT).name (), typeid (DstT).name ());

  return static_cast<DstT*> (static_cast<Object*> (root));
}
