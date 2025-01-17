#ifndef SYSLIB_DYNAMIC_LIBRARY_HEADER
#define SYSLIB_DYNAMIC_LIBRARY_HEADER

#include <stl/auto_ptr.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Динамически подключаемая библиотека
///////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicLibrary
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicLibrary  ();
    DynamicLibrary  (const char* name);
    DynamicLibrary  (const wchar_t* name);
    ~DynamicLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load   (const char* name);
    void Load   (const wchar_t* name);
    void Unload ();

    bool IsLoaded () const; //загружена ли библиотека

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*     Name        () const;
    const wchar_t*  NameUnicode () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение адреса символа по строковому идентификатору
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetSymbol (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (DynamicLibrary&);
    
  private:
    DynamicLibrary (const DynamicLibrary&); //no impl
    DynamicLibrary& operator = (const DynamicLibrary&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (DynamicLibrary&, DynamicLibrary&);

}

#endif
