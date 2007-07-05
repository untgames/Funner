#ifndef __TEST__
#define __TEST__

#include <stl/string>
#include <stl/utility>

#define TRY_EXEC(X) try { X; }\
  catch (stl::exception& e) { printf ("exception: '%s'\n",e.what ()); }\
  catch (Char::Error) { printf ("exception 'Try to copy \"X\"'\n"); }\
  catch (...) { printf ("Unknown exception!\n"); }

inline void print (const char* s)   { printf ("'%s'",s); }
inline void print (int x)           { printf ("%d",x); }
inline void print (unsigned int x)  { printf ("%u",x); }
inline void print (char c)          { printf ("%c",c); }
inline void print (float x)         { printf ("%f",x); }

inline void print (const stl::string& s)  { printf ("'%s'",s.c_str ()); }

template <class T1,class T2> 
void print (const stl::pair<T1,T2>& p)
{ 
  printf ("(");
  print  (p.first);
  printf (",");    
  print  (p.second);  
  printf (")");  
}

template <class Iter> 
void print (const char* title,Iter first,Iter last)
{
  printf ("%s",title);  
  
  for (;first!=last;++first)
  {
    printf (" ");
    print  (*first);
  }
  
  printf ("\n");
}

inline int myrand ()
{
  static int holdrand = 1;
  
  return ((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
}

inline bool odd      (int x)       { return (x % 2) != 0; }
inline bool twice    (int x,int y) { return 2*x == y; }
inline bool abs_less (int x,int y) { return abs (x) < abs (y); }
inline bool negative (int x)       { return x < 0; }
inline bool positive (int x)       { return x > 0; }

#if !defined (__NIOS2__)
#pragma pack (push,1)
#endif

struct Char
{
  enum Error { ERROR = -1};

  Char () { 
    c = 0; 
    obj_count++; 
  }
  
  Char (char _c) {
    if (_c == 'X')
      throw ERROR;
  
    c = _c; 
    obj_count++; 
  }
  
  Char (const Char& _c) { 
    if (_c.c == 'X')
      throw ERROR;

    obj_count++;        
    c = _c; 
  }
  ~Char () { 
    obj_count--; 
  }  

  operator char& ()       { return c; }
  operator char  () const { return c; }

  char c;   
  
  static int obj_count;
};

template <class T> class MyAlloc: public stl::allocator<T>
{
  public:
    template <class Other> struct rebind
    { 
      typedef MyAlloc<Other> other; 
    };  
    
    typedef typename stl::allocator<T>::size_type size_type;
    
                        MyAlloc () {}
                        MyAlloc (const MyAlloc&) {}
    template <class T1> MyAlloc (const MyAlloc<T1>&) {}
    
    template <class T1> MyAlloc& operator = (const MyAlloc<T1>&) { return *this; }
  
    size_type max_size () const { return 10; }
};

#if !defined (__NIOS2__)
#pragma pack(pop)
#endif

int Char::obj_count = 0;

#endif
