#include <stl/string>
#include <stl/list>
#include <common/etc.h>
#include <common/strlib.h>
#include "validator.h"

using namespace stl;
using namespace math;

namespace common
{

/*
    Внутренние флаги состояния переменной
*/

enum VarInternalFlags 
{
  VAR_DIRTY_STRING_VALUE = 1,  //строковое значение переменной требует пересчёта
  VAR_DIRTY_FLOAT_VALUE  = 2,  //вещественное значение требует пересчёта
  VAR_DIRTY_INT_VALUE    = 4,  //целое значение переменной требует пересчёта
  VAR_PROCESS_SET        = 8,  //переменная нах-ся в транзакции OnSet
  VAR_PROCESS_CHECK      = 16, //переменная нах-ся в транзакции OnCheck
  VAR_PROCESS_GET        = 32, //переменная нах-ся в транзакции OnGet
};

/*
    Таблица идентификации типа переменной
*/

template <class T> struct VarTypeTraits {};

template <> struct VarTypeTraits<const char*>  { enum { TYPE = VAR_STRING,  INTERNAL_TYPE = TYPE }; };
template <> struct VarTypeTraits<string>       { enum { TYPE = VAR_STRING,  INTERNAL_TYPE = TYPE }; };
template <> struct VarTypeTraits<bool>         { enum { TYPE = VAR_BOOL,    INTERNAL_TYPE = VAR_INTEGER }; };
template <> struct VarTypeTraits<int>          { enum { TYPE = VAR_INTEGER, INTERNAL_TYPE = TYPE }; };
template <> struct VarTypeTraits<float>        { enum { TYPE = VAR_FLOAT,   INTERNAL_TYPE = TYPE }; };
template <> struct VarTypeTraits<vec2i>        { enum { TYPE = VAR_VEC2I,   INTERNAL_TYPE = VAR_INTEGER }; };
template <> struct VarTypeTraits<vec3i>        { enum { TYPE = VAR_VEC3I,   INTERNAL_TYPE = VAR_INTEGER }; };
template <> struct VarTypeTraits<vec4i>        { enum { TYPE = VAR_VEC4I,   INTERNAL_TYPE = VAR_INTEGER }; };
template <> struct VarTypeTraits<vec2f>        { enum { TYPE = VAR_VEC2F,   INTERNAL_TYPE = VAR_FLOAT }; };
template <> struct VarTypeTraits<vec3f>        { enum { TYPE = VAR_VEC3F,   INTERNAL_TYPE = VAR_FLOAT }; };
template <> struct VarTypeTraits<vec4f>        { enum { TYPE = VAR_VEC4F,   INTERNAL_TYPE = VAR_FLOAT }; };

/*
    Описание реализации переменной
*/

typedef stl::list<Var::EventHandler> HandlerList; //список обработчиков события

struct Var::Impl
{
  public:
    typedef void (*bind_fn)(Var&);

    Etc&          etc;           //владелец
    VarType       user_type;     //тип переменной (видимый для пользователя)
    VarType       itype;         //внутренний тип
    VarType       value_type;    //тип значения
    size_t        iflags;        //внутренние флаги
    string        str_value;     //строковое значение переменной 
    vec4f         float_value;   //вещественное векторное значение переменной
    vec4i         int_value;     //целое векторное значение переменной
    void*         bind_link;     //ссылка на бинд
    size_t        bind_buf_size; //размер прикреплённого буфера    
    bind_fn       bind_set;      //функция обновления подшитой переменной
    bind_fn       bind_get;      //функция чтения подшитой переменной
    HandlerList   set_handlers;  //обработчики события onSet
    EventHandler  get_handler;   //обработчик события onGet
    VarValidator* validator;     //валидатор значения переменной
    char          name [1];      //имя переменной

    Impl (Etc&,const char* prefix,const char* name);

                       const char* get_string (Var&);
    template <class T> const T&    get (Var&);
    template <class T> bool        set (const T&,Var&);

    const char* value (Var&,char* buf,size_t buf_size);

    template <class T> void bind (T&);
                       void bind (char* buf,size_t buf_size);
                       void unbind ();

                       void verify ();
    template <class T> void verify (const T& min,const T& max);
                       void verify (size_t count,const char** values);
                       void verify (const vector<string>&);

  private:  
    void set_notify (Var&);
    void get_notify (Var&);        

