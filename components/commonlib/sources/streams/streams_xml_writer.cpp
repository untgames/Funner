#include "shared.h"

using namespace common;

/*
    Состояние записи
*/

enum WriterState
{
  WriterState_DocumentStart,  //запись в начале документа
  WriterState_DocumentEnd,    //запись завершена
  WriterState_AttributeList,  //запись списка атрибутов
  WriterState_AttributeValue, //запсь значения атрибута
  WriterState_NodeScope,      //запись в блоке узла
};

/*
    Описание реализации XmlWriter    
*/

typedef stl::stack<stl::string> NodeStack;

struct XmlWriter::Impl
{
  OutputTextStream stream;         //поток вывода
  size_t           node_indent;    //отступ между узлами
  NodeStack        node_stack;     //стек имён узлов
  WriterState      state;          //состояние записи
  
  Impl (const WriteFunction& writer, size_t in_node_indent) :
       stream (writer), node_indent (in_node_indent), state (WriterState_DocumentStart) {}
};

/*
    Конструкторы
*/

XmlWriter::XmlWriter (const char* file_name, size_t node_indent)
  : impl (new Impl (FileWriter (file_name), node_indent))
{
  WriteHeader ();
}

XmlWriter::XmlWriter (const WriteFunction& writer, size_t node_indent)
  : impl (new Impl (writer, node_indent))
{
  WriteHeader ();
}

XmlWriter::~XmlWriter ()
{
  while (!impl->node_stack.empty ())
    EndNode ();
}

/*
    Поток вывода
*/

const OutputTextStream& XmlWriter::Stream () const
{
  return impl->stream;
}

OutputTextStream& XmlWriter::Stream ()
{
  return impl->stream;
}

/*
    Размер отступа нового узла
*/

size_t XmlWriter::NodeIndent () const
{
  return impl->node_indent;
}

/*
    Работа с атрибутами
*/

void XmlWriter::BeginAttribute (const char* name)
{
  if (!name)
    RaiseNullArgument ("common::XmlWriter::WriteAttribute", "name");    
    
  if (impl->state != WriterState_AttributeList)
    RaiseInvalidOperation ("common::XmlWriter::WriteAttribute", "Can't write attribute. Attribute list not opened");
        
  write (impl->stream, " ");
  write (impl->stream, name);
  write (impl->stream, "=\"");

  impl->state = WriterState_AttributeValue;
}

void XmlWriter::EndAttribute ()
{
  if (impl->state != WriterState_AttributeValue)
    return;
    
  write (impl->stream, "\"");
    
  impl->state = WriterState_AttributeList;
}

void XmlWriter::CloseAttributeList ()
{
  EndAttribute ();

  if (impl->state != WriterState_AttributeList)
    return;

  write (impl->stream, ">\n");
  
  impl->state = WriterState_NodeScope;
}

/*
    Работа с узлами
*/

void XmlWriter::BeginNode (const char* name)
{
  if (!name)
    RaiseNullArgument ("common::XmlWriter::BeginNode", "name");

    //проверка возможности записи

  CheckEndOfDocument ("common::XmlWriter::BeginNode");

    //закрытие списка атрибутов

  CloseAttributeList ();
  
    //запись отступа
  
  WriteIndent ();  

    //помещение имени узла в стек

  impl->node_stack.push (name);

    //запись заголовка узла

  write (impl->stream, "<");
  write (impl->stream, name);

    //переход в состояние записи списка атрибутов

  impl->state = WriterState_AttributeList;
}

void XmlWriter::EndNode ()
{
  if (impl->node_stack.empty ())
    return;

  switch (impl->state)
  {
    case WriterState_DocumentStart:      
      return;
    case WriterState_NodeScope:
      WriteIndent (-1);

      write (impl->stream, "</");
      write (impl->stream, impl->node_stack.top ());
      write (impl->stream, ">\n");
      
      impl->node_stack.pop ();

      if (impl->node_stack.empty ())
        impl->state = WriterState_DocumentEnd;          

      break;
    case WriterState_AttributeValue:
      write (impl->stream, "\"");
    case WriterState_AttributeList:    
      write (impl->stream, "/>\n");

      impl->node_stack.pop ();

      impl->state = impl->node_stack.empty () ? WriterState_DocumentEnd : WriterState_NodeScope;

      break;
    case WriterState_DocumentEnd:
      RaiseEndOfDocument ("common::XmlWriter::EndNode");
      break;
  }
}

/*
    Запись данных
*/

void XmlWriter::BeginData ()
{
  CheckEndOfDocument ("common::XmlWriter::BeginData");
  CloseAttributeList ();
  WriteIndent        ();
}

void XmlWriter::EndData ()
{
  write (impl->stream, "\n");
}

void XmlWriter::BeginCData ()
{
  CheckEndOfDocument ("common::XmlWriter::BeginCData");
  CloseAttributeList ();
  WriteIndent        ();
  write              (impl->stream, "<![CDATA[");
}

void XmlWriter::EndCData ()
{
  write (impl->stream, "]]>\n");
}

/*
    Вставка комментария
*/

void XmlWriter::WriteComment (const char* comment)
{
  if (!comment)
    RaiseNullArgument ("common::XmlWriter::WriteComment", "comment");
    
    //проверка возможности записи
    
  CheckEndOfDocument ("common::XmlWriter::WriteComment");
    
    //закрытие списка атрибутов
    
  CloseAttributeList ();

    //запись комментария

  WriteIndent ();

  write (impl->stream, "<!--");

  if (strchr (comment,'\n'))
  { 
    write (impl->stream, "\n");
    
    const char *line_start = comment, *current_char = comment;

    for (;*current_char; current_char++)
      switch (*current_char)
      {
        case '\n':
          WriteIndent        (1);
          impl->stream.Write (line_start, current_char - line_start);
          impl->stream.Write ("\n");

          line_start = current_char + 1;

          break;
        default:
          break;
      }

    if (line_start != current_char)
    {
      WriteIndent (1);
      write       (impl->stream, line_start);  
      write       (impl->stream, "\n");
    }

    WriteIndent ();
  }
  else
  {
    write (impl->stream, " ");
    write (impl->stream, comment);
    write (impl->stream, " ");
  }

  write (impl->stream, "-->\n");
}

/*
    Сброс буферов потока
*/

void XmlWriter::Flush ()
{
  impl->stream.Buffer ().Flush ();  
}

/*
    Запись заголовка
*/

void XmlWriter::WriteHeader ()
{
  impl->stream.Write ("<?xml version=\"1.0\" encoding = \"windows-1251\"?>\n");
}

/*
    Запись отступа
*/

void XmlWriter::WriteIndent (int offset)
{
  write (impl->stream, (impl->node_stack.size () + offset) * impl->node_indent, ' ');
}

/*
    Проверка корректности. Возбуждение исключения в случае записи в конец документа
*/

void XmlWriter::RaiseEndOfDocument (const char* source)
{
  RaiseInvalidOperation (source, "Root node has been closed");
}

void XmlWriter::CheckEndOfDocument (const char* source)
{
  if (impl->state == WriterState_DocumentEnd)
    RaiseEndOfDocument (source);
}
