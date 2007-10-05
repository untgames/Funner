#include <common/hash.h>
#include <common/file.h>
#include <common/strwrap.h>
#include "context.h"
#include "dispatch.h"

using namespace common;

const size_t START_ATTR_BUF_SIZE = 4096;           //начальный размер буфера разбора
const size_t MAX_ATTR_BUF_SIZE   = 1024*1024*1024; //максимальный размер буфера разбора

#pragma pack(1)

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текстовый буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseTextBuffer
{
  ParseTextBuffer* next;   //поддержка списка загруженных текстовых
  size_t           size;    //размер буфера
  char             buf [1]; //буфер текста исходника
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка принадлежности строки буферу
///////////////////////////////////////////////////////////////////////////////////////////////////  
  bool IsBelong (const void* ptr) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация узла разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseNodeImpl
{
  size_t      argc;         //количество атрибутов связанных с узлом
  size_t      hash;         //хэш от имени узла
  size_t      next_offset;  //смещение к следующему узлу на данном уровне иерархии
  size_t      first_offset; //смещение до первого ребёнка
  const char* source_name;    //имя файла, в котором встретился атрибут
  size_t      line_number;  //номер строки
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя узла
///////////////////////////////////////////////////////////////////////////////////////////////////      
  const char* Tag () const; 
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор узлов дерева  
///////////////////////////////////////////////////////////////////////////////////////////////////
  ParseNodeImpl* Next  () const; //следующий узел в дереве
  ParseNodeImpl* First () const; //первый потомок
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации дерева разбора
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseTreeImpl
{  
  ParseTextBuffer* first_buf; //поддержка списка загруженных текстовых буферов
};

}

/*
    ParseNodeImpl
*/

inline ParseNodeImpl* ParseNodeImpl::First () const
{
  return first_offset ? (ParseNodeImpl*)((char*)this+first_offset) : 0;
}

inline ParseNodeImpl* ParseNodeImpl::Next () const
{
  return next_offset ? (ParseNodeImpl*)((char*)this+next_offset) : 0;
}

inline const char* ParseNodeImpl::Tag () const
{
  return *(const char**)((char*)this + sizeof (ParseNodeImpl));
}

/*
    ParseTextBuffer
*/

inline bool ParseTextBuffer::IsBelong (const void* ptr) const
{
  return buf <= ptr && buf + size > ptr;
}

/*
    ParseContext
*/

ParseContext::ParseContext (ParseLog* _log)
{  
  frame_offset.reserve (8);

  log       = _log;
  buf       = NULL;
  pos       = NULL;
  end       = NULL;
  grow_koef = 2;
  
  ParseTreeImpl* tree = Alloc<ParseTreeImpl> ();
  
  tree->first_buf = NULL;

  frame_offset.push_back (0);
 
  BeginElement ("root");
  EndElement   ();
  BeginFrame   ();
}

ParseContext::~ParseContext ()
{
}

ParseTreeImpl* ParseContext::GetParseTree () const
{
  return (ParseTreeImpl*)buf;
}

char* ParseContext::AttachTextSource (const char* file_name)
{
  if (!file_name)
    return NULL;

  try
  {
    InputFile file (file_name);

    size_t file_size = file.Size (),
           name_size = strlen (file_name)+1,
           buf_size  = file_size + sizeof (ParseTextBuffer) + name_size; //полагается: sizeof (ParseTextBuffer::buf) = 1

    ParseTextBuffer* text_buf = (ParseTextBuffer*)::operator new (buf_size);
    ParseTreeImpl*   tree     = (ParseTreeImpl*)buf;

    file_size       = file.Read (text_buf->buf+name_size,file_size);
    text_buf->next  = tree->first_buf;
    text_buf->size  = buf_size;
    tree->first_buf = text_buf;
    
    strcpy (text_buf->buf,file_name);
    
    text_buf->buf [file_size+name_size] = 0;

    return text_buf->buf+name_size;
  }
  catch (FileException& exception)
  {
    LogError (file_name,0,"%s",exception.Message ());
    return NULL;
  }
  catch (...)
  {
    throw;
  }
}

char* ParseContext::AttachTextSource (const char* name,const char* buf,size_t len)
{
  if (!buf)
    return NULL;
    
  if (!name)
    name = "__unnamed__";    
    
  size_t name_size = strlen (name)+1,
         buf_size  = len + sizeof (ParseTextBuffer) + name_size; //полагается: sizeof (ParseTextBuffer::buf) = 1
          
  ParseTextBuffer* text_buf = (ParseTextBuffer*)::operator new (buf_size);
  ParseTreeImpl*   tree     = (ParseTreeImpl*)buf;

  strcpy (text_buf->buf,name);
  memcpy (text_buf->buf+name_size,buf,len);

  text_buf->next  = tree->first_buf;
  text_buf->size  = buf_size;
  tree->first_buf = text_buf;

  text_buf->buf [len+name_size] = 0;

  return text_buf->buf+name_size;
}

void ParseContext::BeginElement (const char* name,const char* source_name,size_t line)
{
  size_t offset = pos-buf;

  if ((size_t)(end-pos) < sizeof (ParseNodeImpl) + sizeof (const char*))
    Resize (sizeof (ParseNodeImpl) + sizeof (const char*));

  ParseNodeImpl* node = (ParseNodeImpl*)pos;

  pos += sizeof (ParseNodeImpl) + sizeof (const char*);    

  if (frame_offset.back ())
    ((ParseNodeImpl*)(buf+frame_offset.back ()))->next_offset = offset-frame_offset.back ();
  else if (frame_offset.size () > 1)
    ((ParseNodeImpl*)(buf+frame_offset.end ()[-2]))->first_offset = offset-frame_offset.end ()[-2];

  node->line_number  = line;
  node->source_name  = source_name;
  node->hash         = strhash (name);
  node->next_offset  = 0;
  node->first_offset = 0;
  node->argc         = cur_argc = 1;

  frame_offset.back () = offset;

  ((const char**)pos) [-1] = name;
}

void ParseContext::EndElement ()
{
  ((ParseNodeImpl*)(buf+frame_offset.back ()))->argc = cur_argc;
}

void ParseContext::BeginFrame ()
{
  frame_offset.push_back (0);
}

void ParseContext::EndFrame ()
{
  if (frame_offset.size () > 1)
    frame_offset.pop_back ();
}

const char* ParseContext::GetTextSourceName (void* ptr) const
{
  ParseTreeImpl* tree = GetParseTree ();

  if (!tree)
    return "";

  for (ParseTextBuffer* i=tree->first_buf;i;i=i->next)
    if (i->IsBelong (ptr))
      return i->buf;

  return "__unknown__"; 
}

const char* ParseContext::GetCurrentNodeName () const
{
  if (!buf || frame_offset.size () <= 1)
    return "";

  return ((ParseNodeImpl*)(buf+frame_offset.end () [-2]))->Tag ();
}

void ParseContext::Resize (size_t size)
{
  size_t cur_size = pos-buf;

  if ((size_t)(end-pos) >= size)
    return;    

  if (size > MAX_ATTR_BUF_SIZE - cur_size)
  {
    char buf [128];
    
    string_wrappers::snprintf (buf,sizeof (buf),"Buffer can not be enlarged by %u bytes (cur_size=%u, max_size=%u)",
               size,cur_size,MAX_ATTR_BUF_SIZE);
    
    throw ParseBufferTooLargeException (buf);
  }

  size_t new_size = cur_size;

  if (cur_size < MAX_ATTR_BUF_SIZE / grow_koef) new_size = cur_size * grow_koef;
  if (new_size < cur_size + size)               new_size = cur_size + size;
  if (new_size < START_ATTR_BUF_SIZE)           new_size = START_ATTR_BUF_SIZE;

  char* new_buf = (char*)::operator new (new_size);

  if (buf)
  {
    memcpy (new_buf,buf,pos-buf);
    ::operator delete (buf);
  }

  pos = new_buf + cur_size;      
  buf = new_buf;
  end = new_buf + new_size;
}

bool ParseContext::Reserve (size_t nodes_count,size_t attrs_count)
{
  size_t max_avail = MAX_ATTR_BUF_SIZE - (pos - buf);
  
  if (nodes_count > max_avail / sizeof (ParseNodeImpl))
    return false;
    
  max_avail -= nodes_count * sizeof (ParseNodeImpl);

  if (attrs_count > max_avail / sizeof (char*))
    return false;

  Resize (nodes_count * sizeof (ParseNodeImpl) + attrs_count * sizeof (char*));

  return true;
}

void ParseContext::SetGrow (size_t k)
{
  grow_koef = k >= 2 ? k : 2;
}

void ParseContext::LogFatalError (const char* fmt,...)
{
  va_list list;
  
  va_start       (list,fmt);  
  VLogFatalError (fmt,list);  
}

void ParseContext::LogError (const char* file_name,size_t line,const char* fmt,...)
{
  va_list list;
  
  va_start  (list,fmt);  
  VLogError (file_name,line,fmt,list);  
}

void ParseContext::LogWarning (const char* file_name,size_t line,const char* fmt,...)
{
  va_list list;
  
  va_start    (list,fmt);
  VLogWarning (file_name,line,fmt,list);
}

void ParseContext::VLogFatalError (const char* fmt,va_list list)
{
  if (log)
    log->VFatalError (fmt,list);
}

void ParseContext::VLogError (const char* file_name,size_t line,const char* fmt,va_list list)
{
  if (log)
    log->VError (file_name,line,fmt,list);
}

void ParseContext::VLogWarning (const char* file_name,size_t line,const char* fmt,va_list list)
{
  if (log)
    log->VWarning (file_name,line,fmt,list);
}

/*
    ParseNode
*/

ParseNode* ParseNode::First () const
{
  return (ParseNode*)((ParseNodeImpl*)data)->First ();
}

ParseNode* ParseNode::Next () const
{
  return (ParseNode*)((ParseNodeImpl*)data)->Next ();
}
  
ParseNode* ParseNode::First (const char* tag) const
{ 
  if (!tag)
    return NULL;
    
  const char* subname = strchr (tag,'.');

  if (subname)
  {    
    size_t base_name_len = subname - tag,
           hash          = crc32 (tag,base_name_len);

    for (ParseNodeImpl* node=((ParseNodeImpl*)data)->First ();node;node=node->Next ())
      if (hash == node->hash && !strncmp (tag,node->Tag (),base_name_len))
        return ((ParseNode*)node)->First (subname+1);
  }
  else
  {
    size_t hash = strhash (tag);

    for (ParseNodeImpl* node=((ParseNodeImpl*)data)->First ();node;node=node->Next ())
      if (hash == node->hash && !strcmp (tag,node->Tag ()))
        return (ParseNode*)node;
  }
  
  return NULL;
}

ParseNode* ParseNode::Next (const char* tag) const
{
  if (!tag)
    return NULL;
   
  const char* subname = strchr (tag,'.');

  if (subname)
  {
    size_t base_name_len = subname - tag,
           hash          = crc32 (tag,base_name_len);

    for (ParseNodeImpl* node=((ParseNodeImpl*)data)->Next ();node;node=node->Next ())
      if (hash == node->hash && !strncmp (tag,node->Tag (),base_name_len))
        return ((ParseNode*)node)->First (subname+1);
  }
  else
  {
    size_t hash = strhash (tag);

    for (ParseNodeImpl* node=((ParseNodeImpl*)data)->Next ();node;node=node->Next ())
      if (hash == node->hash && !strcmp (tag,node->Tag ()))
        return (ParseNode*)node;
  }
  
  return NULL;
}

ParseNode* ParseNode::NextNamesake () const
{
  size_t hash = ((ParseNodeImpl*)data)->hash;

  for (ParseNodeImpl* node=((ParseNodeImpl*)data)->Next ();node;node=node->Next ())
    if (hash == node->hash && !strcmp (node->Tag (),((ParseNodeImpl*)data)->Tag ()))
      return (ParseNode*)node;

  return NULL;
}

size_t ParseNode::AttributesCount () const
{
  return ((ParseNodeImpl*)data)->argc-1;
}

const char** ParseNode::Attributes () const
{
  return (const char**)(data+sizeof (ParseNodeImpl))+1;
}

const char* ParseNode::Tag () const
{
  return *(const char**)(data+sizeof (ParseNodeImpl));
}

const char* ParseNode::Attribute (size_t i) const
{
  ParseNodeImpl* node = (ParseNodeImpl*)data;  

  return ++i < node->argc ? ((const char**)(data+sizeof (ParseNodeImpl)))[i] : NULL;
}

const char* ParseNode::SourceName () const
{
  return ((ParseNodeImpl*)data)->source_name;
}

size_t ParseNode::LineNumber () const
{
  return ((ParseNodeImpl*)data)->line_number;
}

/*
    Parser
*/

Parser::Parser (const char* file_name,const char* format)
{
  try
  {
    ParseContext context;

    ParseDispatch::Parse (file_name,context,format);

    impl = context.GetParseTree ();
  }
  catch (...)
  {
    impl = NULL;
  }
}

Parser::Parser (const char* name,const char* buf,size_t len,const char* format)
{
  try
  {
    ParseContext context;

    ParseDispatch::Parse (name,buf,len,context,format);

    impl = context.GetParseTree ();
  }
  catch (...)
  {
    impl = NULL;
  }
}

Parser::Parser (ParseLog& log,const char* file_name,const char* format)
{
  impl = NULL;

  try
  {
    ParseContext context (&log);

    ParseDispatch::Parse (file_name,context,format);

    impl = context.GetParseTree ();
  }
  catch (std::bad_alloc&)
  {
    log.FatalError ("No enough memory");
  }
  catch (std::exception& exception)
  {
    log.FatalError ("Exception at process file '%s': %s",file_name,exception.what ());
  }
  catch (...)
  {
  }
}

Parser::Parser (ParseLog& log,const char* name,const char* buf,size_t len,const char* format)
{
  impl = NULL;

  try
  {
    ParseContext context;

    ParseDispatch::Parse (name,buf,len,context,format);

    impl = context.GetParseTree ();
  }
  catch (std::bad_alloc&)
  {
    log.FatalError ("No enough memory");
  }
  catch (std::exception& exception)
  {
    log.FatalError ("Exception at process buffer '%s' (size=%u): %s",name,len,exception.what ());
  }
  catch (...)
  {
  }
}

Parser::~Parser ()
{
  if (!impl)
    return;

  for (ParseTextBuffer* i=impl->first_buf;i;)
  {
    ParseTextBuffer* next = i->next;

    ::operator delete (i);

    i = next;
  } 

  ::operator delete (impl);
}

Parser::Node* Parser::Root () const
{
  return impl ? (ParseNode*)((char*)impl+sizeof (ParseTreeImpl)) : 0;
}
