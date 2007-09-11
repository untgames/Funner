/*
    ParseIterator
*/

template <class NextFun>
inline ParseIterator<NextFun>::ParseIterator ()
  : node (0)
  {}

template <class NextFun>
inline ParseIterator<NextFun>::ParseIterator (ParseNode* in_node)
  : node (in_node)
  {}

template <class NextFun> template <class NextFun1>
inline ParseIterator<NextFun>::ParseIterator (const ParseIterator<NextFun1>& i)
  : node (&*i)
  { }

template <class NextFun>
inline ParseIterator<NextFun>& ParseIterator<NextFun>::operator = (ParseNode* in_node)
{
  node = in_node;

  return *this;
}

template <class NextFun> template <class NextFun1>
inline ParseIterator<NextFun>& ParseIterator<NextFun>::operator = (const ParseIterator<NextFun1>& i)
{
  node = &*i;

  return *this;
}

template <class NextFun>
inline ParseIterator<NextFun>& ParseIterator<NextFun>::operator ++ ()
{
  node = NextFun ()(node);

  return *this;
}

template <class NextFun>
inline ParseIterator<NextFun> ParseIterator<NextFun>::operator ++ (int)
{
  ParseIterator tmp = *this;

  node = NextFun ()(node);

  return tmp;
}

template <class NextFun>
inline ParseNode* ParseIterator<NextFun>::operator -> () const
{
  return node;
}

template <class NextFun>
inline ParseNode& ParseIterator<NextFun>::operator * () const
{
  return *node;
}

/*
    Parser
*/

inline ParseNode* Parser::NextNodeFun::operator () (ParseNode* node) const
{
  return node ? node->Next () : 0;
}

inline ParseNode* Parser::NextNamesakeNodeFun::operator () (ParseNode* node) const
{
  return node ? node->NextNamesake () : 0;
}

/*
    Получение итератора атрибутов
*/

inline Parser::AttributeIterator make_attribute_iterator (ParseNode* node)
{
  if (!node)
    return Parser::AttributeIterator ();

  const char **first = node->Attributes (), **last = first + node->AttributesCount ();

  return Parser::AttributeIterator (first, last);
}

inline Parser::AttributeIterator make_attribute_iterator (ParseNode* node, const char* tag)
{
  if (node && tag)
    node = node->First (tag);

  return make_attribute_iterator (node);
}

/*
    Чтение атрибутов    
*/

template <class T>
inline bool read (ParseNode* node, const char* tag, T& value)
{
  if (node && tag)
    node = node->First (tag);
  
  return node && read (make_attribute_iterator (node), value);
}

template <class T>
inline void read (ParseNode* node, const char* tag, T& value, const T& default_value)
{
  if (node && tag)
    node = node->First (tag);
  
  if (!node || !read (make_attribute_iterator (node), value))
    value = default_value;
}

template <class Traits,class Allocator>
inline void read (ParseNode* node, const char* tag, stl::basic_string<char, Traits, Allocator>& string, const char* default_value)
{
  if (node && tag)
    node = node->First (tag);

  if (!node || !read (make_attribute_iterator (node), string))
    string = default_value ? default_value : "";
}

template <class T>
inline T get (ParseNode* node, const char* tag, const T& default_value)
{
  if (node && tag)
    node = node->First (tag);

  T value;

  return node && read (make_attribute_iterator (node), value) ? value : default_value;
}

template <class T>
inline T get (ParseNode* node, const char* tag, size_t start, const T& default_value)
{
  if (node && tag)
    node = node->First (tag);

  T value;

  return node && read (make_attribute_iterator (node) += start, value) ? value : default_value;
}

/*
    Чтение интервалов
*/

template <class OutIter>
inline size_t read_range (ParseNode* node, const char* tag, OutIter first, size_t count)
{
  if (node && tag)
    node = node->First (tag);

  return node ? read_range (make_attribute_iterator (node), first, count) : 0;
}

template <class OutIter>
inline size_t read_range (ParseNode* node, const char* tag, OutIter first, size_t count, size_t start, size_t step)
{
  if (node && tag)
    node = node->First (tag);
    
  if (!node)
    return 0;
    
  Parser::AttributeIterator iter = make_attribute_iterator (node);
  
  iter += start;

  if (!iter)
    return 0;

  return read_range (iter, first, count, step);
}

/*
    Тестирование узлов и атрибутов
*/

inline bool test_tag (ParseNode* node, const char* tag)
{
  return node && tag && strcmp (node->Tag (), tag) == 0;
}

inline bool test (ParseNode* node, const char* tag)
{
  return node && tag && node->First (tag);
}

template <class T> 
inline bool test (ParseNode* node, const char* tag, const T& value)
{
  T tmp;
  
  if (node && tag)
    node = node->First (tag);  
  
  if (!node || !read (make_attribute_iterator (node), tmp))
    return false;

  return tmp == value;
}

template <class T> 
inline bool test (ParseNode* node, const char* tag, size_t start, const T& value)
{
  T tmp;
  
  if (node && tag)
    node = node->First (tag);  
  
  if (!node || !read (make_attribute_iterator (node) += start, tmp))
    return false;

  return tmp == value;
}

inline bool test (ParseNode* node, const char* tag, const char* value)
{
  const char* s = get<const char*> (node, tag); 

  return s && strcmp (s, value) == 0;
}

inline bool test (ParseNode* node, const char* tag, size_t start, const char* value)
{
  const char* s = get<const char*> (node, tag, start);

  return s && strcmp (s, value) == 0;
}

/*
    Применение функционала ко всем вложенным узлам
*/

template <class Fn>
inline void for_each_child (ParseNode* node, Fn fn)
{
  if (!node)
    return;

  for (ParseNode* i=node->First (); i; i=i->Next ())
    fn (i);
}

template <class Fn>
inline void for_each_child (ParseNode* node, const char* tag, Fn fn)
{
  if (!node)
    return;

  for (ParseNode* i=node->First (tag); i; i=i->NextNamesake ())
    fn (i);
}

/*
    Вызов функтора, если существует вложенный узел с указанным именем
*/

template <class Fn>
inline void parse_if (ParseNode* node, const char* tag, Fn then_fn)
{
  if (tag && node)
    node = node->First (tag);
    
  if (node)   
    then_fn (node);
}

template <class Fn1, class Fn2>
inline void parse_if (ParseNode*, const char* tag, Fn1 then_fn, Fn2 else_fn)
{
  if (tag && node)
    node = node->First (tag);
    
  if (node) then_fn (node);
  else      else_fn (node);
}
