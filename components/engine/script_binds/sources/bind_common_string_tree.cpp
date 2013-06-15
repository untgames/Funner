#include "shared.h"

#include <stl/vector>

#include <xtl/iterator_range.h>
#include <xtl/reference_counter.h>

#include <common/log.h>
#include <common/parser.h>
#include <common/xml_writer.h>

using namespace common;
using namespace engine;
using namespace script;
using namespace xtl;

namespace
{

template class xtl::declcast<StringNode>;

/*
    Константы
*/

const char* LOG_NAME                   = "script.binds.StringTree";
const char* COMMON_STRING_TREE_LIBRARY = "Common.StringNode";

//получение протокола
Log& get_log ()
{
  static Log log (LOG_NAME);
  
  return log;
}

}

///Узел дерева строк
struct StringNode::Impl
{
  Impl ()
    : ref_count (1)
    {}

  ///Работа с атрибутами
  size_t AttributesCapacity () const
  {
    return attribute_offsets.capacity ();
  }

  size_t AttributesCount () const
  {
    return attribute_offsets.size ();
  }

  void ReserveAttributes (size_t new_size)
  {
    attribute_offsets.reserve (new_size);
    attributes.reserve (new_size * 8);
  }

  const char* Attribute (size_t index) const
  {
    if (index >= AttributesCount ())
      throw xtl::make_range_exception ("script::binds::StringNode::Attribute", "index", index, 0u, AttributesCount ());

    return attributes.data () + attribute_offsets [index];
  }

  void SetAttribute (size_t index, const char* value)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::Attribute";

    if (!value)
      throw xtl::make_null_argument_exception (METHOD_NAME, "value");

