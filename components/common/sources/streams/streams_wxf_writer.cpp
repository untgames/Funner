#include "shared.h"

using namespace common;

/*
    Описание реализации
*/

struct WxfWriter::Impl
{
  OutputTextStream stream;           //поток вывода
  size_t           frame_indent;     //отступ фрейма
  size_t           attr_indent;      //отступ атрибутов от имени тэга
  size_t           current_indent;   //текущий отступ
  bool             begin_frame_flag; //флага начала фрейма
  
  Impl (const WriteFunction& writer, size_t in_frame_indent, size_t in_attr_indent)
    : stream (writer), frame_indent (in_frame_indent), attr_indent (in_attr_indent), current_indent (0), begin_frame_flag (true) {}
};

/*
    Конструкторы
*/

WxfWriter::WxfWriter (const char* file_name, size_t frame_indent, size_t attr_indent)
  : impl (new Impl (FileWriter (file_name), frame_indent, attr_indent))
{
}

WxfWriter::WxfWriter (const WriteFunction& writer, size_t frame_indent, size_t attr_indent)
  : impl (new Impl (writer, frame_indent, attr_indent))
{
}

WxfWriter::~WxfWriter ()
{
  while (impl->current_indent)
    EndFrame ();
}

/*
    Поток вывода
*/

const OutputTextStream& WxfWriter::Stream () const
{
  return impl->stream;
}

OutputTextStream& WxfWriter::Stream ()
{
  return impl->stream;
}

/*
    Размеры отступов
*/

//отступ фреймов
size_t WxfWriter::FrameIndent () const
{
  return impl->frame_indent;
}

//отступ от тэга до списка атрибутов
size_t WxfWriter::AttributesIndent () const
{
  return impl->attr_indent;
}

/*
    Работа с фреймами
*/

void WxfWriter::BeginFrame ()
{
  write       (impl->stream, "\n");
  WriteIndent ();
  write       (impl->stream, "{\n");

  impl->current_indent    += impl->frame_indent;
  impl->begin_frame_flag   = true;
}

void WxfWriter::BeginFrame (const char* frame_tag)
{
  CheckTag    (frame_tag, "common::WxfWriter::BeginFrame");
  WriteIndent ();
  write       (impl->stream, frame_tag);
  BeginFrame  ();
}

void WxfWriter::EndFrame ()
{
  if (!impl->current_indent)
    return;

  impl->current_indent -= impl->frame_indent;

  WriteIndent  ();
  write        (impl->stream, "}\n");
}

/*
    Сериализация атрибутов
*/

void WxfWriter::Write (const char* tag)
{
  CheckTag     (tag, "common::WxfWriter::Write");    
  WriteIndent  ();  
  write        (impl->stream, tag);
  write        (impl->stream, "\n");
}

/*
    Вставка комментария
*/

void WxfWriter::WriteComment (const char* comment)
{
  if (!comment)
    raise_null_argument ("common::WxfWriter::WriteComment", "comment");

  if (!impl->begin_frame_flag)
    write (impl->stream, "\n");

  WriteIndent ();

  if (strchr (comment,'\n'))
  { 
    write (impl->stream, "/*\n");
    
    const char *line_start = comment, *current_char = comment;

    for (;*current_char; current_char++)
      switch (*current_char)
      {
        case '\n':
          WriteIndent        (impl->current_indent + impl->frame_indent);
          impl->stream.Write (line_start, current_char - line_start);
          impl->stream.Write ("\n");

          line_start = current_char + 1;

          break;
        default:
          break;
      }

    if (line_start != current_char)
    {
      WriteIndent (impl->current_indent + impl->frame_indent);
      write       (impl->stream, line_start);  
      write       (impl->stream, "\n");
    }

    WriteIndent   ();
    write         (impl->stream, "*/\n\n");
  }
  else
  {
    write (impl->stream, "//");
    write (impl->stream, comment);
    write (impl->stream, "\n\n");
  }  
}

void WxfWriter::WriteComment (const wchar_t*)
{
  raise_not_implemented ("common::WxfWriter::WriteComment");
}

/*
    Сброс буферов потока
*/

void WxfWriter::Flush ()
{
  impl->stream.Buffer ().Flush ();
}

/*
    Печать отступа
*/

void WxfWriter::WriteIndent ()
{
  WriteIndent (impl->current_indent);
}

void WxfWriter::WriteIndent (size_t indent)
{
  write (impl->stream, indent, ' ');

  impl->begin_frame_flag = false;
}

/*
    Проверка корректности wxf-тэга
*/

void WxfWriter::CheckTag (const char* tag, const char* function_name)
{
  if (!tag)
    raise_null_argument (function_name, "tag");

  for (const char* s=tag; *s; s++)
    switch (*s)
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y': 
      case 'Z':
      case 'a': case 'b': case 'c': case 'd': case 'e':
      case 'f': case 'g': case 'h': case 'i': case 'j':
      case 'k': case 'l': case 'm': case 'n': case 'o':
      case 'p': case 'q': case 'r': case 's': case 't':
      case 'u': case 'v': case 'w': case 'x': case 'y':
      case 'z':
      case '_':
        break;
      default:
        raise<WrongWxfTagException> (function_name, "Wrong wxf-format tag '%s'", tag);
        break;
    }
}
