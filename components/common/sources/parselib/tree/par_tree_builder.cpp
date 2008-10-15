#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t MIN_PARSE_TREE_BUFFER_SIZE = 4096;               //минимальный размер буфера дерева грамматического разбора
const size_t MAX_PARSE_TREE_BUFFER_SIZE = 1024 * 1024 * 1024; //максимальный размер буфера дерева грамматического разбора
const size_t NODES_STACK_RESERVE_SIZE   = 32;                 //резервируемый размер стека вложений узлов грамматического разбора
const char*  DUMMY_NODE_NAME            = "";                 //имя пустого узла
const char*  ROOT_NODE_NAME             = "root";             //имя корневого узла

/*
    Элемент стека узлов
*/

struct NodeStackItem
{
  size_t node_offset;       //смещение узла
  size_t last_child_offset; //смещение последнего ребёнка
  
  NodeStackItem (size_t in_node_offset) : node_offset (in_node_offset), last_child_offset (0) {}
};

}

/*
    Описание реализации ParseTreeBuilder
*/

typedef stl::vector<NodeStackItem> NodeStack;

struct ParseTreeBuilder::Impl
{
  char*           front_offset;                  //текущая позиция выделения памяти в передней части буфера
  char*           back_offset;                   //текущая позиция выделения памяти в задней части буфера
  ParseTreeBuffer buffer;                        //буфер дерева разбора
  NodeStack       nodes_stack;                   //стек смещений кадров
  size_t          total_nodes_count;             //общее количество узлов
  size_t          total_attributes_count;        //общее количество атрибутов
  size_t          current_source_offset;         //смещение до имени текущего исходного текста
  size_t          current_line_number;           //номер текущей строки в исходном тексте
  size_t*         current_node_attributes_count; //текущее количество атрибутов в узле

///Конструктор
  Impl ()
  {
    front_offset                  = 0;
    back_offset                   = 0;
    total_nodes_count             = 0;
    total_attributes_count        = 0;
    current_source_offset         = 0;
    current_line_number           = 0;
    current_node_attributes_count = 0;
    
    nodes_stack.reserve (NODES_STACK_RESERVE_SIZE);
  }

///Копирование
  Impl (const Impl& impl)
    : nodes_stack (impl.nodes_stack)
  {
    nodes_stack.reserve (NODES_STACK_RESERVE_SIZE);
    
    impl.CopyBufferTo (*this, impl.buffer.size ());

    total_nodes_count      = impl.total_nodes_count;
    total_attributes_count = impl.total_attributes_count;
    current_source_offset  = impl.current_source_offset;
    current_line_number    = impl.current_line_number;    
  }
  
///Копирование буфера
  void CopyBufferTo (Impl& impl, size_t buffer_size) const
  {
    size_t old_front_offset                 = front_offset - buffer.data (),
           old_back_offset                  = buffer.data () + buffer.size () - back_offset,
           old_node_attributes_count_offset = current_node_attributes_count ? reinterpret_cast<char*> (current_node_attributes_count) - buffer.data () : 0;
           
    if (buffer_size < old_front_offset + old_back_offset)
      buffer_size = old_front_offset + old_back_offset;

    ParseTreeBuffer new_buffer (buffer_size);

    impl.front_offset                  = new_buffer.data () + old_front_offset;
    impl.back_offset                   = new_buffer.data () + new_buffer.size () - old_back_offset;
    impl.current_node_attributes_count = old_node_attributes_count_offset ? reinterpret_cast<size_t*> (new_buffer.data () + old_node_attributes_count_offset) : 0;

    memcpy (new_buffer.data (), buffer.data (), old_front_offset);
    memcpy (impl.back_offset, buffer.data () + buffer.size () - old_back_offset, old_back_offset);

    impl.buffer.swap (new_buffer);    
  }

///Заказ памяти в буфере
  void Alloc (size_t new_size)
  {
    if (size_t (back_offset - front_offset) > new_size)
      return;

    Reserve (buffer.size () + new_size);
  }

///Резервирование места в буфере
  void Reserve (size_t new_size)
  {
    if (buffer.size () >= new_size)
      return;

    if (new_size < MIN_PARSE_TREE_BUFFER_SIZE)
      new_size = MIN_PARSE_TREE_BUFFER_SIZE;

    if (new_size > MAX_PARSE_TREE_BUFFER_SIZE)
      throw xtl::format_operation_exception ("common::ParseTreeBuilder::Impl::Reserve", "Parse buffer too large (requested_size=%u, max_size=%u)",
        new_size, buffer.size ());

    if (new_size / 2 < buffer.size ())
    {
      if (buffer.size () > MAX_PARSE_TREE_BUFFER_SIZE / 2)
      {
        new_size = MAX_PARSE_TREE_BUFFER_SIZE;
      }
      else
      {
        new_size = buffer.size () * 2;
      }
    }

    CopyBufferTo (*this, new_size);
  }    
  
///Резервирование места в передней части буфера
  char* AllocFront (size_t size)
  {
    Alloc (size);

    char* res = front_offset;

    front_offset += size;

    return res;
  }
  
///Резервирование места в задней части буфера
  char* AllocBack (size_t size)
  {
    Alloc (size);

    back_offset -= size;

    return back_offset;
  }  
};

