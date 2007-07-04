/*
    BasicAttributeReader
*/

inline BasicAttributeReader::BasicAttributeReader (size_t attributes_count,const char** attributes)
  : start (attributes), pos (attributes), finish (attributes+attributes_count)
  { }

inline const char* BasicAttributeReader::GetAttribute () const
{
  return pos != finish ? *pos : NULL;
}

inline const char** BasicAttributeReader::GetAttributeList () const
{
  return pos != finish ? pos : NULL;
}

inline size_t BasicAttributeReader::Available () const
{
  return finish - pos;
}

inline bool BasicAttributeReader::IsEmpty () const
{
  return pos == finish;
}

inline size_t BasicAttributeReader::Tell () const
{
  return pos - start;
}

inline bool BasicAttributeReader::Seek (size_t position)
{
  if (position > (size_t)(finish - start))
    return false;

  pos = start + position;

  return true;
}

inline bool BasicAttributeReader::Skip (size_t attributes_count)
{
  if (attributes_count > (size_t)(finish - pos))
    return false;

  pos += attributes_count;

  return true;
}

inline void BasicAttributeReader::Reset ()
{
  pos = start;
}

inline bool BasicAttributeReader::Read (bool& value)
{
  long int_value = 0;    

  if (!Read (int_value))
    return false;

  value = int_value != 0;

  return true;
}

template <class T>
inline bool __ReadIntAttribute (BasicAttributeReader& parser,T& value)
{
  long int_value = 0;

  if (!parser.Read (int_value))
    return false;

  value = (T)int_value;

  return true;
}

template <class T>
inline bool __ReadUIntAttribute (BasicAttributeReader& parser,T& value)
{
  unsigned long int_value = 0;

  if (!parser.Read (int_value))
    return false;

  value = (T)int_value;

  return true;
}

