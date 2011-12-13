#include "shared.h"

using namespace common;

namespace
{

/*
    ”тилиты
*/

//получение реализации узла
ParseNodeImpl* get_node_impl (ParseTree* tree, size_t offset)
{
  return reinterpret_cast<ParseNodeImpl*> (reinterpret_cast<char*> (tree->Buffer ()) + offset);
}

//получение смещени€ узла в дереве
size_t get_node_offset (ParseTree* tree, ParseNodeImpl* node)
{
  return reinterpret_cast<char*> (node) - reinterpret_cast<char*> (tree->Buffer ());
}

//им€ узла
const char* get_node_name (ParseNodeImpl* node)
{
  return reinterpret_cast<const char*> (node) + sizeof (ParseNodeImpl);
}

//первый потомок
ParseNodeImpl* get_first_child (ParseNodeImpl* node)
{
  return node->first_offset ? reinterpret_cast<ParseNodeImpl*> (reinterpret_cast<char*> (node) + node->first_offset) : 0;
}

//следующий узел в дереве
ParseNodeImpl* get_next_node (ParseNodeImpl* node)
{
  return node->next_offset ? reinterpret_cast<ParseNodeImpl*> (reinterpret_cast<char*> (node) + node->next_offset) : 0;
}

//следующий узел с указанным именем в дереве
ParseNodeImpl* get_next_node (ParseNodeImpl* iter, const char* name)
{
  if (!iter)
    return 0;

  const char* subname = strchr (name, '.');

  if (subname)
  {
    size_t base_name_len = subname - name,
           name_hash     = crc32 (name, base_name_len);

    for (; iter; iter=get_next_node (iter))
      if (name_hash == iter->name_hash && !strncmp (name, get_node_name (iter), base_name_len))
        return get_next_node (get_first_child (iter), subname + 1);
  }
  else
  {
    size_t name_hash = strhash (name);

    for (; iter; iter=get_next_node (iter))
      if (name_hash == iter->name_hash && !strcmp (name, get_node_name (iter)))
        return iter;
  }

  return 0;
}

//получение таблицы указателей на атрибуты с компил€цией при необходимости
const char** get_attributes (ParseNodeImpl* node, ParseTree* tree)
{
  const char** attributes = reinterpret_cast<const char**> (reinterpret_cast<char*> (node) + node->attributes_offset);

  if (node->flags & ParseNodeFlag_AttributesAsPointers)
    return attributes;

  const char **attr = attributes,
              *base = reinterpret_cast<const char*> (tree->Buffer ()) + tree->BufferSize ();

  for (size_t count=node->attributes_count; count--; attr++)
    *attr = base - *reinterpret_cast<size_t*> (attr);

  node->flags |= ParseNodeFlag_AttributesAsPointers;

  return attributes;
}

//ƒерево дл€ работы ParseNode::ParseNode ()
struct NullTreeHolder
{
  xtl::intrusive_ptr<ParseTree> tree;

  NullTreeHolder ()
  {
    static const char* NODE_NAME      = "";
    static size_t      NODE_NAME_SIZE = strlen (NODE_NAME) + 1;

    ParseTreeBuffer buffer (sizeof (ParseNodeImpl) + get_aligned_size (NODE_NAME_SIZE, ALIGN_SIZE));

    ParseNodeImpl* node = reinterpret_cast<ParseNodeImpl*> (buffer.data ());

    node->attributes_count   = 0;
    node->name_hash          = strhash (NODE_NAME);
    node->attributes_offset  = 0;
    node->flags              = 0;
    node->next_offset        = 0;
    node->first_offset       = 0;
    node->source_back_offset = 0;
    node->line_number        = 0;

    char* name = buffer.data () + sizeof (ParseNodeImpl);

    strcpy (name, NODE_NAME);

    tree = xtl::intrusive_ptr<ParseTree> (new ParseTree (buffer), false);
  }
};

}

/*
     онструктор / деструктор / присваивание
*/

ParseNode::ParseNode ()
{
  typedef common::Singleton<NullTreeHolder> NullTreeHolderSingleton;
  
  NullTreeHolderSingleton::Instance instance;

  tree   = instance->tree.get ();
  offset = 0;

  addref (tree);
}