/*
    Конструктор / деструктор / присваивание
*/

ParseTreeBuilder::ParseTreeBuilder ()
  : impl (new Impl)
{
    //создание пустого узла
   
  BeginNode (DUMMY_NODE_NAME);
  EndNode   ();
  
    //создание корневого узла

  impl->nodes_stack.clear ();
  impl->total_nodes_count = 0;

  BeginNode (ROOT_NODE_NAME);
}

ParseTreeBuilder::ParseTreeBuilder (const ParseTreeBuilder& builder)
  : impl (new Impl (*builder.impl))
{
}

ParseTreeBuilder::~ParseTreeBuilder ()
{
}

ParseTreeBuilder& ParseTreeBuilder::operator = (const ParseTreeBuilder& builder)
{
  ParseTreeBuilder (builder).Swap (*this);
  
  return *this;
}

/*
    Информация о контексте
*/

//имя текущего узла
const char* ParseTreeBuilder::NodeName () const
{
  if (!impl->buffer.data () || impl->nodes_stack.empty ())
    return "";

  return impl->buffer.data () + impl->nodes_stack.back ().node_offset + sizeof ParseNodeImpl;
}

//уровень вложенности узлов (корень на нулевом уровне)
size_t ParseTreeBuilder::NodeDepth () const
{
  return impl->nodes_stack.size () - 1;
}

//общее количество узлов
size_t ParseTreeBuilder::TotalNodesCount () const
{
  return impl->total_nodes_count;
}

//общее количество атрибутов
size_t ParseTreeBuilder::TotalAttributesCount () const
{
  return impl->total_attributes_count;
}

//размер буфера разбора
size_t ParseTreeBuilder::BufferSize () const
{
  return impl->buffer.size ();
}

/*
    Имя текущего разбираемого исходного текста
*/

void ParseTreeBuilder::SetSource (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("common::ParseTreeBuilder::SetSource", "name");

  size_t size   = strlen (name) + 1,
         offset = impl->front_offset - impl->buffer.data ();
  char*  dst    = impl->AllocFront (size);

  strcpy (dst, name);

  impl->current_source_offset = offset;
  impl->current_line_number   = 0;
}

const char* ParseTreeBuilder::Source () const
{
  return impl->current_source_offset ? impl->buffer.data () + impl->current_source_offset : "";
}

/*
    Определение границ узла
*/

void ParseTreeBuilder::BeginNode (const char* name)
{
  BeginNode (name, impl->current_line_number);
}