inline bool BasicAttributeReader::Read (char& value)
{
  return __ReadIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (unsigned char& value)
{
  return __ReadUIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (short& value)
{
  return __ReadIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (unsigned short& value)
{
  return __ReadUIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (int& value)
{
  return __ReadIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (unsigned int& value)
{
  return __ReadUIntAttribute (*this,value);
}

inline bool BasicAttributeReader::Read (long& value)
{
  if (IsEmpty ())
    return false;

  const char* src = *pos++;

  value = strtol (src,(char**)&src,0);

  if (*src == '\0')
    return true;

  if (*src != '.')
    return false;

  while (*++src)
    if (!isdigit (*src))
      return false;
      
  return true;
}

inline bool BasicAttributeReader::Read (unsigned long& value)
{
  return Read (*(long*)&value);
}

inline bool BasicAttributeReader::Read (float& value)
{
  if (IsEmpty ())
    return false;
    
  const char* src = *pos++;  

  value = (float)strtod (src,(char**)&src);

  return *src == '\0';
}

inline bool BasicAttributeReader::Read (double& value)
{
  if (IsEmpty ())
    return false;
    
  const char* src = *pos++;  

  value = strtod (src,(char**)&src);

  return *src == '\0';
}

inline bool BasicAttributeReader::Read (long double& value)
{
  if (IsEmpty ())
    return false;

  const char* src = *pos++;  

  value = (long double)strtod (src,(char**)&src);

  return *src == '\0';
}

inline bool BasicAttributeReader::Read (const char*& string_ptr)
{
  if (IsEmpty ())
    return false;
  
  string_ptr = *pos++;
  
  return true;
}

template <class Traits,class Allocator>
inline bool BasicAttributeReader::Read (stl::basic_string<char,Traits,Allocator>& string)
{
  if (IsEmpty ())
    return false;
    
  string = *pos++;
  
  return true;
}

template <class T,size_t size> 
inline bool BasicAttributeReader::Read (math::vec<T,size>& vector)
{
  size_t position = Tell ();
    
  for (size_t i=0;i<size;i++) 
    if (!Read (vector [i]))
    {
      Seek (position);
      return false;
    }  

  return true;
}

template <class T,size_t size> 
inline bool BasicAttributeReader::Read (math::matrix<T,size>& matrix)
{       
  size_t position = Tell ();

  for (size_t i=0;i<size;i++) 
    for (size_t j=0;j<size;j++)   
      if (!Read (matrix [i][j]))
      {
        Seek (position);
        return false;
      }  

  return true;
}

template <class T>
inline bool BasicAttributeReader::Read (math::quat<T>& quat)
{
  size_t position = Tell ();

  for (size_t i=0;i<4;i++) 
    if (!Read (quat [i]))
    {
      Seek (position);
      return false;
    }  

  return true;
}

/*
    ParseAttributeReader
*/

template <class AttrReader>
inline ParseAttributeReader<AttrReader>::ParseAttributeReader (size_t attributes_count,const char** attributes)
  : BaseAttributeReader (attributes_count,attributes)
  { }

template <class AttrReader> template <class T>
inline void ParseAttributeReader<AttrReader>::Read (T& object)
{
  if (!BaseAttributeReader::Read (object))
    object = T ();
}

template <class AttrReader> template <class T>
inline void ParseAttributeReader<AttrReader>::Read (T& object,const T& default_value)
{
  if (!BaseAttributeReader::Read (object))
    object = default_value;
}

template <class AttrReader>
inline const char* ParseAttributeReader<AttrReader>::ReadString (const char* default_value)
{
  const char* value;
  
  return BaseAttributeReader::Read (value) ? value : default_value;
}

template <class AttrReader> template <class OutIter> 
bool ParseAttributeReader<AttrReader>::ReadIter (OutIter& iter)
{
  return BaseAttributeReader::Read (*iter);
}

template <class AttrReader> template <class T> 
bool ParseAttributeReader<AttrReader>::ReadIter (T* ptr)
{ 
  return BaseAttributeReader::Read (*ptr);
}

template <class AttrReader> template <class Container>
bool ParseAttributeReader<AttrReader>::ReadIter (stl::back_insert_iterator<Container>& iter)
{
  typename Container::value_type object;

  if (!BaseAttributeReader::Read (object))
    return false;

  *iter = object;
   
  return true;
}

template <class AttrReader> template <class Container>
bool ParseAttributeReader<AttrReader>::ReadIter (stl::front_insert_iterator<Container>& iter)
{
  typename Container::value_type object;

  if (!BaseAttributeReader::Read (object))
    return false;

  *iter = object;
   
  return true;
}

template <class AttrReader> template <class Container>
bool ParseAttributeReader<AttrReader>::ReadIter (stl::insert_iterator<Container>& iter)
{
  typename Container::value_type object;

  if (!BaseAttributeReader::Read (object))
    return false;

  *iter = object;
   
  return true;
}

template <class AttrReader> template <class OutIter> 
inline size_t ParseAttributeReader<AttrReader>::ReadArray (OutIter first)
{
  return ReadArray (first,(size_t)-1);
}

template <class AttrReader> template <class OutIter> 
inline size_t ParseAttributeReader<AttrReader>::ReadArray (OutIter first,size_t count)
{
  size_t read_count = 0;
  
  for (;count-- && ReadIter (first);++first,read_count++);

  return read_count;  
}

template <class AttrReader> template <class OutIter> 
inline size_t ParseAttributeReader<AttrReader>::ReadArray (OutIter first,size_t count,size_t step)
{
  if (!step)
    return ReadArray (first,count);

  size_t read_count = 0,
         position   = BaseAttributeReader::Tell ();
  
  for (;count-- && ReadIter (first);++first)
  {
    read_count++;
    position += step;

    if (!BaseAttributeReader::Seek (position))
      break;
  }

  return read_count;
}

/*
    ParseReadableNode
*/

template <class AttrReader>
inline ParseReadableNode<AttrReader>* ParseReadableNode<AttrReader>::First () const
{
  return (Node*)ParseNode::First ();
}

template <class AttrReader>
inline ParseReadableNode<AttrReader>* ParseReadableNode<AttrReader>::Next () const
{
  return (Node*)ParseNode::Next ();
}

template <class AttrReader>
inline ParseReadableNode<AttrReader>* ParseReadableNode<AttrReader>::First (const char* tag) const
{
  return (Node*)ParseNode::First (tag);
}

template <class AttrReader>
inline ParseReadableNode<AttrReader>* ParseReadableNode<AttrReader>::Next (const char* tag) const
{
  return (Node*)ParseNode::Next (tag);
}

template <class AttrReader>
inline ParseReadableNode<AttrReader>* ParseReadableNode<AttrReader>::NextNamesake () const
{
  return (Node*)ParseNode::NextNamesake ();
}

template <class AttrReader>
inline typename ParseReadableNode<AttrReader>::AttributeReader 
ParseReadableNode<AttrReader>::Reader () const
{
  return AttributeReader (AttributesCount (),Attributes ());
}

template <class AttrReader>
inline typename ParseReadableNode<AttrReader>::AttributeReader 
ParseReadableNode<AttrReader>::Reader (const char* tag) const
{
  const Node* node = tag ? First (tag) : this;

  return node ? node->Reader () : AttributeReader (0,NULL);
}

template <class AttrReader>
inline typename ParseReadableNode<AttrReader>::BaseAttributeReader
ParseReadableNode<AttrReader>::BaseReader () const
{
  return BaseAttributeReader (AttributesCount (),Attributes ());
}

template <class AttrReader>
inline typename ParseReadableNode<AttrReader>::BaseAttributeReader 
ParseReadableNode<AttrReader>::BaseReader (const char* tag) const
{
  const Node* node = tag ? First (tag) : this;

  return node ? node->BaseReader () : BaseAttributeReader (0,NULL);
}

template <class AttrReader> template <class T>
inline void ParseReadableNode<AttrReader>::Read (const char* tag,T& object) const
{
  const Node* node = tag ? First (tag) : this;
  
  if (!node || !node->BaseReader ().Read (object))
    object = T ();
} 

template <class AttrReader> template <class T>
inline void ParseReadableNode<AttrReader>::Read (const char* tag,T& object,const T& default_value) const
{
  const Node* node = tag ? First (tag) : this;
  
  if (!node || !node->BaseReader ().Read (object))
    object = default_value;
}

template <class AttrReader> template <class Traits,class Allocator>
inline void ParseReadableNode<AttrReader>::Read 
 (const char*                               tag,
  stl::basic_string<char,Traits,Allocator>& string,
  const char*                               default_value) const
{
  const Node* node = tag ? First (tag) : this;

  if (!node || !node->BaseReader ().Read (string))
    string = default_value ? default_value : "";
}

template <class AttrReader> template <class OutIter>
inline size_t ParseReadableNode<AttrReader>::ReadArray (const char* tag,OutIter first) const
{
  return ReadArray (tag,first,(size_t)-1);
}

template <class AttrReader> template <class OutIter>
inline size_t ParseReadableNode<AttrReader>::ReadArray (const char* tag,OutIter first,size_t count) const
{
  const Node* node = tag ? First (tag) : this;

  return node ? node->Reader ().ReadArray (first,count) : 0;
}

template <class AttrReader> template <class OutIter>
inline size_t ParseReadableNode<AttrReader>::ReadArray 
 (const char* tag,
  OutIter     first,
  size_t      count,
  size_t      start) const
{
  const Node* node = tag ? First (tag) : this;

  if (!node)
    return 0;

  AttributeReader reader = node->Reader ();

  if (!reader.Skip (start))
    return 0;

  return reader.ReadArray (first,count);
}

template <class AttrReader> template <class OutIter>
inline size_t ParseReadableNode<AttrReader>::ReadArray 
 (const char* tag,
  OutIter     first,
  size_t      count,
  size_t      start,
  size_t      step) const
{
  const Node* node = tag ? First (tag) : this;
  
  if (!node)
    return 0;

  AttributeReader reader = node->Reader ();

  if (!reader.Skip (start))
    return 0;

  return reader.ReadArray (first,count,step);
}

template <class AttrReader> template <class T> 
inline bool ParseReadableNode<AttrReader>::Test (const char* tag,const T& value) const
{
  T tmp;
    
  return BaseReader (tag).Read (tmp) && tmp == value;
}

template <class AttrReader> template <class T> 
inline bool ParseReadableNode<AttrReader>::Test (const char* tag,size_t start,const T& value) const
{
  T tmp;
  
  BaseAttributeReader reader = BaseReader (tag);
  
  return reader.Skip (start) && reader.Read (tmp) && tmp == value;
}

template <class AttrReader>
inline bool ParseReadableNode<AttrReader>::Test (const char* tag,const char* string) const
{
  const char* tmp = NULL;
    
  return BaseReader (tag).Read (tmp) && tmp && !strcmp (tmp,string);
}

template <class AttrReader>
inline bool ParseReadableNode<AttrReader>::Test (const char* tag,size_t start,const char* string) const
{
  const char* tmp = NULL;

  BaseAttributeReader reader = BaseReader (tag);

  return reader.Skip (start) && reader.Read (tmp) && tmp && !strcmp (tmp,string);
}

template <class AttrReader>
inline bool ParseReadableNode<AttrReader>::Present (const char* tag)
{
  return First (tag) != NULL;
}

template <class AttrReader>
inline const char* ParseReadableNode<AttrReader>::ReadString (const char* tag,const char* default_value) const
{
  const char* value;
  
  Read (tag,value,default_value);
  
  return value;
}

template <class AttrReader>
inline int ParseReadableNode<AttrReader>::ReadInt (const char* tag,int default_value) const
{
  int value;
  
  Read (tag,value,default_value);
  
  return value;
}

template <class AttrReader>
inline unsigned int ParseReadableNode<AttrReader>::ReadUInt (const char* tag,unsigned int default_value) const
{
  unsigned int value;
  
  Read (tag,value,default_value);
  
  return value;
}

template <class AttrReader>
inline float ParseReadableNode<AttrReader>::ReadFloat (const char* tag,float default_value) const
{
  float value;
  
  Read (tag,value,default_value);
  
  return value;
}

/*
    ParseIterator
*/

template <class NodeType,class NextFun>
inline ParseIterator<NodeType,NextFun>::ParseIterator ()
  : node (NULL)
  { }

template <class NodeType,class NextFun>
inline ParseIterator<NodeType,NextFun>::ParseIterator (ParseNode* _node)
  : node (_node)
  { }

template <class NodeType,class NextFun> template <class NodeType1,class NextFun1>
inline ParseIterator<NodeType,NextFun>::ParseIterator (const ParseIterator<NodeType1,NextFun1>& i)
{
  node = &*i;
}

template <class NodeType,class NextFun>
inline ParseIterator<NodeType,NextFun>& ParseIterator<NodeType,NextFun>::operator = (ParseNode* _node)
{
  node = _node;
  return *this;
}

template <class NodeType,class NextFun> template <class NodeType1,class NextFun1>
inline ParseIterator<NodeType,NextFun>& 
ParseIterator<NodeType,NextFun>::operator = (const ParseIterator<NodeType1,NextFun1>& i)
{
  node = &*i;
  return *this;
}

template <class NodeType,class NextFun>
inline ParseIterator<NodeType,NextFun>& ParseIterator<NodeType,NextFun>::operator ++ ()
{
  node = NextFun ()(node);
  return *this;
}

template <class NodeType,class NextFun>
inline ParseIterator<NodeType,NextFun> ParseIterator<NodeType,NextFun>::operator ++ (int)
{
  ParseIterator tmp = *this;

  node = NextFun ()(node);

  return tmp;
}

template <class NodeType,class NextFun>
inline NodeType* ParseIterator<NodeType,NextFun>::operator -> () const
{
  return (Node*)node;
}

template <class NodeType,class NextFun>
inline NodeType& ParseIterator<NodeType,NextFun>::operator * () const
{
  return *(Node*)node;
}

/*
    BasicParser
*/

template <class AttrReader>
inline BasicParser<AttrReader>::BasicParser (const char* file_name,const char* format)
  : ParseTree (file_name,format)
  { }

template <class AttrReader>
inline BasicParser<AttrReader>::BasicParser (const char* name,const char* buf,size_t buf_size,const char* format)
  : ParseTree (name,buf,buf_size,format)
  { }
  
template <class AttrReader>
inline BasicParser<AttrReader>::BasicParser (ParseLog& log,const char* file_name,const char* format)
  : ParseTree (log,file_name,format)
  { }

template <class AttrReader>
inline BasicParser<AttrReader>::BasicParser (ParseLog& log,const char* name,const char* buf,size_t buf_size,const char* format)
  : ParseTree (log,name,buf,buf_size,format)
  { }  

template <class AttrReader>
inline typename BasicParser<AttrReader>::Node* BasicParser<AttrReader>::Root () const
{
  return (Node*)ParseTree::Root ();
}

template <class AttrReader>
inline ParseNode* BasicParser<AttrReader>::NextNodeFun::operator () (ParseNode* node) const
{
  return node->Next ();
}

template <class AttrReader>
inline ParseNode* BasicParser<AttrReader>::NextNamesakeNodeFun::operator () (ParseNode* node) const
{
  return node->NextNamesake ();
}
