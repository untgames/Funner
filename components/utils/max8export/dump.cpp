#include "export.h"

void DumpMatrix (const char* tag,const Matrix3& tm)
{
  logBeginFrame (tag);
  dump<<tm;
  logEndFrame   ();
}

DumpStream& operator << (DumpStream& os,const Color& p)
{
  return os<<p.r<<"\t"<<p.g<<"\t"<<p.b;
}

DumpStream& operator << (DumpStream& os,const IKey& key)
{
  return os<<key.time;
} 

DumpStream& operator << (DumpStream& os,const ITCBKey& key)
{
  return os<<key.tens<<"\t"<<key.cont<<"\t"<<key.bias<<"\t"<<key.easeIn<<"\t"<<key.easeOut;
}

DumpStream& operator << (DumpStream& os,const ITCBPoint3Key& key)
{
  return os<<(IKey&)key<<"\t\t"<<(Point3&)key.val<<"\t\t"<<(ITCBKey&)key;
}

DumpStream& operator << (DumpStream& os,const ITCBRotKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<(ITCBKey&)key;
}

DumpStream& operator << (DumpStream& os,const ITCBScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<(ITCBKey&)key;
}

DumpStream& operator << (DumpStream& os,const IBezPoint3Key& key)
{
 
/*  logBeginFrame ("");
  {    
    os<<"time\t"<<key.time<<endl;
    os<<"intan\t"<<key.intan<<endl;
    os<<"outtan\t"<<key.outtan<<endl;
    os<<"inlen\t"<<key.inLength<<endl;
    os<<"outlen\t"<<key.outLength<<endl;
  }  
  logEndFrame ();

  return os;*/
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<key.intan<<" "<<key.outtan<<"\t\t"<<key.inLength<<" "<<key.outLength<<" "<<key.flags;
}

DumpStream& operator << (DumpStream& os,const IBezScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<key.intan<<"\t"<<key.outtan;
}

DumpStream& operator << (DumpStream& os,const IBezQuatKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

DumpStream& operator << (DumpStream& os,const ILinPoint3Key& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

DumpStream& operator << (DumpStream& os,const ILinRotKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

DumpStream& operator << (DumpStream& os,const ILinScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

//Внимание! Объявлен в maxsdk\include\quat.h. Но реаизация не найдена
//При экспорте возможна ситуация с разной логикой операторов

DumpStream& operator << (DumpStream& os,const Quat& q)
{
  Point3 axis;
  float  angle;

  AngAxisFromQ (q,&angle,axis);

  return os<<axis<<"\t\t"<<angle;
} 

DumpStream& operator << (DumpStream& os,const AngAxis& v)
{
  return os<<v.axis<<"\t"<<v.angle;
}

DumpStream& operator << (DumpStream& os,const ScaleValue& s)
{
  return os<<s.s<<"\t"<<s.q;
}

DumpStream& operator << (DumpStream& os,const Point3& p)
{
  return os<<p.x<<"\t"<<p.y<<"\t"<<p.z;
//  return os<<-1.0f*p.x<<"\t"<<p.z<<"\t"<<p.y;
}

DumpStream& operator << (DumpStream& os,const Point4& p)
{
  return os<<p.x<<"\t"<<p.y<<"\t"<<p.z<<"\t"<<p.w;
//  return os<<-1.0f*p.x<<"\t"<<p.z<<"\t"<<p.y;
}

DumpStream& operator << (DumpStream& os,const Matrix3& m)
{
/*  static float d [4][3] = {
    {-1.0,0,0},
    {0,0,1},
    {0,1,0},
    {0,0,0}
  };
  static Matrix3 dm (d);
  static float   x [4] = {0,0,0,1};

  Matrix3 dumpMatrix = dm * m;
//  Matrix3 dumpMatrix = m;

  for (int i=0;i<3;i++)    
  {
    os<<"row\t\t";
    for (int j=0;j<4;j++)       
      os<<dumpMatrix.GetRow (j)[i]<<"\t";
    os<<endl;  
  }  
  os<<"row\t\t0\t0\t0\t1"<<endl;*/
//    os<<dumpMatrix.GetRow (i).x<<"\t"
//      <<dumpMatrix.GetRow (i).y<<"\t"
//      <<dumpMatrix.GetRow (i).z<<"\t"
//      <<x [i]
//      <<endl;
  for (int i=0;i<3;i++)
    os<<"row\t"<<m.GetColumn (i)<<endl;

  os<<"row\t\t0\t0\t0\t1"<<endl;

  return os;
}