    if (index >= AttributesCount ())
      throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, AttributesCount ());
      
    size_t value_length = xtl::xstrlen (value);

    if (index < AttributesCount () - 1)
    {
      if (value_length < (attribute_offsets [index + 1] - attribute_offsets [index]))
      {
        xtl::xstrncpy (&attributes [attribute_offsets [index]], value, value_length);
        attributes [value_length] = 0;

        return;
      }
    }

    attributes.reserve (attributes.length () + value_length + 1);

    size_t offset = attributes.length ();

    attributes                += value;
    attributes                += '\0';
    attribute_offsets [index]  = offset;
  }

  size_t AddAttribute (const char* value)
  {
    if (!value)
      throw xtl::make_null_argument_exception ("script::binds::StringNode::AddAttribute", "value");

    attributes.reserve (attributes.length () + xtl::xstrlen (value) + 1);

    attribute_offsets.push_back (attributes.length ());
    attributes += value;
    attributes += '\0';

    return attribute_offsets.size () - 1;
  }

  void AddAttribute (size_t index, const char* value)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::AddAttribute";

    if (!value)
      throw xtl::make_null_argument_exception (METHOD_NAME, "value");

    if (index > AttributesCount ())
      throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, AttributesCount () + 1);

    attributes.reserve (attributes.length () + xtl::xstrlen (value) + 1);

    attribute_offsets.insert (attribute_offsets.begin () + index, attributes.length ());
    attributes += value;
    attributes += '\0';
  }

  void RemoveAttribute (size_t index)
  {
    if (index >= AttributesCount ())
      return;

    attribute_offsets.erase (attribute_offsets.begin () + index);

    if (attribute_offsets.empty ())
      attributes.clear ();
  }

  void RemoveAllAttributes ()
  {
    attribute_offsets.clear ();
    attributes.clear ();
  }

  ///Работа с детьмя
  size_t ChildrenCapacity () const
  {
    return childs.capacity ();
  }

  size_t ChildrenCount () const
  {
    return childs.size ();
  }

  void ReserveChildren (size_t new_size)
  {
    childs.reserve (new_size);
  }

  Pointer Child (size_t index) const
  {
    if (index >= ChildrenCount ())
      throw xtl::make_range_exception ("script::binds::StringNode::Child", "index", index, 0u, ChildrenCount ());

    return childs [index];
  }

  size_t AddChild (StringNode& new_child)
  {
    AddChild (ChildrenCount (), new_child);

    return childs.size () - 1;
  }

  void AddChild (size_t index, StringNode& new_child)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::AddChild";

    if (index > ChildrenCount ())
      throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, ChildrenCount () + 1);

    if (new_child.impl->HasNode (this))
      throw xtl::format_operation_exception (METHOD_NAME, "Can't add child because it contains this node");

    childs.insert (childs.begin () + index, Pointer (&new_child));
  }

  void RemoveChild (size_t index)
  {
    if (index >= ChildrenCount ())
      return;

    childs.erase (childs.begin () + index);
  }

  void RemoveAllChildren ()
  {
    childs.clear ();
  }

  ///Загрузка/сохранение
  static Pointer LoadXml (const char* file_name)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::LoadXml";

    if (!file_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

    if (strstr (file_name, "<?xml"))
      return LoadXmlFromString (file_name);

    return LoadXmlFromFile (file_name);
  }

  static Pointer LoadFromFile (const char* file_name, const char* format)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::LoadXmlFromFile";

    if (!file_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

    if (!format)
      throw xtl::make_null_argument_exception (METHOD_NAME, "format");

    Pointer return_value (new StringNode, false);

    Parser parser (file_name, format);
    ParseLog parse_log = parser.Log ();

    for (size_t i = 0; i < parse_log.MessagesCount (); i++)
      switch (parse_log.MessageType (i))
      {
        case ParseLogMessageType_Error:
        case ParseLogMessageType_FatalError:
          get_log ().Printf ("Parser error: '%s'", parse_log.Message (i));
          break;
        default:
          break;
      }

    ParseNode iter = parser.Root ().First ();

    ProcessNode (return_value, iter);

    return return_value;
  }

  static Pointer LoadFromString (const char* string, const char* format)
  {
    static const char* METHOD_NAME = "script::binds::StringNode::LoadXmlFromString";

    if (!string)
      throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

    if (!format)
      throw xtl::make_null_argument_exception (METHOD_NAME, "format");

    Pointer return_value (new StringNode, false);

    Parser parser (format, xtl::xstrlen (string), string, format);
    ParseLog parse_log = parser.Log ();

    for (size_t i = 0; i < parse_log.MessagesCount (); i++)
      switch (parse_log.MessageType (i))
      {
        case ParseLogMessageType_Error:
        case ParseLogMessageType_FatalError:
          get_log ().Printf ("Parser error: '%s'", parse_log.Message (i));
          break;
        default:
          break;
      }

    ParseNode iter = parser.Root ().First ();

    if (!iter)
      throw xtl::format_operation_exception (METHOD_NAME, "There is no root node in string '%s'", string);

    ProcessNode (return_value, iter);

    return return_value;
  }

  void SaveXml (const char* file_name)
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("script::binds::StringNode::SaveXml", "file_name");

    XmlWriter writer (file_name);

    SaveNode (writer);
  }
  
  void SafeSaveXml (const char* file_name)
  {
    try
    {
      if (!file_name)
        throw xtl::make_null_argument_exception ("", "file_name");

      stl::string tmp_file_name = common::format ("%s.tmp", file_name);

      SaveXml (tmp_file_name.c_str ());
      
      try
      {
        common::FileSystem::Remove (file_name);
      }
      catch (...)
      {
        //подавление всех исключений
      }

      common::FileSystem::Rename (tmp_file_name.c_str (), file_name);

      try
      {
        common::FileSystem::Remove (tmp_file_name.c_str ());
      }
      catch (...)
      {
        //подавление всех исключений
      }            
    }
    catch (xtl::exception& e)
    {
      e.touch ("script::binds::StringNode::SafeSaveXml");
      throw;
    }
  }  

  ///Поиск узла
  Pointer FindNode (const char* name_to_find, const char* value, bool create_if_not_exist)
  {
    if (!name_to_find)
      throw xtl::make_null_argument_exception ("script::binds::StringNode::FindNode", "name");

    const char* subname = strchr (name_to_find, '.');

    if (subname)
    {
      size_t base_name_len = subname - name_to_find;

      for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
        if (!xstrncmp (name_to_find, (*iter)->Name (), base_name_len))
          return (*iter)->FindNode (subname + 1, value, create_if_not_exist);
    }
    else
    {
      for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
      {
        if (!xstrcmp (name_to_find, (*iter)->Name ()))
        {
          if (value)
          {
            const char* attr = (*iter)->Attribute (0);

            if (attr && !xstrcmp (value, attr))
              return *iter;
          }
          else
          {
            return *iter;
          }
        }
      }
    }

    if (create_if_not_exist)
      return CreateNode (name_to_find);

    return 0;
  }

  static void ProcessNode (StringNode::Pointer string_node, const ParseNode& node)
  {
    if (!node)
      return;
    
    string_node->SetName (node.Name ());

    string_node->ReserveAttributes (node.AttributesCount ());

    for (size_t i = 0; i < node.AttributesCount (); i++)
      string_node->AddAttribute (node.Attribute (i));

    size_t children_count = 0;

    for (ParseNode iter = node.First (); iter; iter = iter.Next ())
      children_count++;

    string_node->ReserveChildren (children_count);

    for (ParseNode iter = node.First (); iter; iter = iter.Next ())
    {
      if (!xstrcmp ("#text", iter.Name ()))
      {
        for (size_t i = 0; i < iter.AttributesCount (); i++)
          string_node->AddAttribute (iter.Attribute (i));

        continue;
      }

      StringNode::Pointer child = StringNode::Create ();

      ProcessNode (child, iter);

      string_node->AddChild (*child);
    }
  }

  void SaveNode (XmlWriter& writer)
  {
    if (name.empty ())
    {
      get_log ().Print ("Can't save node, empty name");
      return;
    }

    if ((attribute_offsets.size () == 1) && (childs.empty ()))
    {
      writer.WriteAttribute (name.c_str (), attributes.data () + attribute_offsets[0]);
      return;
    }

    XmlWriter::Scope scope (writer, name.c_str ());

    for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
      if ((*iter)->impl->attribute_offsets.size () == 1)
        (*iter)->impl->SaveNode (writer);

    if (attribute_offsets.size () > 1)
    {
      AttributeIterator start (&attribute_offsets [0], attributes.c_str ()),
                        finish (&*attribute_offsets.end (), attributes.c_str ());

      writer.WriteData (xtl::make_const_ref (xtl::make_iterator_range (start, finish)));
    }

    for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
      if ((*iter)->impl->attribute_offsets.size () != 1)
        (*iter)->impl->SaveNode (writer);
  }

  bool HasNode (StringNode::Impl* node)
  {
    if (this == node)
      return true;

    for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
      if ((*iter)->impl->HasNode (node))
        return true;

    return false;
  }

  Pointer CreateNode (const char* node_name)
  {
    const char* subname = strchr (node_name, '.');

    if (subname)
    {
      Pointer new_child = Create ();

      new_child->impl->name = stl::string (node_name, subname - node_name);

      Pointer result = new_child->impl->CreateNode (subname + 1);

      AddChild (*new_child);

      return result;
    }
    else
    {
      Pointer new_child = Create ();

      new_child->impl->name = node_name;

      AddChild (*new_child);

      return new_child;
    }
  }

  typedef stl::vector<size_t>  AttributeOffsetArray;
  typedef stl::vector<Pointer> ChildArray;

  struct AttributeIterator: public stl::iterator<stl::forward_iterator_tag, const char*>
  {
    public:
      AttributeIterator (const size_t* in_offset, const char* in_attributes) : offset (in_offset), attributes (in_attributes) {}

      AttributeIterator& operator ++ () {
        offset++;

        return *this;
      }

      AttributeIterator operator ++ (int) {
        AttributeIterator tmp = *this;

        ++*this;

        return tmp;
      }

      const char* operator * () const { return attributes + *offset; }

      bool operator == (const AttributeIterator& iter) const { return offset == iter.offset; }
      bool operator != (const AttributeIterator& iter) const { return offset != iter.offset; }

    private:
      const size_t* offset;
      const char*   attributes;
  };

  stl::string          name;                 //имя узла
  stl::string          attributes;           //значения атрибутов
  AttributeOffsetArray attribute_offsets;    //массив смещений атрибутов
  ChildArray           childs;               //дети
  size_t               ref_count;            //количество ссылок
};

