#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

const char* LOG_NAME = "script.binds.common_string_node"; //имя потока протоколирования

const char* COMMON_STRING_TREE_LIBRARY = "StringNode";

//получение протокола
Log& get_log ()
{
  static Log log (LOG_NAME);
  
  return log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Узел дерева строк
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringNode: public xtl::reference_counter, public xtl::dynamic_cast_root
{
  public:
    typedef xtl::intrusive_ptr<StringNode> Pointer;

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание копии
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer Clone ()
    {
      Pointer return_value = Pointer (new StringNode, false);

      return_value->name       = name;
      return_value->attributes = attributes;

      return_value->attribute_offsets.insert (return_value->attribute_offsets.begin (), attribute_offsets.begin (), attribute_offsets.end ());

      for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
        return_value->AddChild ((*iter)->Clone ().get ());

      return return_value;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени/переименование
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const
    {
      return name.c_str ();
    }

    void SetName (const char* new_name)
    {
      if (!new_name)
        throw xtl::make_null_argument_exception ("script::binds::StringNode::SetName", "new_name");

      name = new_name;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с атрибутами
///////////////////////////////////////////////////////////////////////////////////////////////////
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

      return attributes.data () + attribute_offsets[index];
    }

    void SetAttribute (size_t index, const char* value)
    {
      static const char* METHOD_NAME = "script::binds::StringNode::Attribute";

      if (!value)
        throw xtl::make_null_argument_exception (METHOD_NAME, "value");

      if (index >= AttributesCount ())
        throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, AttributesCount ());

      if (index < (AttributesCount () - 1))
      {
        size_t value_length = xtl::xstrlen (value);

        if (value_length < (attribute_offsets[index + 1] - attribute_offsets[index]))
        {
          xtl::xstrncpy (&attributes[attribute_offsets[index]], value, value_length);
          attributes[value_length] = 0;
        }
      }
      else
      {
        attribute_offsets[index] = attributes.length ();
        attributes += value;
        attributes += '\0';
      }
    }

    size_t AddAttribute (const char* value)
    {
      if (!value)
        throw xtl::make_null_argument_exception ("script::binds::StringNode::AddAttribute", "value");

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с детьмя
///////////////////////////////////////////////////////////////////////////////////////////////////
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

    StringNode& Child (size_t index) const
    {
      if (index >= ChildrenCount ())
        throw xtl::make_range_exception ("script::binds::StringNode::Child", "index", index, 0u, ChildrenCount ());

      return *childs[index];
    }

    size_t AddChild (StringNode* new_child)
    {
      AddChild (ChildrenCount (), new_child);

      return childs.size () - 1;
    }

    void AddChild (size_t index, StringNode* new_child)
    {
      static const char* METHOD_NAME = "script::binds::StringNode::AddChild";

      if (index > ChildrenCount ())
        throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, ChildrenCount () + 1);

      if (!new_child)
        throw xtl::make_null_argument_exception (METHOD_NAME, "new_child");

      if (new_child->HasNode (this))
        throw xtl::format_operation_exception (METHOD_NAME, "Can't add child because it contains this node");

      childs.insert (childs.begin () + index, Pointer (new_child));
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка/сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer LoadXml (const char* file_name)
    {
      static const char* METHOD_NAME = "script::binds::StringNode::LoadXml";

      if (!file_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

      Pointer return_value = Pointer (new StringNode, false);

      ParseLog parse_log;
      Parser   parser (parse_log, file_name);      

      for (size_t i = 0; i < parse_log.MessagesCount (); i++)
        if (parse_log.MessageType (i) == PARSE_LOG_ERROR || parse_log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
          get_log ().Printf ("Parser error: '%s'", parse_log.Message (i));

      ParseNode* iter = parser.Root ()->First ();

      if (!iter)
        throw xtl::format_operation_exception (METHOD_NAME, "There is no root node in file '%s'", file_name);

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание нового узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ()
    {
      return Pointer (new StringNode, false);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer FindNode (const char* name_to_find, bool create_if_not_exist)
    {
      if (!name_to_find)
        throw xtl::make_null_argument_exception ("script::binds::StringNode::FindNode", "name");

      const char* subname = strchr (name_to_find, '.');

      if (subname)
      {    
        size_t base_name_len = subname - name_to_find;

        for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
          if (!xstrncmp (name_to_find, (*iter)->Name (), base_name_len))
            return (*iter)->FindNode (subname + 1, create_if_not_exist);
      }
      else
      {
        for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
          if (!xstrcmp (name_to_find, (*iter)->Name ()))
            return *iter;
      }

      if (create_if_not_exist)
        return CreateNode (name_to_find);

      return 0;
    }

  private:
    static void ProcessNode (StringNode::Pointer string_node, ParseNode* node)
    {
      string_node->SetName (node->Tag ());

      string_node->ReserveAttributes (node->AttributesCount ());

      for (size_t i = 0; i < node->AttributesCount (); i++)
        string_node->AddAttribute (node->Attribute (i));

      size_t children_count = 0;

      for (ParseNode* iter = node->First (); iter; iter = iter->Next ())
        children_count++;

      string_node->ReserveChildren (children_count);

      for (ParseNode* iter = node->First (); iter; iter = iter->Next ())
      {
        if (!xstrcmp ("#text", iter->Tag ()))
        {
          for (size_t i = 0; i < iter->AttributesCount (); i++)
            string_node->AddAttribute (iter->Attribute (i));
        
          continue;
        }

        StringNode::Pointer child = StringNode::Create ();

        ProcessNode (child, iter);

        string_node->AddChild (child.get ());
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
      
      if (attribute_offsets.size () > 1)
      {
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

        AttributeIterator start (&attribute_offsets [0], attributes.c_str ()),
                          finish (&*attribute_offsets.end (), attributes.c_str ());

        writer.WriteData (xtl::make_iterator_range (start, finish));

        return;
      }

      for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
        (*iter)->SaveNode (writer);
    }

    bool HasNode (StringNode* node)
    {
      if (this == node)
        return true;

      for (ChildArray::iterator iter = childs.begin (), end = childs.end (); iter != end; ++iter)
        if ((*iter)->HasNode (node))
          return true;

      return false;
    }

    Pointer CreateNode (const char* node_name)
    {
      const char* subname = strchr (node_name, '.');

      if (subname)
      {
        Pointer new_child = Create ();
      
        new_child->name = stl::string (node_name, subname - node_name);

        AddChild (new_child.get ());

        return new_child->CreateNode (subname + 1);
      }
      else
      {
        Pointer new_child = Create ();
      
        new_child->name = node_name;

        AddChild (new_child.get ());

        return new_child;
      }
    }

  private:
    typedef stl::vector<size_t>  AttributeOffsetArray;
    typedef stl::vector<Pointer> ChildArray;

  private:
    stl::string          name;                 //имя узла
    stl::string          attributes;           //значения атрибутов
    AttributeOffsetArray attribute_offsets;    //массив смещений атрибутов
    ChildArray           childs;               //дети
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск дочернего узла по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
StringNode::Pointer find (StringNode* node, const char* name)
{
  return node->FindNode (name, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение значения нулевого атрибута дочернего узла с именем name
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get (StringNode* node, const char* name)
{
  StringNode::Pointer find_node = node->FindNode (name, false);

  if (!find_node)
    throw xtl::format_operation_exception ("script::binds::get (StringNode, const char*)", "No such node");

  if (!find_node->AttributesCount ())
    throw xtl::format_operation_exception ("script::binds::get (StringNode, const char*)", "Node hasn't attributes");

  return find_node->Attribute (0);
}

const char* get (StringNode* node, const char* name, const char* default_value)
{
  StringNode::Pointer find_node = node->FindNode (name, false);

  if (!find_node)
    return default_value;

  if (!find_node->AttributesCount ())
    return default_value;

  return find_node->Attribute (0);
}

void set (StringNode* node, const char* name, const char* value)
{
  StringNode::Pointer find_node = node->FindNode (name, true);

  if (find_node->AttributesCount ())
    find_node->SetAttribute (0, value);
  else
    find_node->AddAttribute (value);
}

}

namespace script
{

namespace binds
{

/*
    Регистрация библиотеки работы с деревьями строк
*/

void bind_common_string_tree (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_STRING_TREE_LIBRARY);

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
  lib.Register ("AddChild",            make_invoker (make_invoker (implicit_cast<void (StringNode::*) (size_t, StringNode*)> (&StringNode::AddChild)),
                                                     make_invoker (implicit_cast<size_t (StringNode::*) (StringNode*)> (&StringNode::AddChild))));
  lib.Register ("RemoveChild",         make_invoker (&StringNode::RemoveChild));

  lib.Register ("RemoveAllChildren",   make_invoker (&StringNode::RemoveAllChildren));
  lib.Register ("LoadXml",             make_invoker (&StringNode::LoadXml));
  lib.Register ("SaveXml",             make_invoker (&StringNode::SaveXml));
  lib.Register ("Find",                make_invoker (&find));
  lib.Register ("Get",                 make_invoker (make_invoker(implicit_cast<const char* (*) (StringNode*, const char*, const char*)> (&get)),
                                                     make_invoker(implicit_cast<const char* (*) (StringNode*, const char*)> (&get))));
  lib.Register ("Set",                 make_invoker (&set));

    //регистрация типов данных

  environment.RegisterType<StringNode> (COMMON_STRING_TREE_LIBRARY);
}

}

}

