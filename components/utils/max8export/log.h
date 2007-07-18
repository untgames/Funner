#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fstream>

using std::ofstream;

const char endl = '\n';
const char tab = '\t';

///функции вывода

bool  logOpen        (const char* file_name,int tab_size=4);
void  logClose       ();

int   logPrintf      (const char* fmt,...);
void  logPuts        (const char*);

void  logBeginFrame  (const char*,const char* params = "");
void  logEndFrame    ();
int   logFrameLevel  ();

struct DumpStream
{
   DumpStream& operator << (const char* s)       { logPuts(s); return *this; }
   DumpStream& operator << (const void* ptr)     { logPrintf("%p",ptr); return *this; }
   DumpStream& operator << (int value)           { logPrintf("%d",value); return *this; }
   DumpStream& operator << (long value)          { logPrintf("%ld",value); return *this; }
   DumpStream& operator << (unsigned int v)      { logPrintf("%u",v); return *this; }
   DumpStream& operator << (unsigned long v)     { logPrintf("%lu",v); return *this; }
   DumpStream& operator << (float  f)            { logPrintf("%8.4f",f); return *this; }
   DumpStream& operator << (char ch)             { logPrintf("%c",ch); return *this; }
};

extern DumpStream dump; 
extern ofstream dout;

///вывод стандартных структур

DumpStream& operator << (DumpStream&,const Color&);
DumpStream& operator << (DumpStream&,const Point3&);
DumpStream& operator << (DumpStream&,const Point4&);
DumpStream& operator << (DumpStream&,const Matrix3&);
//DumpStream& operator << (DumpStream&,const Quat&);
DumpStream& operator << (DumpStream&,const AngAxis&);
DumpStream& operator << (DumpStream&,const ScaleValue&);

void DumpMatrix (const char* tag,const Matrix3&);

//Для анимации

DumpStream& operator << (DumpStream&,const IKey&);
DumpStream& operator << (DumpStream&,const ITCBKey&);
DumpStream& operator << (DumpStream&,const ITCBPoint3Key&);
DumpStream& operator << (DumpStream&,const ITCBRotKey&);
DumpStream& operator << (DumpStream&,const ITCBScaleKey&);
DumpStream& operator << (DumpStream&,const IBezPoint3Key&);
DumpStream& operator << (DumpStream&,const IBezScaleKey&);
DumpStream& operator << (DumpStream&,const IBezQuatKey&);
DumpStream& operator << (DumpStream&,const ILinPoint3Key&);
DumpStream& operator << (DumpStream&,const ILinRotKey&);
DumpStream& operator << (DumpStream&,const ILinScaleKey&);
