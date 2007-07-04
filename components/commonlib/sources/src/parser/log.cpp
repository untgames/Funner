#include <common/parser.h>
#include <common/strwrap.h>

using namespace common;

const size_t START_LOG_BUF_SIZE   = 1024;    //стартовый размер протокола
const size_t MAX_LOG_BUF_SIZE     = 16*1024; //максимальный размер протокола
const size_t INTERNAL_BUFFER_SIZE = 128;     //размер внутреннего буфера протокола

///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о сообщении
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseLogMessage
{
  ParseLogMessageType type;           //тип сообщения  
  size_t              message_offset; //смещение до строки с сообщением
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации протокола парсера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ParseLog::Impl 
{
  size_t           buffer_size;     //размер буфера
  size_t           available;       //доступный размер буфера
  char*            buffer;          //буфер протокола
  ParseLogMessage* message_pos;     //позиция для записи следующего заголовка сообщения
  char*            string_pos;      //позиция для записи следующей строки
  bool             has_errors;      //есть ошибки разбора
  bool             has_warnings;    //есть предупреждения разбора
  size_t           messages_count;  //количество сообщений
  char             internal_buffer [INTERNAL_BUFFER_SIZE]; //внутренний буфер на случай фатальных ошибок

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  Impl  ();
  ~Impl ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Резервирование места под сообщение
///////////////////////////////////////////////////////////////////////////////////////////////////
  char* AllocMessage (size_t message_size,ParseLogMessageType type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление сообщения
///////////////////////////////////////////////////////////////////////////////////////////////////
  void InsertMessage (const char*         file_name,
                      size_t              line,
                      ParseLogMessageType msg_type,
                      const char*         format,
                      va_list             list);
};

/*
    ParseLog::Impl
*/

ParseLog::Impl::Impl ()
{
  buffer_size    = sizeof (internal_buffer);
  available      = buffer_size;
  buffer         = internal_buffer;
  string_pos     = buffer;
  message_pos    = (ParseLogMessage*)(buffer + buffer_size);
  has_errors     = false;
  has_warnings   = false;
  messages_count = 0;
}

ParseLog::Impl::~Impl ()
{
  if (buffer != internal_buffer)
    ::operator delete (buffer);
}

char* ParseLog::Impl::AllocMessage (size_t message_size,ParseLogMessageType type)
{
  size_t size = message_size + sizeof (ParseLogMessage) + sizeof (char);

  if (available < size)
  {
    size_t new_size = buffer_size > size ? buffer_size * 2 : buffer_size + size;

    if (new_size < START_LOG_BUF_SIZE)
      new_size = START_LOG_BUF_SIZE;

    if (new_size > MAX_LOG_BUF_SIZE)
      return NULL;

    char* new_buf = (char*)::operator new (new_size,std::nothrow);

    if (!new_buf)
      return NULL;
      
    size_t messages_buf_size = buffer + buffer_size - (char*)message_pos,
           string_buf_size   = string_pos - buffer;
    
    if (buffer)
    {
      memcpy (new_buf,buffer,string_buf_size);
      memcpy (new_buf + new_size - messages_buf_size,message_pos,messages_buf_size);

      if (buffer != internal_buffer)
        ::operator delete (buffer);
    }

    buffer      = new_buf;
    buffer_size = new_size;
    available   = new_size - string_buf_size - messages_buf_size;
    string_pos  = new_buf + string_buf_size;
    message_pos = (ParseLogMessage*)(new_buf + new_size - messages_buf_size);
  }

  ParseLogMessage* message_header = --message_pos;
  message_header->type            = type;
  message_header->message_offset  = string_pos - buffer;  
  available                      -= size;

  messages_count++;  
  
  char* message_string = string_pos;
  
  string_pos += message_size + sizeof (char);
  
  return message_string;
}

void ParseLog::Impl::InsertMessage
 (const char*         file_name,
  size_t              line,
  ParseLogMessageType type,
  const char*         format,
  va_list             list)
{
  const char* message_type_string;

  switch (type)
  {
    default:                    type                = PARSE_LOG_ERROR;
    case PARSE_LOG_ERROR:       message_type_string = "error";   break;
    case PARSE_LOG_WARNING:     message_type_string = "warning"; break;
    case PARSE_LOG_FATAL_ERROR: message_type_string = "fatal error"; break;
  }
    
  if (type != PARSE_LOG_FATAL_ERROR)
  {
    size_t size = vsnprintf (NULL,0,format,list) + snprintf (NULL,0,"%s(%u): %s: ",file_name,line,message_type_string);
    
    if (size == (size_t)-1)
      return;

    char* pos = AllocMessage (size,type);
    
    if (!pos)
      return;

    pos    += sprintf  (pos,"%s(%u): %s: ",file_name,line,message_type_string);
    pos    += vsprintf (pos,format,list);
    *pos++  = '\0';
  }
  else
  {    
    size_t size = vsnprintf (NULL,0,format,list) + snprintf (NULL,0,"%s: ",message_type_string);
    
    if (size == (size_t)-1)
      return;

    char* pos = AllocMessage (size,type);
    
    if (!pos)
      return;

    pos    += sprintf  (pos,"%s: ",message_type_string);
    pos    += vsprintf (pos,format,list);
    *pos++  = '\0';
  }
}

/*
    ParseLog
*/

ParseLog::ParseLog ()
{
  impl = new Impl;
}

ParseLog::~ParseLog ()
{
  delete impl;
}

size_t ParseLog::MessagesCount () const
{
  return impl->messages_count;
}

bool ParseLog::HasErrors () const
{
  return impl->has_errors;
}

bool ParseLog::HasWarnings () const
{
  return impl->has_warnings;
}

const char* ParseLog::Message (size_t index) const
{
  if (index >= impl->messages_count)
    return ""; //throw exception!!!!

  return impl->buffer + impl->message_pos [impl->messages_count - index - 1].message_offset;
}

ParseLogMessageType ParseLog::MessageType (size_t index) const
{
  if (index >= impl->messages_count)
    return PARSE_LOG_FATAL_ERROR;

  return impl->message_pos [impl->messages_count - index - 1].type;
}

void ParseLog::VFatalError (const char* format,va_list list)
{
  impl->has_errors = true;

  impl->InsertMessage ("__undefined__",0,PARSE_LOG_FATAL_ERROR,format,list);
}

void ParseLog::VError (const char* file_name,size_t line,const char* format,va_list list)
{
  impl->has_errors = true;

  impl->InsertMessage (file_name,line,PARSE_LOG_ERROR,format,list);
}

void ParseLog::VWarning (const char* file_name,size_t line,const char* format,va_list list)
{
  impl->has_warnings = true;

  impl->InsertMessage (file_name,line,PARSE_LOG_WARNING,format,list);
}

void ParseLog::VError (ParseNode* node,const char* format,va_list list)
{
  VError (node->SourceName (),node->LineNumber (),format,list);
}

void ParseLog::VWarning (ParseNode* node,const char* format,va_list list)
{
  VWarning (node->SourceName (),node->LineNumber (),format,list);
}

void ParseLog::FatalError (const char* format,...)
{
  va_list list;

  va_start    (list,format);  
  VFatalError (format,list);
}

void ParseLog::Error (const char* file_name,size_t line,const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  VError   (file_name,line,format,list);
}

void ParseLog::Warning (const char* file_name,size_t line,const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  VWarning (file_name,line,format,list);
}

void ParseLog::Error (ParseNode* node,const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  VError   (node,format,list);  
}

void ParseLog::Warning (ParseNode* node,const char* format,...)
{
  va_list list;
  
  va_start (list,format);  
  VWarning (node,format,list);
}