/*
   Конструктор/деструктор
*/

StringNode::StringNode ()
  : impl (new Impl)
  {}

StringNode::~StringNode ()
{
  delete impl;
}

/*
   Создание нового узла
*/

StringNode::Pointer StringNode::Create ()
{
  return Pointer (new StringNode, false);
}

/*
   Создание копии
*/

StringNode::Pointer StringNode::Clone ()
{
  Pointer return_value (new StringNode, false);

  return_value->impl->name              = impl->name;
  return_value->impl->attributes        = impl->attributes;
  return_value->impl->attribute_offsets = impl->attribute_offsets;

  for (Impl::ChildArray::iterator iter = impl->childs.begin (), end = impl->childs.end (); iter != end; ++iter)
    return_value->AddChild (*(*iter)->Clone ());

  return return_value;
}

/*
   Получение имени/переименование
*/

const char* StringNode::Name () const
{
  return impl->name.c_str ();
}

void StringNode::SetName (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("script::binds::StringNode::SetName", "new_name");

  impl->name = new_name;
}

/*
   Работа с атрибутами
*/

size_t StringNode::AttributesCapacity () const
{
  return impl->AttributesCapacity ();
}

size_t StringNode::AttributesCount () const
{
  return impl->AttributesCount ();
}

void StringNode::ReserveAttributes (size_t new_size)
{
  impl->ReserveAttributes (new_size);
}

