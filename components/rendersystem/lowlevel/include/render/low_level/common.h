#ifndef RENDER_LOW_LEVEL_COMMON_HEADER
#define RENDER_LOW_LEVEL_COMMON_HEADER

#include <stddef.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef float Matrix4f;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Color3f
{
  float red, green, blue;
};

struct Color4f: public Color3f
{
  float alpha;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Rect
{
  int    x;      //�������� ������ �������� ���� �������
  int    y;      //�������� ������ �������� ���� �������
  size_t width;  //������ �������
  size_t height; //������ �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Viewport: public Rect
{
  float  min_depth; //����������� ������� [0;1]
  float  max_depth; //������������ ������� [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CompareMode
{
  CompareMode_AlwaysFail,   //��������� ��������� ������ ����
  CompareMode_AlwaysPass,   //��������� ��������� ������ ������
  CompareMode_Equal,        //new_value == reference_value
  CompareMode_NotEqual,     //new_value != reference_value
  CompareMode_Less,         //new_value <  reference_value
  CompareMode_LessEqual,    //new_value <= reference_value
  CompareMode_Greater,      //new_value >  reference_value
  CompareMode_GreaterEqual, //new_value >= reference_value

  CompareMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum UsageMode
{
  UsageMode_Default,  //��� ������ ���������� � ������������ � �������������� ���������� ���������
  UsageMode_Static,   //��������� ����� �� ����� ���� �������
  UsageMode_Dynamic,  //����� ����� �������� ������ ����
  UsageMode_Stream,   //����� ������ ��� ������ CPU<->GPU
    //�������� SystemMemory
  
  UsageMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BindFlag
{
  BindFlag_VertexBuffer = 1,  //����� ����� ���� ����������� ��� ��������� �����
  BindFlag_IndexBuffer  = 2,  //����� ����� ���� ����������� ��� ��������� �����
  BindFlag_Texture      = 4,  //����� ����� ���� ����������� ��� ��������
  BindFlag_RenderTarget = 8,  //����� ����� ��������� � �������� �������� ������ ���������
  BindFlag_DepthStencil = 16, //����� ����� ��������� � �������� dpeth-stencil ������ ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AccessFlag
{
  AccessFlag_Read  = 1, //������ �� ������
  AccessFlag_Write = 2 //������ �� ������
};

}

}

#endif