    template <VarType type> void _get ();
                            void _set (const char*);
                            void _set (const string&);
                            void _set (int);
                            void _set (float);
                            void _set (const vec<float,2>& v) { _set_float_vector (v); }
                            void _set (const vec<float,3>& v) { _set_float_vector (v); }
                            void _set (const vec<float,4>& v) { _set_float_vector (v); }
                            void _set (const vec<int,2>&   v) { _set_int_vector (v); }
                            void _set (const vec<int,3>&   v) { _set_int_vector (v); }
                            void _set (const vec<int,4>&   v) { _set_int_vector (v); }
    template <class T>      void _set_float_vector (const T&);
    template <class T>      void _set_int_vector (const T&);


    template <class T> T& select ();
    
    template <class T> static void get_bind (Var&);
    template <class T> static void set_bind (Var&);

    static void get_bind_string (Var&);
    static void set_bind_string (Var&);
};

/*
    Специализация получения значения переменной
*/

template <> float& Var::Impl::select<float> () { return float_value [0]; }
template <> int&   Var::Impl::select<int>   () { return int_value [0]; }
template <> vec2f& Var::Impl::select<vec2f> () { return *(vec2f*)&float_value; }
template <> vec3f& Var::Impl::select<vec3f> () { return *(vec3f*)&float_value; }
template <> vec4f& Var::Impl::select<vec4f> () { return *(vec4f*)&float_value; }
template <> vec2i& Var::Impl::select<vec2i> () { return *(vec2i*)&int_value; }
template <> vec3i& Var::Impl::select<vec3i> () { return *(vec3i*)&int_value; }
template <> vec4i& Var::Impl::select<vec4i> () { return *(vec4i*)&int_value; }

/*
    Var::Impl
*/

Var::Impl::Impl (Etc& _etc,const char* _prefix,const char* _name)
  : etc (_etc)
{
  user_type   = VAR_STRING;
  value_type  = VAR_STRING;
  iflags      = VAR_DIRTY_INT_VALUE|VAR_DIRTY_FLOAT_VALUE;
  bind_link   = NULL;
  bind_set    = NULL;
  bind_get    = NULL;
  str_value   = "";  
  validator   = NULL;
  
  if (_prefix) sprintf (name,"%s.%s",_prefix,_name);
  else         strcpy  (name,_name);
}

/*
    Конструкторы 
*/

Var::Var (Impl* _impl)
  : impl (_impl)
  { }
  
Var::~Var ()
{
  if (impl->validator)
    impl->validator->release ();
}  

/*
    Create / delete
*/

Var* Var::create (Etc& etc,const char* prefix,const char* name)
{  
  size_t size = sizeof (Var) + sizeof (Var::Impl) + strlen (name);
  
  if (prefix)
    size += strlen (prefix) + 1;
    
  char* buf = (char*)::operator new (size);

  return new (buf) Var (new (buf+sizeof (Var)) Var::Impl (etc,prefix,name));
}

void Var::release ()
{
  this->~Var ();
  ::operator delete (this);
}

/*
    Информация о переменной
*/

const char* Var::name () const
{
  return impl->name;
}

VarType Var::type () const
{
  return impl->user_type;
}

/*
    Чтение значения переменной
*/

inline void Var::Impl::get_notify (Var& var)
{
  iflags |= VAR_PROCESS_GET;

  if (bind_link && bind_get)
    bind_get (var);

  if (get_handler.valid ())
    get_handler (var);

  iflags &= ~VAR_PROCESS_GET;  
}

template <>
inline void Var::Impl::_get<VAR_INTEGER> ()
{
  if (!(iflags & VAR_DIRTY_INT_VALUE))
    return;    

  switch (itype)
  {
    case VAR_FLOAT:
      for (int i=0;i<4;i++) int_value [i] = (int)float_value [i];
      break;
    case VAR_STRING:
      int_value = atoi (str_value.c_str ());
      break;
    default:
      break;
  }

  iflags &= ~VAR_DIRTY_INT_VALUE;
}

template <>
inline void Var::Impl::_get<VAR_FLOAT> ()
{
  if (!(iflags & VAR_DIRTY_FLOAT_VALUE))
    return;

  switch (itype)
  {
    case VAR_INTEGER: 
      for (int i=0;i<4;i++) float_value [i] = (float)int_value [i];
      break;
    case VAR_STRING:
      float_value = (float)atof (str_value.c_str ());
      break;
    default:
      break;
  }

  iflags &= ~VAR_DIRTY_FLOAT_VALUE;
}

template <class T>
inline const T& Var::Impl::get (Var& var)
{
  if (!(iflags & VAR_PROCESS_GET))
    get_notify (var);
    
  _get<(VarType)VarTypeTraits<T>::INTERNAL_TYPE> ();
  
  return select<T> ();
}

inline const char* Var::Impl::get_string (Var& var)
{
  if (!(iflags & VAR_PROCESS_GET))
    get_notify (var);
      
  if (iflags & VAR_DIRTY_STRING_VALUE)
  {
    char buf [32];
    
    *buf = 0;    
  
    switch (itype)
    {
      case VAR_FLOAT:   sprintf (buf,"%f",float_value [0]); break;
      case VAR_INTEGER: sprintf (buf,"%d",int_value [0]); break;
      default:          break;
    }

    try
    {
      str_value = buf;      
    }
    catch (...)
    {
      try
      {
        str_value = "out of memory";
      }
      catch (...)
      {
        str_value = "";
      }
    }

    iflags &= ~VAR_DIRTY_STRING_VALUE;    
  }

  return str_value.c_str ();
}

inline const char* Var::Impl::value (Var& var,char* user_buf,size_t buf_size)
{
  static char default_value_buf [128];

  get_notify (var);
  
  char* buf = user_buf;
  
  if (!buf)
  {
    buf      = default_value_buf;
    buf_size = sizeof (default_value_buf);
  }
  
  if (!buf_size)
    return "";
  
  *buf = 0;
  
  switch (value_type)
  {
    case VAR_STRING:
      return str_value.c_str ();
    case VAR_BOOL:
      snprintf (buf,buf_size,"%s",int_value != vec4i (0) ? "true" : "false");
      break;
    case VAR_INTEGER:
      snprintf (buf,buf_size,"%d",int_value [0]);
      break;
    case VAR_FLOAT:
      snprintf (buf,buf_size,"%g",float_value [0]);
      break;
    case VAR_VEC2I:
      snprintf (buf,buf_size,"(%d,%d)",int_value [0],int_value [1]);
      break;
    case VAR_VEC3I:
      snprintf (buf,buf_size,"(%d,%d,%d)",int_value [0],int_value [1],int_value [2]);
      break;
    case VAR_VEC4I:
      snprintf (buf,buf_size,"(%d,%d,%d,%d)",int_value [0],int_value [1],int_value [2],int_value [3]);
      break;
    case VAR_VEC2F:
      snprintf (buf,buf_size,"(%g,%g)",float_value [0],float_value [1]);
      break;
    case VAR_VEC3F:
      snprintf (buf,buf_size,"(%g,%g,%g)",float_value [0],float_value [1],float_value [2]);
      break;
    case VAR_VEC4F:
      snprintf (buf,buf_size,"(%g,%g,%g,%g)",float_value [0],float_value [1],float_value [2],float_value [3]);
      break;
    default:
      break;
  }  

  buf [buf_size-1] = 0;
  
  return buf;
}

const char* Var::value (char* buf,size_t buf_size) const
{
  return impl->value ((Var&)*this,buf,buf_size);
}

const char* Var::gets () const
{
  return impl->get_string ((Var&)*this);
}

int Var::geti () const
{
  return impl->get<int> ((Var&)*this);
}

bool Var::test () const
{
  return impl->get<vec4i> ((Var&)*this) != vec4i (0);
}

const vec2i& Var::get2i () const
{
  return impl->get<vec2i> ((Var&)*this);
}

const vec3i& Var::get3i () const
{
  return impl->get<vec3i> ((Var&)*this);
}

const vec4i& Var::get4i () const
{
  return impl->get<vec4i> ((Var&)*this);
}

float Var::getf () const
{
  return impl->get<float> ((Var&)*this);
}

const vec2f& Var::get2f () const
{
  return impl->get<vec2f> ((Var&)*this);
}

const vec3f& Var::get3f () const
{
  return impl->get<vec3f> ((Var&)*this);
}

const vec4f& Var::get4f () const
{
  return impl->get<vec4f> ((Var&)*this);
}

/*
    Установка переменной
*/

inline void Var::Impl::set_notify (Var& var)
{    
  iflags |= VAR_PROCESS_CHECK;  
  
  if (validator)
    validator->validate (var);
  
  iflags |= VAR_PROCESS_SET;

  for (HandlerList::iterator i=set_handlers.begin ();i!=set_handlers.end ();++i)
    (*i)(var);

  if (bind_link && bind_set)
    bind_set (var);

  iflags &= ~(VAR_PROCESS_SET|VAR_PROCESS_CHECK);
}

inline void Var::Impl::_set (const char* s)
{
  try
  {
    str_value = s?s:"";
  }
  catch (...)
  {
    try
    {
      str_value = "out of memory";
    }
    catch (...)
    {
      str_value = "";
    }    
  }
    
  iflags    |= VAR_DIRTY_INT_VALUE|VAR_DIRTY_FLOAT_VALUE;
  iflags    &= ~VAR_DIRTY_STRING_VALUE;
}

inline void Var::Impl::_set (const string& s)
{
  _set (s.c_str ());
}

inline void Var::Impl::_set (int x)
{
  int_value  = x;
  iflags    |= VAR_DIRTY_FLOAT_VALUE|VAR_DIRTY_STRING_VALUE;
  iflags    &= ~VAR_DIRTY_INT_VALUE;
}

inline void Var::Impl::_set (float x)
{
  float_value  = x;
  iflags      |= VAR_DIRTY_INT_VALUE|VAR_DIRTY_STRING_VALUE;
  iflags    &= ~VAR_DIRTY_FLOAT_VALUE;
}

template <class T>
inline void Var::Impl::_set_int_vector (const T& v)
{
  int_value  = v;
  iflags    |= VAR_DIRTY_FLOAT_VALUE|VAR_DIRTY_STRING_VALUE;
  iflags    &= ~VAR_DIRTY_INT_VALUE;  
}

template <class T>
inline void Var::Impl::_set_float_vector (const T& v)
{
  float_value  = v;
  iflags      |= VAR_DIRTY_INT_VALUE|VAR_DIRTY_STRING_VALUE;
  iflags      &= ~VAR_DIRTY_FLOAT_VALUE;  
}

template <class T>
inline bool Var::Impl::set (const T& x,Var& var)
{
  if (iflags & VAR_PROCESS_SET)
    return false;

  _set (x);
  
  itype = (VarType)VarTypeTraits<T>::INTERNAL_TYPE;

  if (!(iflags & VAR_PROCESS_GET))
    user_type = (VarType)VarTypeTraits<T>::TYPE;
    
  value_type = (VarType)VarTypeTraits<T>::TYPE;
    
  if (!(iflags & VAR_PROCESS_CHECK))
    set_notify (var);

  return true;
}

bool Var::set (const char* s)
{
  return impl->set (s,*this);
}

bool Var::set (int value)
{
  return impl->set (value,*this);
}

bool Var::set (bool value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec2i& value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec3i& value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec4i& value)
{
  return impl->set (value,*this);
}

bool Var::set (float value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec2f& value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec3f& value)
{
  return impl->set (value,*this);
}

bool Var::set (const vec4f& value)
{
  return impl->set (value,*this);
}

/*
    Работа со списком оповещений
*/

void Var::attach (VarEvent event,const EventHandler& handler)
{
  switch (event)
  {
    case VAR_ON_SET:
      if (handler.valid ())    
        impl->set_handlers.push_back (handler);
      break;
    case VAR_ON_GET:
      impl->get_handler = handler;
      break;
    default:
      break;
  }
}

void Var::detach (VarEvent event,const EventHandler& handler)
{
  switch (event)
  {
    case VAR_ON_SET:
      impl->set_handlers.remove (handler);
      break;
    case VAR_ON_GET:
      if (impl->get_handler == handler)
        impl->get_handler = NULL;
      break;
    default:
      break;
  }
}

void Var::detach (VarEvent event)
{
  switch (event)
  {
    case VAR_ON_SET:
      impl->set_handlers.clear ();
      break;
    case VAR_ON_GET:
      impl->get_handler = NULL;
      break;
    default:
      break;
  }
}

void Var::detach ()
{
  impl->set_handlers.clear ();
  impl->get_handler = NULL;
}

/*
    Подшивка переменных
*/

template <class T>
void Var::Impl::get_bind (Var& var)
{
  var.impl->set (*(T*)var.impl->bind_link,var);
}

template <class T>
void Var::Impl::set_bind (Var& var)
{
  *(T*)var.impl->bind_link = var.impl->get<T> (var);
}

template <>
void Var::Impl::set_bind<stl::string> (Var& var)
{
  *(stl::string*)var.impl->bind_link = var.impl->get_string (var);
}

template <>
void Var::Impl::set_bind<bool> (Var& var)
{
  *(bool*)var.impl->bind_link = var.impl->get<vec4i> (var) != vec4i (0);
}

void Var::Impl::get_bind_string (Var& var)
{
  var.impl->set ((const char*)var.impl->bind_link,var);
}

void Var::Impl::set_bind_string (Var& var)
{ 
  const char* src = var.gets ();
  char*       dst = (char*)var.impl->bind_link;
  
  for (size_t count=var.impl->bind_buf_size;count-- && *src && *dst;*dst++=*src++);
  
  *dst = 0;  
}

template <class T>
inline void Var::Impl::bind (T& obj)
{
  bind_link     = &obj;
  bind_set      = set_bind<T>;
  bind_get      = get_bind<T>;
  bind_buf_size = 0;
}

inline void Var::Impl::bind (char* buf,size_t buf_size)
{
  if (!buf_size)
  {
    unbind ();    
    return;
  }

  bind_link     = buf;
  bind_buf_size = buf_size-1;
  bind_set      = set_bind_string;
  bind_get      = get_bind_string;  
} 

inline void Var::Impl::unbind ()
{
  bind_link     = NULL;
  bind_set      = NULL;
  bind_get      = NULL;
  bind_buf_size = 0;
}

void Var::bind (char* buf,size_t buf_size)
{
  impl->bind (buf,buf_size);
}

void Var::bind (stl::string& x)
{
  impl->bind (x);
}

void Var::bind (int& x)
{
  impl->bind (x);
}

void Var::bind (bool& x)
{
  impl->bind (x);
}

void Var::bind (float& x)
{
  impl->bind (x);
}

void Var::bind (vec2f& x)
{
  impl->bind (x);
}

void Var::bind (vec2i& x)
{
  impl->bind (x);
}

void Var::bind (vec3f& x)
{
  impl->bind (x);
}

void Var::bind (vec3i& x)
{
  impl->bind (x);
}

void Var::bind (vec4f& x)
{
  impl->bind (x);
}

void Var::bind (vec4i& x)
{
  impl->bind (x);
}

void Var::unbind ()
{
  impl->unbind ();
}

/*
    Верификация
*/

inline VarValidatorFactory& Var::get_validator_factory (Etc& etc)
{
  return etc.get_validator_factory ();
}

inline void Var::Impl::verify ()
{
  if (validator)
    validator->release ();

  validator = NULL;
}

template <class T> 
inline void Var::Impl::verify (const T& min,const T& max)
{
  if (validator)
    validator->release ();  
    
  validator = get_validator_factory (etc).create (min,max);
}

inline void Var::Impl::verify (size_t count,const char** values)
{
  if (validator)
    validator->release ();  
    
  validator = get_validator_factory (etc).create (count,values);
}

inline void Var::Impl::verify (const vector<string>& values)
{
  if (validator)
    validator->release ();

  validator = get_validator_factory (etc).create (values);
}

void Var::verify ()
{
  impl->verify ();
}

void Var::verify (size_t count,const char** values)
{
  if (!values || !count)
  {
    verify ();
    return;
  }

  impl->verify (count,values);
}

void Var::verify (const char* values_string,const char* delimiters,const char* spaces)
{
  if (!values_string || !delimiters || !spaces)
  {
    verify ();
    return;
  }  
  
  impl->verify (split (values_string,delimiters,spaces));
}

void Var::verify (int min,int max)
{
  impl->verify (min,max); 
}

void Var::verify (float min,float max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec2i& min,const vec2i& max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec3i& min,const vec3i& max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec4i& min,const vec4i& max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec2f& min,const vec2f& max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec3f& min,const vec3f& max)
{
  impl->verify (min,max); 
}

void Var::verify (const vec4f& min,const vec4f& max)
{
  impl->verify (min,max); 
}

}