const char* StringNode::Attribute (size_t index) const
{
  return impl->Attribute (index);
}

void StringNode::SetAttribute (size_t index, const char* value)
{
  impl->SetAttribute (index, value);
}

size_t StringNode::AddAttribute (const char* value)
{
  return impl->AddAttribute (value);
}

void StringNode::AddAttribute (size_t index, const char* value)
{
  impl->AddAttribute (index, value);
}

void StringNode::RemoveAttribute (size_t index)
{
  impl->RemoveAttribute (index);
}

void StringNode::RemoveAllAttributes ()
{
  impl->RemoveAllAttributes ();
}

/*
   Работа с детьмя
*/

size_t StringNode::ChildrenCapacity () const
{
  return impl->ChildrenCapacity ();
}

size_t StringNode::ChildrenCount () const
{
  return impl->ChildrenCount ();
}

void StringNode::ReserveChildren (size_t new_size)
{
  impl->ReserveChildren (new_size);
}

StringNode::Pointer StringNode::Child (size_t index) const
{
  return impl->Child (index);
}

size_t StringNode::AddChild (StringNode& new_child)
{
  return impl->AddChild (new_child);
}

void StringNode::AddChild (size_t index, StringNode& new_child)
{
  impl->AddChild (index, new_child);
}

void StringNode::RemoveChild (size_t index)
{
  impl->RemoveChild (index);
}

void StringNode::RemoveAllChildren ()
{
  impl->RemoveAllChildren ();
}

/*
   Загрузка/сохранение
*/

StringNode::Pointer StringNode::LoadXml (const char* file_name)
{
  return Impl::LoadXml (file_name);
}

StringNode::Pointer StringNode::LoadXmlFromFile (const char* file_name)
{
  return Impl::LoadFromFile (file_name, "xml");
}

StringNode::Pointer StringNode::LoadXmlFromString (const char* string)
{
  return Impl::LoadFromString (string, "xml");
}

StringNode::Pointer StringNode::LoadJsonFromFile (const char* file_name)
{
  return Impl::LoadFromFile (file_name, "json");
}

StringNode::Pointer StringNode::LoadJsonFromString (const char* string)
{
  return Impl::LoadFromString (string, "json");
}

void StringNode::SaveXml (const char* file_name)
{
  impl->SaveXml (file_name);
}

void StringNode::SafeSaveXml (const char* file_name)
{
  impl->SafeSaveXml (file_name);
}

/*
   Поиск узла
*/

StringNode::Pointer StringNode::FindNode (const char* name_to_find, const char* value, bool create_if_not_exist)
{
  return impl->FindNode (name_to_find, value, create_if_not_exist);
}

/*
   Подсчёт ссылок
*/

void StringNode::AddRef () const
{
  if (impl->ref_count)
    impl->ref_count++;
}

void StringNode::Release () const
{
    //защита от повторного удаления в обработчике

  if (!impl->ref_count)
    return;

  if (!--impl->ref_count)
    delete this;
}

/*
   Получение значения нулевого атрибута дочернего узла с именем name
*/

const char* StringNode::Get (const char* name)
{
  StringNode::Pointer find_node = FindNode (name, 0, false);

  if (!find_node)
    throw xtl::format_operation_exception ("script::binds::get (StringNode, const char*)", "There is no node '%s'", name);

  if (!find_node->AttributesCount ())
    throw xtl::format_operation_exception ("script::binds::get (StringNode, const char*)", "Node '%s' hasn't attributes", name);

  return find_node->Attribute (0);
}