void ParseTreeBuilder::BeginNode (const char* name, size_t line_number)
{
    //проверка корректности аргументов

  if (!name)
    throw xtl::make_null_argument_exception ("common::ParseTreeBuilder::BeginNode(const char*, size_t)", "name");
    
    //резервирование памяти для расположения узла

  size_t name_size   = strlen (name) + 1,
         node_offset = impl->front_offset - impl->buffer.data (),
         node_size   = sizeof ParseNodeImpl + name_size;

  ParseNodeImpl* node = reinterpret_cast<ParseNodeImpl*> (impl->AllocFront (node_size));
  
    //инициализация узла

  node->attributes_count              = 0;
  node->name_hash                     = strhash (name);
  node->attributes_offset             = node_size;
  node->flags                         = 0;
  node->next_offset                   = 0;
  node->first_offset                  = 0;
  node->source_back_offset            = reinterpret_cast<char*> (node) - impl->buffer.data () - impl->current_source_offset;
  node->line_number                   = line_number;
  impl->current_line_number           = line_number;
  impl->current_node_attributes_count = &node->attributes_count;
  
  strcpy (reinterpret_cast<char*> (node) + sizeof ParseNodeImpl, name);  
  
    //обновление ссылок на узел
    
  impl->nodes_stack.push_back (NodeStackItem (node_offset));

  if (impl->nodes_stack.size () > 1)
  {
    NodeStackItem& parent = impl->nodes_stack.end ()[-2];

    if (parent.last_child_offset)
    {
      ParseNodeImpl* prev_node = reinterpret_cast<ParseNodeImpl*> (impl->buffer.data () + parent.last_child_offset);

      prev_node->next_offset = node_offset - parent.last_child_offset;
    }
    else
    {      
      ParseNodeImpl* parent_node = reinterpret_cast<ParseNodeImpl*> (impl->buffer.data () + parent.node_offset);

      parent_node->first_offset = node_offset - parent.node_offset;
    }

    parent.last_child_offset = node_offset;
  }
  
    //увеличение числа узлов
    
  impl->total_nodes_count++;
}

void ParseTreeBuilder::EndNode ()
{
  impl->current_node_attributes_count = 0;

  if (impl->nodes_stack.size () <= 1)
    return;

  impl->nodes_stack.pop_back ();
}

/*
    Добавление атрибутов
*/

void ParseTreeBuilder::AddAttribute (const char* value)
{
  AddAttributes (1, &value);
}

void ParseTreeBuilder::AddAttributes (size_t count, const char** values)
{
  static const char* METHOD_NAME = "common::ParseTreeBuilder::AddAttributes";
  
    //проверка корректности аргументов и подсчёт количества памяти, необходимого для хранения значений атрибутов
    
  if (!impl->current_node_attributes_count)
    throw xtl::format_operation_exception (METHOD_NAME, "Attributes section closed");

  if (!values)
    throw xtl::make_null_argument_exception (METHOD_NAME, "values");      

  size_t attribute_values_size = 0;  
  
  for (size_t i=0; i<count; i++)
  {
    const char* value = values [i];
    
    if (!value)
      throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "Null argument 'values[%u]'", i);

    attribute_values_size += strlen (value) + 1;
  }

    //резервирование памяти

  impl->Alloc (attribute_values_size + count * sizeof (size_t));

  char*   dst_value  = impl->AllocBack (attribute_values_size);
  size_t* dst_offset = reinterpret_cast<size_t*> (impl->AllocFront (count * sizeof (size_t)));  
  
  dst_value += attribute_values_size;

    //копирование атрибутов в буфер    

  for (size_t i=0; i<count; i++, dst_offset++)
  {
    const char* value = values [i];

    dst_value -= strlen (value) + 1;

    strcpy (dst_value, value);

    *dst_offset = impl->buffer.data () + impl->buffer.size () - dst_value;
  }  

    //увеличение числа атрибутов

  *impl->current_node_attributes_count += count;
  impl->total_attributes_count         += count;
}

/*
    Резервирование буфера разбора
*/

void ParseTreeBuilder::ReserveBuffer (size_t size)
{
  impl->Reserve (size);
}

/*
    Завершение построения
*/

ParseNode ParseTreeBuilder::BuildCore (const ParseLog* log)
{
  ParseTreeBuilder new_builder;

  xtl::intrusive_ptr<ParseTree> tree (new ParseTree (impl->buffer, log), false);

  ParseNode node (tree.get (), sizeof (ParseNodeImpl) + strlen (DUMMY_NODE_NAME) + 1);

  new_builder.Swap (*this);

  return node;
}

ParseNode ParseTreeBuilder::Build ()
{
  return BuildCore (0);
}

ParseNode ParseTreeBuilder::Build (const ParseLog& log)
{
  return BuildCore (&log);
}

/*
    Обмен
*/

void ParseTreeBuilder::Swap (ParseTreeBuilder& builder)
{
  stl::swap (impl, builder.impl);
}

namespace
{

void swap (ParseTreeBuilder& builder1, ParseTreeBuilder& builder2)
{
  builder1.Swap (builder2);
}

}
