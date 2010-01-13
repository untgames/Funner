/*
    Приведение типов объектов
*/

template <class DstT, class SrcT>
inline DstT* cast_object (SrcT* ptr, const char* source, const char* argument_name)
{
  if (!ptr)
    return 0;

  DstT* casted_ptr = dynamic_cast<DstT*> (ptr);

  if (!casted_ptr)
    throw xtl::format_exception<xtl::bad_argument> (source, "Invalid argument <%s>. No convertion from %s to %s", argument_name, typeid (SrcT).name (), typeid (DstT).name ());

  return casted_ptr;
}