const char* StringNode::Get (const char* name, const char* default_value)
{
  StringNode::Pointer find_node = FindNode (name, 0, false);

  if (!find_node)
    return default_value;

  if (!find_node->AttributesCount ())
    return default_value;

  return find_node->Attribute (0);
}

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск дочернего узла по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
StringNode::Pointer find (StringNode& node, const char* name)
{
  return node.FindNode (name, 0, false);
}

StringNode::Pointer find (StringNode& node, const char* name, const char* value)
{
  return node.FindNode (name, value, false);
}

void set (StringNode& node, const char* name, const char* value)
{
  StringNode::Pointer find_node = node.FindNode (name, 0, true);

  if (find_node->AttributesCount ())
    find_node->SetAttribute (0, value);
  else
    find_node->AddAttribute (value);
}

}

namespace engine
{

/*
    Регистрация библиотеки работы с деревьями строк
*/

void bind_common_string_tree (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_STRING_TREE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&StringNode::Create));

    //регистрация операций

  lib.Register ("set_Name",               make_invoker (&StringNode::SetName));
  lib.Register ("get_Name",               make_invoker (&StringNode::Name));
  lib.Register ("get_AttributesCount",    make_invoker (&StringNode::AttributesCount));
  lib.Register ("get_AttributesCapacity", make_invoker (&StringNode::AttributesCapacity));
  lib.Register ("get_ChildrenCount",      make_invoker (&StringNode::ChildrenCount));
  lib.Register ("get_ChildrenCapacity",   make_invoker (&StringNode::ChildrenCapacity));

  lib.Register ("Clone",               make_invoker (&StringNode::Clone));
  lib.Register ("ReserveAttributes",   make_invoker (&StringNode::ReserveAttributes));
  lib.Register ("Attribute",           make_invoker (&StringNode::Attribute));
  lib.Register ("SetAttribute",        make_invoker (&StringNode::SetAttribute));
  lib.Register ("AddAttribute",        make_invoker (make_invoker (implicit_cast<void (StringNode::*) (size_t, const char*)> (&StringNode::AddAttribute)),
                                                     make_invoker (implicit_cast<size_t (StringNode::*) (const char*)> (&StringNode::AddAttribute))));
  lib.Register ("RemoveAttribute",     make_invoker (&StringNode::RemoveAttribute));
  lib.Register ("RemoveAllAttributes", make_invoker (&StringNode::RemoveAllAttributes));
  lib.Register ("ReserveChildren",     make_invoker (&StringNode::ReserveChildren));
  lib.Register ("Child",               make_invoker (&StringNode::Child));
  lib.Register ("AddChild",            make_invoker (make_invoker (implicit_cast<void (StringNode::*) (size_t, StringNode&)> (&StringNode::AddChild)),
                                                     make_invoker (implicit_cast<size_t (StringNode::*)(StringNode&)> (&StringNode::AddChild))));
  lib.Register ("RemoveChild",         make_invoker (&StringNode::RemoveChild));

  lib.Register ("RemoveAllChildren",   make_invoker (&StringNode::RemoveAllChildren));
  lib.Register ("LoadXml",             make_invoker (&StringNode::LoadXml));
  lib.Register ("LoadXmlFromString",   make_invoker (&StringNode::LoadXmlFromString));
  lib.Register ("LoadXmlFromFile",     make_invoker (&StringNode::LoadXmlFromFile));
  lib.Register ("LoadJsonFromString",  make_invoker (&StringNode::LoadJsonFromString));
  lib.Register ("LoadJsonFromFile",    make_invoker (&StringNode::LoadJsonFromFile));
  lib.Register ("SaveXml",             make_invoker (&StringNode::SaveXml));
  lib.Register ("SafeSaveXml",         make_invoker (&StringNode::SafeSaveXml));  
  lib.Register ("Find",                make_invoker (
                                                     make_invoker ((StringNode::Pointer (*)(StringNode&, const char*, const char*))&find),
                                                     make_invoker ((StringNode::Pointer (*)(StringNode&, const char*))&find)
                                                    ));
  lib.Register ("Get",                 make_invoker (make_invoker (implicit_cast<const char* (StringNode::*) (const char*, const char*)> (&StringNode::Get)),
                                                     make_invoker (implicit_cast<const char* (StringNode::*) (const char*)> (&StringNode::Get))));
  lib.Register ("Set",                 make_invoker (&set));

    //регистрация типов данных

  environment.RegisterType<StringNode> (COMMON_STRING_TREE_LIBRARY);
}

}