ParseNode::ParseNode (ParseTree* in_tree, size_t in_offset)
  : tree (in_tree),
    offset (in_offset)
{
  addref (tree);
}

ParseNode::ParseNode (const ParseNode& node)
  : tree (node.tree),
    offset (node.offset)
{
  addref (tree);
}

ParseNode::~ParseNode ()
{
  release (tree);
}

ParseNode& ParseNode::operator = (const ParseNode& node)
{
  ParseNode (node).Swap (*this);

  return *this;
}

/*
    ѕроверка на пустоту
*/

bool ParseNode::IsEmpty () const
{
  return offset == 0;
}

bool ParseNode::operator ! () const
{
  return IsEmpty ();
}

ParseNode::operator UnspecifiedBoolType () const
{
  return IsEmpty () ? 0 : &ParseNode::IsEmpty;
}

/*
    »м€ узла
*/

const char* ParseNode::Name () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return get_node_name (node);
}

/*
    –абота с атрибутами
*/

size_t ParseNode::AttributesCount () const
{
  return get_node_impl (tree, offset)->attributes_count;
}

const char* ParseNode::Attribute (size_t index) const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  if (index >= node->attributes_count)
    throw xtl::make_range_exception ("common::ParseNode::Attribute", "index", index, node->attributes_count);

  return get_attributes (node, tree) [index];
}

const char** ParseNode::Attributes () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return get_attributes (node, tree);
}

/*
    ѕеребор вложенных узлов
*/

//первый потомок
ParseNode ParseNode::First () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return ParseNode (tree, node->first_offset ? offset + node->first_offset : 0);
}

//следующий узел на данном уровне иерархии
ParseNode ParseNode::Next () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return ParseNode (tree, node->next_offset ? offset + node->next_offset : 0);
}

/*
    ѕеребор узлов по имени
*/

//первый потомок с указанным именем
ParseNode ParseNode::First (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("common::ParseNode::First(const char*)", "name");

  if (!*name)
    return *this;

  ParseNodeImpl* node = get_next_node (::get_first_child (get_node_impl (tree, offset)), name);

  if (node)
    return ParseNode (tree, get_node_offset (tree, node));

  return ParseNode (tree, 0);
}

//следующий узел на данном уровне иерархии с указанным именем
ParseNode ParseNode::Next (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("common::ParseNode::Next(const char*)", "name");

  if (!*name)
    return *this;

  ParseNodeImpl* node = get_next_node (get_next_node (get_node_impl (tree, offset)), name);

  if (node)
    return ParseNode (tree, get_node_offset (tree, node));

  return ParseNode (tree, 0);
}

//следующий "тЄзка" на данном уровне иерархии
ParseNode ParseNode::NextNamesake () const
{
  ParseNodeImpl* node      = get_node_impl (tree, offset);
  size_t         name_hash = node->name_hash;
  const char*    name      = Name ();

  for (ParseNodeImpl* iter=get_next_node (node); iter; iter=get_next_node (iter))
    if (name_hash == iter->name_hash && !strcmp (get_node_name (iter), name))
      return ParseNode (tree, get_node_offset (tree, iter));

  return ParseNode (tree, 0);
}

/*
    »нформаци€ о расположении узла
*/

//им€ исходного текста,  в котором расположен узел
const char* ParseNode::Source () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return node->source_back_offset ? reinterpret_cast<const char*> (node) - node->source_back_offset : "";
}

//номер строки
size_t ParseNode::LineNumber () const
{
  ParseNodeImpl* node = get_node_impl (tree, offset);

  return node->line_number;
}

/*
    ѕротокол разбора
*/

ParseLog ParseNode::Log () const
{
  return tree->Log ();
}

/*
    ќбмен
*/

void ParseNode::Swap (ParseNode& node)
{
  stl::swap (tree,   node.tree);
  stl::swap (offset, node.offset);
}

namespace common
{

void swap (ParseNode& node1, ParseNode& node2)
{
  node1.Swap (node2);
}

}
